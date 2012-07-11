
/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <mach/irqs.h>
#include <linux/log2.h>
#include <linux/spinlock.h>
#include <linux/hrtimer.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <mach/gpio.h>
#include <linux/interrupt.h>
#include <linux/switch.h>

#include <linux/wakelock.h>

#define FLIP_DET_PIN 23
#define FLIP_NOTINIT -1
#define FLIP_OPEN 1
#define FLIP_CLOSE 0

#define FLIP_SCAN_INTERVAL (50)
#define FLIP_STABLE_COUNT (1)

#define dbg_printk(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)


struct sec_flip_pdata {
	void (*pmic_gpio_config) (void);
	int wakeup;
	int gpio;
};

struct sec_flip {
	struct input_dev *input;
	struct sec_flip_pdata *pdata;
	struct wake_lock wlock;
	struct timer_list flip_timer;

	int flip_status;
	int gpio;
	int irq;

	struct mutex gpio_get_lock;
};

struct switch_dev switch_flip = {
	.name = "flip",
};

static void sec_report_flip_key(struct sec_flip *flip)
{
	switch_set_state(&switch_flip, !flip->flip_status);

	if (flip->flip_status) {
		input_report_key(flip->input, KEY_FOLDER_OPEN, 1);
		input_report_key(flip->input, KEY_FOLDER_OPEN, 0);
		input_sync(flip->input);
	} else {
		input_report_key(flip->input, KEY_FOLDER_CLOSE, 1);
		input_report_key(flip->input, KEY_FOLDER_CLOSE, 0);
		input_sync(flip->input);
	}
}

static void set_flip_status(struct sec_flip *flip)
{
	int val = 0;

	val = gpio_get_value_cansleep(flip->gpio);
	flip->flip_status = val ? 1 : 0;
}

static irqreturn_t sec_flip_irq_handler(int irq, void *_flip)
{
	struct sec_flip *flip = _flip;
	unsigned long flags;

	mutex_lock(&flip->gpio_get_lock);
	wake_lock_timeout(&flip->wlock, 1 * HZ);
	set_flip_status(flip);
	mutex_unlock(&flip->gpio_get_lock);
	sec_report_flip_key(flip);

	return IRQ_HANDLED;
}

static void sec_flip_timer(unsigned long data)
{
	int val = 0;
	struct sec_flip* flip = (struct sec_flip *)data;
	static int wait_flip_count;
	static int wait_flip_status;

	val = gpio_get_value_cansleep(flip->gpio);
	if (val != wait_flip_status) {
		wait_flip_count = 0;
		wait_flip_status = val;
	} else if (wait_flip_count < FLIP_STABLE_COUNT) {
		wait_flip_count++;
	}
	if (wait_flip_count >= FLIP_STABLE_COUNT) {
		if (val)
			flip->flip_status = 1;
		else
			flip->flip_status = 0;

		sec_report_flip_key(flip);
	} else {
		mod_timer(&flip->flip_timer,
			jiffies + msecs_to_jiffies(FLIP_SCAN_INTERVAL));
	}
}

static int sec_flip_suspend(struct device *dev)
{
	struct sec_flip *flip = dev_get_drvdata(dev);

	if (device_may_wakeup(dev))
		enable_irq_wake(flip->irq);

	return 0;
}

static int sec_flip_resume(struct device *dev)
{
	struct sec_flip *flip = dev_get_drvdata(dev);

	if (device_may_wakeup(dev))
		disable_irq_wake(flip->irq);

	return 0;
}

static const struct dev_pm_ops pm8921_flip_pm_ops = {
	.suspend = sec_flip_suspend,
	.resume = sec_flip_resume,
};

static ssize_t status_check(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct sec_flip *flip = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", flip->flip_status);
}

static DEVICE_ATTR(flipStatus, S_IRUGO | S_IWUSR | S_IWGRP ,
	status_check, NULL);

