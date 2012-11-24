/*
* linux/drivers/video/msm/mhl_v1/sii9234.c
*
* DESCRIPTION
*  This file explains the SiI9234 driver registration
*					and call the mhl function.
*
* Copyright (C) (2010, Samsung Electronics Co., Ltd.)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
*/

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/freezer.h>
#include <linux/fcntl.h>
#include <linux/sii9234.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include "Common_Def.h"
#include "sii9234_driver.h"

#define SUBJECT "MHL_DRIVER"

#define SII_DEV_DBG(format, ...)\
	pr_debug("[ "SUBJECT " (%s, %d) ] "format"\n", \
		 __func__, __LINE__, ## __VA_ARGS__);
/*
- Required by fsa9480 for calling sii9234 initialization
*/
int SII9234_i2c_status;
EXPORT_SYMBOL(SII9234_i2c_status);


static struct i2c_client *sii9234_i2c_client;
static struct i2c_client *sii9234a_i2c_client;
static struct i2c_client *sii9234b_i2c_client;
static struct i2c_client *sii9234c_i2c_client;

static struct i2c_device_id sii9234_id[] = {
	{"sii9234_mhl_tx", 0},
	{}
};

static struct i2c_device_id sii9234a_id[] = {
	{"sii9234_tpi", 0},
	{}
};

static struct i2c_device_id sii9234b_id[] = {
	{"sii9234_hdmi_rx", 0},
	{}
};

static struct i2c_device_id sii9234c_id[] = {
	{"sii9234_cbus", 0},
	{}
};

struct sii9234_state {
	struct i2c_client *client;
	/*check i2c initialization for each I2C module*/
	int mhl_i2c_init;
	int mhl_power_state;
	struct class *class;
	struct kobject *mhl_kobj;
};
static bool get_mhl_power_state(void);
#define EXIT_ON_CABLE_DISCONNECTION \
do {\
	if (!get_mhl_power_state()) {\
		SII_DEV_DBG("MHL Power off OR Cable disconnected\n\r");\
		return false;\
	 } \
} while (0)


static ssize_t sysfs_check_mhl_command(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int count;
	int res;
	struct sii9234_platform_data *sil9234c_pdata;
	struct sii9234_state *state = i2c_get_clientdata(sii9234c_i2c_client);
	sil9234c_pdata = sii9234c_i2c_client->dev.platform_data;

	sil9234c_pdata->hw_onoff(1);
	state->mhl_power_state = 1;
	sil9234c_pdata->hw_reset();

	msleep(300);

	res = sii9234_start_tpi();
	count = snprintf(buf, 100, "%d\n", res);
	sil9234c_pdata->hw_onoff(0);

	return count;

}

static ssize_t sysfs_change_switch_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	int rc, value = 0;
	struct sii9234_platform_data *sil9234C_pdata;
	sil9234C_pdata = sii9234c_i2c_client->dev.platform_data;

	rc = kstrtoul(buf, 0, (unsigned long *)&value);
	if (rc < 0)
		return rc;

	if ((sil9234C_pdata == NULL) || (sil9234C_pdata->gpio < 0)) {
		pr_err(" there is no mhl_sel in gpio\n");
		return size;
	}

	gpio_set_value_cansleep(sil9234C_pdata->gpio, (value == 1) ? 1 : 0);

	pr_info("gpio_level(mhl_sel) :%d\n",
		gpio_get_value_cansleep(sil9234C_pdata->gpio));

	return size;
}

static CLASS_ATTR(test_result, 0664 , sysfs_check_mhl_command,
			sysfs_change_switch_store);


static ssize_t sysfs_mhl_turn_onoff_store(struct device *dev,
		struct device_attribute *attr, char *buf, size_t size)
{
	int rc, value = 0;
	rc = kstrtoul(buf, 0, (unsigned long *)&value);
	if (rc < 0)
		return rc;

	mhl_onoff_ex(value == 1 ? 1 : 0);

	return size;
}

static DEVICE_ATTR(mhl_power, 0664 , NULL,
					 sysfs_mhl_turn_onoff_store);

/*
FSA9485 call this function to turn on or off MHL chip
*/
u8 mhl_onoff_ex(bool onoff)
{
	struct sii9234_platform_data *sil9234C_pdata;
	struct sii9234_state *state = i2c_get_clientdata(sii9234c_i2c_client);
	sil9234C_pdata = sii9234c_i2c_client->dev.platform_data;

	if (!sil9234C_pdata) {
		pr_info("mhl_onoff_ex: getting resource is failed\n");
		return 2;
	}

	if (onoff) {
		sil9234C_pdata->hw_onoff(1);
		sil9234C_pdata->hw_reset();
		state->mhl_power_state = 1;
		sii9234_initialize(state->mhl_kobj);
	} else {
		state->mhl_power_state = 0;
		sil9234C_pdata->hw_onoff(0);
	}
	return 2;
}
EXPORT_SYMBOL(mhl_onoff_ex);

