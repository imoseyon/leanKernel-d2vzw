/*
 * Copyright (c) 2010 SAMSUNG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */


#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/i2c/gp2a.h>
#include <linux/slab.h>
#include <linux/regulator/consumer.h>
#include <linux/mfd/pm8921-adc.h>
#include <mach/vreg.h>
#include <linux/workqueue.h>


/* for debugging */

#define gprintk(x...) do { } while (0)

#define SENSOR_NAME "light_sensor"
#define SENSOR_DEFAULT_DELAY            (200)   /* 200 ms */
#define SENSOR_MAX_DELAY                (2000)  /* 2000 ms */
#define ABS_STATUS                      (ABS_BRAKE)
#define ABS_WAKE                        (ABS_MISC)
#define ABS_CONTROL_REPORT              (ABS_THROTTLE)

#define LIGHT_BUFFER_UP	5
#define LIGHT_BUFFER_DOWN	20


#define MSM_LIGHTSENSOR_ADC_READ

static struct sensor_data {
	struct input_dev *input_dev;
	struct mutex mutex;
	struct delayed_work work;
	struct class *lightsensor_class;
	struct device *switch_cmd_dev;

	int enabled;
	int delay;

	state_type light_data;
	int testmode;
	int light_buffer;
	int light_count;
	int light_level_state;
	bool light_first_level;
};


static const int adc_table[4] = {
	15,
	163,
	1650,
	15700,
};


#ifdef MSM_LIGHTSENSOR_ADC_READ
static unsigned int lightsensor_get_adc(int channel)
{
	int rc;
	struct pm8921_adc_chan_result result;

	rc = pm8921_adc_read(channel, &result);
	if (rc) {
		pr_err("error reading adc channel = %d, rc = %d\n",
					channel, rc);
		return rc;
	}
	pr_debug("light sensor = %lld meas = 0x%llx\n", result.physical,
						result.measurement);
	return (int)result.physical;

}
#endif

static int lightsensor_get_adcvalue(void);

static struct workqueue_struct *light_workqueue;

/* global var */
static struct platform_device *sensor_pdev;

static int cur_adc_value;

static int state_to_lux(state_type state)
{
	int lux = 0;

	if (state == LIGHT_LEVEL5)
		lux = 15000;
	else if (state == LIGHT_LEVEL4)
		lux = 9000;
	else if (state == LIGHT_LEVEL3)
		lux = 5000;
	else if (state == LIGHT_LEVEL2)
		lux = 1000;
	else if (state == LIGHT_LEVEL1)
		lux = 6;
	else
		lux = 5000;

	return lux;
}

static ssize_t lightsensor_file_state_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int adc = 0;

	adc = lightsensor_get_adcvalue();
	printk(KERN_ERR "%s : adc(%d)\n", __func__, adc);

	return snprintf(buf, PAGE_SIZE, "%d\n", adc);
}

/* Light Sysfs interface */
static ssize_t
light_delay_show(struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->delay);
}

static ssize_t
light_delay_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);
	int delay = simple_strtoul(buf, NULL, 10);

	if (delay < 0)
		return count;

	if (SENSOR_MAX_DELAY < delay)
		delay = SENSOR_MAX_DELAY;

	data->delay = delay;

	mutex_lock(&data->mutex);

	if (data->enabled)	{
		cancel_delayed_work_sync(&data->work);
		queue_delayed_work(light_workqueue, &data->work,
			msecs_to_jiffies(delay));
	}

	input_report_abs(input_data, ABS_CONTROL_REPORT,
		(data->delay<<16) | delay);

	mutex_unlock(&data->mutex);

	return count;
}

static ssize_t
light_enable_show(struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->enabled);
}

static ssize_t
light_enable_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);
	int value = simple_strtoul(buf, NULL, 10);

	if (value != 0 && value != 1)
		return count;


	mutex_lock(&data->mutex);

	if (data->enabled && !value) {
		data->enabled = value;
		/* sync with gp2a_work_func_light function */
		cancel_delayed_work_sync(&data->work);
		gprintk("timer canceled.\n");
	}
	if (!data->enabled && value) {
		data->enabled = value;
		/* sync with gp2a_work_func_light function */
		queue_delayed_work(light_workqueue, &data->work, 0);
		gprintk("timer started.\n");
	}

	input_report_abs(input_data, ABS_CONTROL_REPORT,
		(value<<16) | data->delay);

	mutex_unlock(&data->mutex);

	return count;
}

static ssize_t
light_wake_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct input_dev *input_data = to_input_dev(dev);
	static int cnt = 1;

	input_report_abs(input_data, ABS_WAKE, cnt++);

	return count;
}

static ssize_t
light_data_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);

	int light_lux;

	mutex_lock(&data->mutex);
	light_lux = state_to_lux(data->light_level_state);
	mutex_unlock(&data->mutex);

	return snprintf(buf, PAGE_SIZE, "%d\n", light_lux);
}

static ssize_t light_testmode_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);
	int value;

	value = simple_strtoul(buf, NULL, 10);

	if (value == 1) {
		data->testmode = 1;
		gprintk("lightsensor testmode ON.\n");
	} else if (value == 0) {
		data->testmode  = 0;
		gprintk("lightsensor testmode OFF.\n");
	}

	return size;
}