static int __devinit sec_flip_probe(struct platform_device *pdev)
{
	struct input_dev *input;
	int err;
	struct sec_flip *flip;
	struct sec_flip_pdata *pdata = pdev->dev.platform_data;

	dev_info(&pdev->dev, "probe\n");

	if (!pdata) {
		dev_err(&pdev->dev, "power key platform data not supplied\n");
		return -EINVAL;
	}

	flip = kzalloc(sizeof(*flip), GFP_KERNEL);
	if (!flip)
		return -ENOMEM;

	flip->pdata   = pdata;

	/* Enable runtime PM ops, start in ACTIVE mode */
	err = pm_runtime_set_active(&pdev->dev);
	if (err < 0) {
		dev_err(&pdev->dev, "unable to set runtime pm state\n");
		goto free_flip;
	}
	pm_runtime_enable(&pdev->dev);

	/* INPUT DEVICE */
	input = input_allocate_device();
	if (!input) {
		dev_err(&pdev->dev, "Can't allocate power button\n");
		err = -ENOMEM;
		goto free_pm;
	}

	set_bit(EV_KEY, input->evbit);
	set_bit(KEY_FOLDER_OPEN & KEY_MAX, input->keybit);
	set_bit(KEY_FOLDER_CLOSE & KEY_MAX, input->keybit);

	input->name = "sec_hall_key";
	input->phys = "sec_hall/input0";
	input->dev.parent = &pdev->dev;

	err = input_register_device(input);
	if (err) {
		dev_err(&pdev->dev, "Can't register power key: %d\n", err);
		goto free_input_dev;
	}
	flip->input = input;

	input_set_capability(flip->input, EV_ABS, ABS_MISC);
	input_set_abs_params(flip->input, ABS_MISC, 0, 1, 0, 0);

	platform_set_drvdata(pdev, flip);

	err = switch_dev_register(&switch_flip);
	if (err < 0) {
		printk(KERN_ERR "FLIP: Failed to register switch device\n");
		goto free_input_dev;
	}

	/* INTERRUPT */
	flip->gpio = pdata->gpio;

	if (pdata->pmic_gpio_config)
		pdata->pmic_gpio_config();


	flip->irq = gpio_to_irq(flip->gpio);

	mutex_init(&flip->gpio_get_lock);

	err = request_threaded_irq(flip->irq, NULL, sec_flip_irq_handler,
		(IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING),
		"flip_det_irq", flip);
	if (err < 0) {
		dev_err(&pdev->dev, "Can't get %d IRQ for flip: %d\n",
		flip->irq, err);
		goto unreg_input_dev;
	}

	device_init_wakeup(&pdev->dev, pdata->wakeup);

	wake_lock_init(&flip->wlock, WAKE_LOCK_SUSPEND, "sec_flip");

	err = device_create_file(&pdev->dev, &dev_attr_flipStatus);
	if (err < 0) {
		printk(KERN_ERR "flip status check cannot create file :	%d\n",
			flip->flip_status);
		goto unreg_input_dev;
	}

	init_timer(&flip->flip_timer);
	flip->flip_timer.function = sec_flip_timer;
	flip->flip_timer.data = (unsigned long)flip;

	mod_timer(&flip->flip_timer, jiffies + msecs_to_jiffies(5000));

	return 0;

err_flip:
	del_timer_sync(&flip->flip_timer);
	switch_dev_unregister(&switch_flip);

unreg_input_dev:
	input_unregister_device(input);
free_input_dev:
	input_free_device(input);
free_pm:
	pm_runtime_set_suspended(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
free_flip:
	kfree(flip);
	return err;
}

static int __devexit sec_flip_remove(struct platform_device *pdev)
{
	struct sec_flip *flip = platform_get_drvdata(pdev);

	printk(KERN_DEBUG "%s:\n", __func__);

	if (flip != NULL)
		del_timer_sync(&flip->flip_timer);
	switch_dev_unregister(&switch_flip);

	pm_runtime_set_suspended(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
	device_init_wakeup(&pdev->dev, 0);

	if (flip != NULL) {
		free_irq(flip->irq, NULL);
		input_unregister_device(flip->input);
		kfree(flip);
	}

	return 0;
}

static struct platform_driver sec_flip_driver = {
	.probe = sec_flip_probe,
	.remove = __devexit_p(sec_flip_remove),
	.driver = {
		.name = "hall_sw",
		.owner = THIS_MODULE,
		.pm = &pm8921_flip_pm_ops,
	},
};

static int __init sec_flip_init(void)
{
	return platform_driver_register(&sec_flip_driver);
}

static void __exit sec_flip_exit(void)
{
	platform_driver_unregister(&sec_flip_driver);
}

late_initcall(sec_flip_init);
module_exit(sec_flip_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("PMIC8921 Hall Switch");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:hall_sw");