struct i2c_client *get_sii9234_client(u8 device_id)
{

	struct i2c_client *client_ptr;

	if (device_id == 0x72)
		client_ptr = sii9234_i2c_client;
	else if (device_id == 0x7A)
		client_ptr = sii9234a_i2c_client;
	else if (device_id == 0x92)
		client_ptr = sii9234b_i2c_client;
	else if (device_id == 0xC8)
		client_ptr = sii9234c_i2c_client;
	else
		client_ptr = NULL;

	return client_ptr;
}
EXPORT_SYMBOL(get_sii9234_client);

u8 sii9234_i2c_read(struct i2c_client *client, u8 reg)
{
	int ret;
	struct sii9234_state *state = i2c_get_clientdata(client);

	if (!state->mhl_i2c_init) {
		SII_DEV_DBG("I2C not ready\n");
		return 0;
	}
	EXIT_ON_CABLE_DISCONNECTION;
	ret = i2c_smbus_write_byte(client, reg);
	ret = i2c_smbus_read_byte(client);
	if (ret < 0) {
		SII_DEV_DBG("i2c read fail\n");
		return -EIO;
	}
	return ret;

}
EXPORT_SYMBOL(sii9234_i2c_read);

int sii9234_i2c_write(struct i2c_client *client, u8 reg, u8 data)
{
	struct sii9234_state *state = i2c_get_clientdata(client);

	if (!state->mhl_i2c_init) {
		SII_DEV_DBG("I2C not ready\n");
		return 0;
	}
	EXIT_ON_CABLE_DISCONNECTION;
	return i2c_smbus_write_byte_data(client, reg, data);

}
EXPORT_SYMBOL(sii9234_i2c_write);

irqreturn_t mhl_int_irq_handler(int irq, void *dev_id)
{
	sii9234_interrupt_event();
	return IRQ_HANDLED;
}

static int sii9234_i2c_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	int ret = 0;
	struct sii9234_state *state;
	struct class *mhl_class;
	struct device *mhl_dev;

	SII_DEV_DBG("");

	state = kzalloc(sizeof(struct sii9234_state), GFP_KERNEL);
	if (state == NULL) {
		pr_err("failed to allocate memory\n");
		return -ENOMEM;
	}

	state->client = client;
	i2c_set_clientdata(client, state);

	/* rest of the initialisation goes here. */
	sii9234_i2c_client = client;

	state->mhl_i2c_init = 1;

	mhl_class = class_create(THIS_MODULE, "mhl");
	if (IS_ERR(mhl_class)) {
		pr_err("Failed to create class(mhl)!\n");
		goto err_out;
	}
	state->class = mhl_class;
	ret = class_create_file(mhl_class, &class_attr_test_result);
	if (ret) {
		pr_err("Failed to create device file in sysfs entries!\n");
		goto err_out_class;
	}

	mhl_dev = device_create(mhl_class, NULL, 0, NULL, "mhl_dev");
	if (IS_ERR(mhl_dev)) {
		pr_err("Failed to create device(mhl_dev)!\n");
		goto err_out_class;
	}

	ret = device_create_file(mhl_dev, &dev_attr_mhl_power);
	if (ret) {
		pr_err("Failed to create device file in sysfs entries(%s)!\n",
						dev_attr_mhl_power.attr.name);
		goto err_out_dev;
	}

	return ret;

err_out_dev:
	device_destroy(mhl_class, 0);
err_out_class:
	class_destroy(mhl_class);
err_out:
	kfree(state);

	return ret;
}

static int __devexit sii9234_remove(struct i2c_client *client)
{
	struct sii9234_state *state = i2c_get_clientdata(client);

	device_destroy(state->class, 0);
	class_destroy(state->class);
	kfree(state);

	return 0;
}

static int sii9234a_i2c_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct sii9234_state *state;

	SII_DEV_DBG("");

	state = kzalloc(sizeof(struct sii9234_state), GFP_KERNEL);
	if (state == NULL) {
		pr_err("failed to allocate memory\n");
		return -ENOMEM;
	}

	state->client = client;
	i2c_set_clientdata(client, state);

	/* rest of the initialisation goes here. */
	sii9234a_i2c_client = client;
	state->mhl_i2c_init = 1;

	return 0;

}

static int __devexit sii9234a_remove(struct i2c_client *client)
{
	struct sii9234_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}

static int sii9234b_i2c_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct sii9234_state *state;

	SII_DEV_DBG("");

	state = kzalloc(sizeof(struct sii9234_state), GFP_KERNEL);
	if (state == NULL) {
		pr_err("failed to allocate memory\n");
		return -ENOMEM;
	}

	state->client = client;
	i2c_set_clientdata(client, state);

	/* rest of the initialisation goes here. */
	sii9234b_i2c_client = client;
	state->mhl_i2c_init = 1;

	return 0;

}

static int __devexit sii9234b_remove(struct i2c_client *client)
{
	struct sii9234_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}