static ssize_t light_testmode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct input_dev *input_data = to_input_dev(dev);
	struct sensor_data *data = input_get_drvdata(input_data);

	gprintk(" : %d\n", data->testmode);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->testmode);
}

static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP, light_delay_show,
	light_delay_store);
static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP, light_enable_show,
	light_enable_store);
static DEVICE_ATTR(wake, S_IWUSR|S_IWGRP, NULL, light_wake_store);
static DEVICE_ATTR(lux, S_IRUGO, light_data_show, NULL);
static DEVICE_ATTR(testmode, S_IRUGO|S_IWUSR|S_IWGRP, light_testmode_show,
	light_testmode_store);
static DEVICE_ATTR(adc, S_IRUGO|S_IWUSR|S_IWGRP,
	lightsensor_file_state_show, NULL);

static struct attribute *lightsensor_attributes[] = {
	&dev_attr_delay.attr,
	&dev_attr_enable.attr,
	&dev_attr_wake.attr,
	&dev_attr_lux.attr,
	&dev_attr_testmode.attr,
	&dev_attr_adc.attr,
	NULL
};

static struct device_attribute *lightsensor_additional_attributes[] = {
	&dev_attr_enable.attr,
	&dev_attr_lux.attr,
	&dev_attr_adc.attr,
	NULL
};


static struct attribute_group lightsensor_attribute_group = {
	.attrs = lightsensor_attributes
};

extern int sensors_register(struct device *dev, void * drvdata,
	struct device_attribute *attributes[], char *name);

static int
lightsensor_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct sensor_data *data = platform_get_drvdata(pdev);

	int rt = 0;

	mutex_lock(&data->mutex);

	if (data->enabled) {
		rt = cancel_delayed_work_sync(&data->work);
		gprintk(": The timer is cancled.\n");
	}

	mutex_unlock(&data->mutex);

	return rt;
}

static int
lightsensor_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct sensor_data *data = platform_get_drvdata(pdev);
	int rt = 0;

	data->light_count = 0;
	data->light_buffer = 0;
	data->light_first_level = true;

	mutex_lock(&data->mutex);

	if (data->enabled) {
		rt = queue_delayed_work(light_workqueue, &data->work, 0);
		gprintk(": The timer is started.\n");
	}

	mutex_unlock(&data->mutex);

	return rt;
}

static int lightsensor_get_adcvalue(void)
{
	int i = 0;
	unsigned int adc_total = 0;
	static int adc_avr_value;
	unsigned int adc_index = 0;
	static unsigned int adc_index_count;
	unsigned int adc_max = 0;
	unsigned int adc_min = 0;
	int value = 0;
	static int adc_value_buf[ADC_BUFFER_NUM] = {0};

	/* get ADC */
#ifdef MSM_LIGHTSENSOR_ADC_READ
	value = lightsensor_get_adc(CHANNEL_MPP_2);
#endif
	cur_adc_value = value;

	adc_index = (adc_index_count++)%ADC_BUFFER_NUM;

	adc_value_buf[adc_index] = value;

	adc_max = adc_value_buf[0];
	adc_min = adc_value_buf[0];

	for (i = 0; i < ADC_BUFFER_NUM; i++) {
		adc_total += adc_value_buf[i];

		if (adc_max < adc_value_buf[i])
			adc_max = adc_value_buf[i];

		if (adc_min > adc_value_buf[i])
			adc_min = adc_value_buf[i];
	}
	adc_avr_value = (adc_total-(adc_max+adc_min))/(ADC_BUFFER_NUM-2);

	if (adc_index_count == ADC_BUFFER_NUM-1)
		adc_index_count = 0;

	return adc_avr_value;
}

static void gp2a_work_func_light(struct work_struct *work)
{
	struct sensor_data *data = container_of(
		(struct delayed_work *)work, struct sensor_data, work);

	static int lightsensor_log_cnt;
	int i;
	int adc = 0;

	adc = lightsensor_get_adcvalue();

	for (i = 0; ARRAY_SIZE(adc_table); i++)
		if (adc <= adc_table[i])
			break;

	if (data->light_buffer == i) {
		if (data->light_level_state <= i ||
			data->light_first_level == true) {
			if (data->light_count++ == LIGHT_BUFFER_UP) {
				if (lightsensor_log_cnt == 10) {
					printk(KERN_ERR "[LIGHT SENSOR] lux up 0x%0X (%d)\n",
						adc, adc);
					lightsensor_log_cnt = 0;
					}

				lightsensor_log_cnt++;
				input_report_abs(data->input_dev,
					ABS_MISC, adc);
				input_sync(data->input_dev);
				data->light_count = 0;
				data->light_first_level = false;
				data->light_level_state = data->light_buffer;
			}
		} else{
		if (data->light_count++ == LIGHT_BUFFER_DOWN) {
			if (lightsensor_log_cnt == 10) {
				printk(KERN_ERR "[LIGHT SENSOR] lux down 0x%0X (%d)\n",
					adc, adc);
				lightsensor_log_cnt = 0;
				}

				lightsensor_log_cnt++;
					input_report_abs(data->input_dev,
						ABS_MISC, adc);
					input_sync(data->input_dev);
					data->light_count = 0;
					data->light_level_state =
						data->light_buffer;
				}
		}
	} else {
		data->light_buffer = i;
		data->light_count = 0;
	}

	if (data->enabled)
		queue_delayed_work(light_workqueue, &data->work,
			msecs_to_jiffies(data->delay));
}


static int
lightsensor_probe(struct platform_device *pdev)
{
	struct sensor_data *data = NULL;
	struct input_dev *input_data = NULL;
	static struct device *light_sensor_device;
	int rt;

	data = kzalloc(sizeof(struct sensor_data), GFP_KERNEL);
	if (!data) {
		rt = -ENOMEM;
		goto err;
	}
	data->enabled = 0;
	data->delay = SENSOR_DEFAULT_DELAY;
	data->testmode = 0;
	data->light_level_state = 0;

	light_workqueue = create_singlethread_workqueue("klightd");
	if (!light_workqueue) {
		rt = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate work queue\n",
			__func__);
		goto err;
	}

	INIT_DELAYED_WORK(&data->work, gp2a_work_func_light);

	input_data = input_allocate_device();
	if (!input_data) {
		rt = -ENOMEM;
		printk(KERN_ERR
			"sensor_probe: Failed to allocate input_data device\n");
		goto err;
	}

	set_bit(EV_ABS, input_data->evbit);
	input_set_capability(input_data, EV_ABS, ABS_X);
	input_set_capability(input_data, EV_ABS, ABS_MISC);
	input_set_capability(input_data, EV_ABS, ABS_WAKE); /* wake */
	input_set_capability(input_data, EV_ABS, ABS_CONTROL_REPORT);
	/* enabled/delay */
	input_set_abs_params(input_data, ABS_MISC, 0, 1, 0, 0);
	input_data->name = SENSOR_NAME;

	rt = input_register_device(input_data);
	if (rt) {
		printk(KERN_ERR
			"sensor_probe: Unable to register input_data device: %s\n",
			input_data->name);
		goto err;
	}
	input_set_drvdata(input_data, data);

	rt = sysfs_create_group(&input_data->dev.kobj,
				&lightsensor_attribute_group);
	if (rt) {
		printk(KERN_ERR
			"sensor_probe: sysfs_create_group failed[%s]\n",
			input_data->name);
		goto err_input_registered;
	}
	mutex_init(&data->mutex);
	data->input_dev = input_data;

	platform_set_drvdata(pdev, data);

	rt = sensors_register(light_sensor_device, data,
		lightsensor_additional_attributes, "light_sensor");

	if (rt) {

		pr_err("%s: cound not register sensor device\n", __func__);

		goto err_sysfs_created;

	}

	dev_set_drvdata(data->switch_cmd_dev, data);
	return 0;

err_sysfs_created:
	sysfs_remove_group(&input_data->dev.kobj,
		&lightsensor_attribute_group);
err_input_registered:
	input_unregister_device(input_data);
err:
	if (light_workqueue)
		destroy_workqueue(light_workqueue);
	if (data != NULL) {
		if (input_data != NULL) {
			input_free_device(input_data);
			input_data = NULL;
		}
		kfree(data);
	}
	return rt;
}

static int lightsensor_remove(struct platform_device *pdev)
{
	struct sensor_data *data = platform_get_drvdata(pdev);

	int rt = 0;
	if (data->input_dev != NULL) {
		data->enabled = 0;
		sysfs_remove_group(&data->input_dev->dev.kobj,
			&lightsensor_attribute_group);
		cancel_delayed_work(&data->work);
		flush_workqueue(light_workqueue);
		destroy_workqueue(light_workqueue);
		input_unregister_device(data->input_dev);
		kfree(data->input_dev);
	}
	kfree(data);
	return rt;
}


/*
 * Module init and exit
 */

static const struct dev_pm_ops lightsensor_dev_pm_ops = {
	.suspend = lightsensor_suspend,
	.resume = lightsensor_resume,
};

static struct platform_driver lightsensor_driver = {
	.probe      = lightsensor_probe,
	.remove     = lightsensor_remove,
	.driver = {
		.name   = SENSOR_NAME,
		.owner  = THIS_MODULE,
		.pm = &lightsensor_dev_pm_ops,
	},
};

static int __init lightsensor_init(void)
{
	sensor_pdev = platform_device_register_simple(SENSOR_NAME, 0, NULL, 0);
	if (IS_ERR(sensor_pdev))
		return IS_ERR(sensor_pdev);

	return platform_driver_register(&lightsensor_driver);
}
module_init(lightsensor_init);

static void __exit lightsensor_exit(void)
{
	platform_driver_unregister(&lightsensor_driver);
	platform_device_unregister(sensor_pdev);
}
module_exit(lightsensor_exit);

MODULE_AUTHOR("SAMSUNG");
MODULE_DESCRIPTION("Optical Sensor driver for GP2AP002A00F");
MODULE_LICENSE("GPL");