static void mhl_i2c_client_info(void)
{
	pr_err("sii9234_i2c_client name = %s, device_id = 0x%x\n",
			sii9234_i2c_client->name, sii9234_i2c_client->addr);
	pr_err("sii9234a_i2c_client name = %s, device_id = 0x%x\n",
			sii9234a_i2c_client->name, sii9234a_i2c_client->addr);
	pr_err("sii9234b_i2c_client name = %s, device_id = 0x%x\n",
			sii9234b_i2c_client->name, sii9234b_i2c_client->addr);
	pr_err("sii9234c_i2c_client name = %s, device_id = 0x%x\n",
			sii9234c_i2c_client->name, sii9234c_i2c_client->addr);
}

static int sii9234c_i2c_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	int ret = 0;
	struct sii9234_state *state = NULL;
	struct sii9234_platform_data *sil9234c_pdata;
	int mhl_int_irq;

	SII_DEV_DBG("");

	state = kzalloc(sizeof(struct sii9234_state), GFP_KERNEL);
	if (state == NULL) {
		pr_err("failed to allocate memory\n");
		return -ENOMEM;
	}

	state->client = client;
	state->mhl_kobj = &(client->dev.kobj);
	i2c_set_clientdata(client, state);
	/* rest of the initialisation goes here. */
	sii9234c_i2c_client = client;
	state->mhl_i2c_init = 1;

	sil9234c_pdata = client->dev.platform_data;
	if ((sil9234c_pdata == NULL) || (sil9234c_pdata->get_irq == NULL)) {
		pr_err("failed to get get resource - irq\n");
		goto failed_free_mem;
	}

	mhl_int_irq = sil9234c_pdata->get_irq();
	irq_set_irq_type(mhl_int_irq, IRQ_TYPE_EDGE_RISING);

	ret = request_threaded_irq(mhl_int_irq, NULL, mhl_int_irq_handler,
				 IRQF_DISABLED, "mhl_int", (void *)state);
	if (ret < 0) {
		pr_err("unable to request irq mhl_int err:: %d\n", ret);
		goto failed_free_mem;
	}

	SII9234_i2c_status = 1;
	mhl_i2c_client_info();

	return ret;

failed_free_mem:
	kfree(state);
	return ret;

}

static int __devexit sii9234c_remove(struct i2c_client *client)
{
	struct sii9234_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}


struct i2c_driver sii9234_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9234_mhl_tx",
	},
	.id_table	= sii9234_id,
	.probe	= sii9234_i2c_probe,
	.remove	= __devexit_p(sii9234_remove),
	.command = NULL,
};

struct i2c_driver sii9234a_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9234_tpi",
	},
	.id_table	= sii9234a_id,
	.probe	= sii9234a_i2c_probe,
	.remove	= __devexit_p(sii9234a_remove),
	.command = NULL,
};

struct i2c_driver sii9234b_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9234_hdmi_rx",
	},
	.id_table	= sii9234b_id,
	.probe	= sii9234b_i2c_probe,
	.remove	= __devexit_p(sii9234b_remove),
	.command = NULL,
};

struct i2c_driver sii9234c_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9234_cbus",
	},
	.id_table	= sii9234c_id,
	.probe	= sii9234c_i2c_probe,
	.remove	= __devexit_p(sii9234c_remove),
	.command = NULL,
};


static bool get_mhl_power_state(void)
{
	struct sii9234_state *state = i2c_get_clientdata(sii9234c_i2c_client);
	return state->mhl_power_state;
}

static int __init sii9234_init(void)
{
	int ret;

	ret = i2c_add_driver(&sii9234_i2c_driver);
	if (ret != 0)
		goto init_err;
	ret = i2c_add_driver(&sii9234a_i2c_driver);
	if (ret != 0)
		goto init_i2c_err1;
	ret = i2c_add_driver(&sii9234b_i2c_driver);
	if (ret != 0)
		goto init_i2c_err2;
	ret = i2c_add_driver(&sii9234c_i2c_driver);
	if (ret != 0)
		goto init_i2c_err3;

	return ret;

init_i2c_err3:
	i2c_del_driver(&sii9234b_i2c_driver);
init_i2c_err2:
	i2c_del_driver(&sii9234a_i2c_driver);
init_i2c_err1:
	i2c_del_driver(&sii9234_i2c_driver);
init_err:
	pr_err("i2c_add_driver is failed - mhl\n");
	return ret;
}
module_init(sii9234_init);

static void __exit sii9234_exit(void)
{
	i2c_del_driver(&sii9234_i2c_driver);
	i2c_del_driver(&sii9234a_i2c_driver);
	i2c_del_driver(&sii9234b_i2c_driver);
	i2c_del_driver(&sii9234c_i2c_driver);
};
module_exit(sii9234_exit);

MODULE_DESCRIPTION("Sii9234 MHL driver");
MODULE_AUTHOR("Aakash Manik");
MODULE_LICENSE("GPL");
