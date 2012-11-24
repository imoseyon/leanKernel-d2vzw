/*
 *  tablet accessory detection driver.
 *
 *  Copyright (C) 2012 Samsung Electronics Co.Ltd
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/sec_tablet_conn.h>
#include <linux/switch.h>
#include <linux/wakelock.h>
#include <asm/irq.h>

#ifdef CONFIG_VIDEO_MHL_TAB_V2
#include "mhl_tab_v2/sii9234_driver.h"
#endif
#define SUBJECT "ACCESSORY"

#define ACC_CONDEV_DBG(format, ...) \
	pr_info("[ "SUBJECT " (%s,%d) ] " format "\n", \
		__func__, __LINE__, ## __VA_ARGS__);

#define DETECTION_INTR_DELAY	(get_jiffies_64() + (HZ*15)) /* 20s */

enum accessory_type {
	ACCESSORY_NONE = 0,
	ACCESSORY_OTG,
	ACCESSORY_LINEOUT,
#ifdef CONFIG_CAMERON_HEALTH
	ACCESSORY_CAMERON,
#endif
	ACCESSORY_CARMOUNT,
	ACCESSORY_UNKNOWN,
};

enum dock_type {
	DOCK_NONE = 0,
	DOCK_DESK,
	DOCK_KEYBOARD,
};

enum uevent_dock_type {
	UEVENT_DOCK_NONE = 0,
	UEVENT_DOCK_DESK,
	UEVENT_DOCK_CAR,
	UEVENT_DOCK_KEYBOARD = 9,
	UEVENT_DOCK_CONNECTED = 255,
};

struct acc_con_info {
	struct device *acc_dev;
	struct acc_con_platform_data *pdata;
	struct delayed_work acc_dwork;
	struct delayed_work acc_id_dwork;
	struct switch_dev dock_switch;
	struct switch_dev ear_jack_switch;
	struct wake_lock wake_lock;
	struct mutex lock;
	enum accessory_type current_accessory;
	enum dock_type current_dock;
	int accessory_irq;
	int dock_irq;
	int mhl_irq;
};

static int check_using_stmpe811_adc(void)
{
	return true;
}

static int connector_detect_change(void)
{
	int i;
	u32 adc = 0, adc_sum = 0;
	u32 adc_buff[5] = {0};
	u32 mili_volt;
	u32 adc_min = 0;
	u32 adc_max = 0;

	for (i = 0; i < 5; i++) {
		mili_volt = acc_get_adc_value();

		adc_buff[i] = mili_volt;
		adc_sum += adc_buff[i];
		if (i == 0) {
			adc_min = adc_buff[0];
			adc_max = adc_buff[0];
		} else {
			if (adc_max < adc_buff[i])
				adc_max = adc_buff[i];
			else if (adc_min > adc_buff[i])
				adc_min = adc_buff[i];
		}
		msleep(20);
	}
	adc = (adc_sum - adc_max - adc_min)/3;
	ACC_CONDEV_DBG("ACCESSORY_ID : ADC value = %d\n", adc);
	return (int)adc;
}

void acc_notified(struct acc_con_info *acc, int acc_adc)
{
	enum accessory_type current_accessory = ACCESSORY_NONE;
	char *env_ptr;
	char *stat_ptr;
	char *envp[3];
/*
  value is changed for Espresso
  3 pole earjack  1.00 V ( 0.90~1.10V)   adc: 1207~1256
  Car mount        1.38 V (1.24~1.45V)   adc: 1680~1749
  4 pole earjack   just bundles is supported . adc :2189~2278 : No Warranty
  OTG                 2.2 V  (2.00~2.35V)    adc: 2676~2785
*/

	if (acc_adc != false) {
		if (check_using_stmpe811_adc()) {
			if ((1207 < acc_adc) && (1256 > acc_adc)) {
				env_ptr = "ACCESSORY=lineout";
				current_accessory = ACCESSORY_LINEOUT;
			} else if ((1680 < acc_adc) && (1749 > acc_adc)) {
				env_ptr = "ACCESSORY=carmount";
				acc->current_accessory = ACCESSORY_CARMOUNT;
			} else if ((2189 < acc_adc) && (2278 > acc_adc)) {
				env_ptr = "ACCESSORY=lineout";
				current_accessory = ACCESSORY_LINEOUT;
#ifdef CONFIG_CAMERON_HEALTH
			} else if ((2400 < acc_adc) && (2500 > acc_adc)) {
				env_ptr = "ACCESSORY=cameron";
				current_accessory = ACCESSORY_CAMERON;
#endif
			} else if ((2676 < acc_adc) && (2785 > acc_adc)) {
				env_ptr = "ACCESSORY=OTG";
				current_accessory = ACCESSORY_OTG;
			} else {
				env_ptr = "ACCESSORY=unknown";
				current_accessory = ACCESSORY_UNKNOWN;
			}
		} else { /* not stmpe adc */
			if ((797 < acc_adc) && (1002 > acc_adc)) {
				env_ptr = "ACCESSORY=lineout";
				current_accessory = ACCESSORY_LINEOUT;
			} else if ((1134 < acc_adc) && (1352 > acc_adc)) {
				env_ptr = "ACCESSORY=carmount";
				current_accessory = ACCESSORY_CARMOUNT;
			} else if ((1400 < acc_adc) && (1690 > acc_adc)) {
				env_ptr = "ACCESSORY=lineout";
				current_accessory = ACCESSORY_LINEOUT;
			} else if ((1900 < acc_adc) && (2250 > acc_adc)) {
				env_ptr = "ACCESSORY=OTG";
				current_accessory = ACCESSORY_OTG;
			} else {
				env_ptr = "ACCESSORY=unknown";
				current_accessory = ACCESSORY_UNKNOWN;
			}
		}

		if (current_accessory == acc->current_accessory) {
			ACC_CONDEV_DBG("same accessory type is connected %d",
				current_accessory);
			return;
		}

		if (acc->current_accessory != ACCESSORY_NONE) {
			ACC_CONDEV_DBG("assuming prev accessory "
				"disconnected %d", acc->current_accessory);

			if (acc->current_accessory == ACCESSORY_OTG)
				envp[0] = "ACCESSORY=OTG";
			else if (acc->current_accessory == ACCESSORY_LINEOUT)
				envp[0] = "ACCESSORY=lineout";
			else if (acc->current_accessory == ACCESSORY_CARMOUNT)
				envp[0] = "ACCESSORY=carmount";
#ifdef CONFIG_CAMERON_HEALTH
			else if (acc->current_accessory == ACCESSORY_CAMERON)
				envp[0] = "ACCESSORY=cameron";
#endif
			else
				envp[0] = "ACCESSORY=unknown";

			envp[1] = "STATE=offline";
			envp[2] = NULL;
			kobject_uevent_env(&acc->acc_dev->kobj,
				KOBJ_CHANGE, envp);
			if ((acc->current_accessory == ACCESSORY_OTG) &&
				acc->pdata->otg_en)
				acc->pdata->otg_en(0);

			if (acc->current_accessory == ACCESSORY_LINEOUT)
				switch_set_state(&acc->ear_jack_switch,
					UEVENT_DOCK_NONE);

			ACC_CONDEV_DBG("%s : %s", envp[0], envp[1]);
		}

		acc->current_accessory = current_accessory;

		stat_ptr = "STATE=online";
		envp[0] = env_ptr;
		envp[1] = stat_ptr;
		envp[2] = NULL;

		if (acc->current_accessory == ACCESSORY_OTG) {
			/* force acc power off to ensure otg detection */
			if (acc->pdata->acc_power)
				acc->pdata->acc_power(0, false);
			msleep(20);

			if (acc->pdata->otg_en)
				acc->pdata->otg_en(1);
			msleep(30);
		} else if (acc->current_accessory == ACCESSORY_LINEOUT)
			switch_set_state(&acc->ear_jack_switch, 1);
#ifdef CONFIG_CAMERON_HEALTH
		else if (acc->current_accessory == ACCESSORY_CAMERON) {
			/* To do, when the cameron health device is connected */

			/* force acc power off to ensure otg detection */
			if (acc->pdata->acc_power)
				acc->pdata->acc_power(0, false);
			msleep(20);

			if (acc->pdata->cameron_health_en)
				acc->pdata->cameron_health_en(1);
			msleep(30);
		}
#endif
		kobject_uevent_env(&acc->acc_dev->kobj, KOBJ_CHANGE, envp);
		ACC_CONDEV_DBG("%s : %s", env_ptr, stat_ptr);
	} else {
		if (acc->current_accessory == ACCESSORY_OTG) {
			env_ptr = "ACCESSORY=OTG";
		} else if (acc->current_accessory == ACCESSORY_LINEOUT) {
			env_ptr = "ACCESSORY=lineout";
			switch_set_state(&acc->ear_jack_switch,
				UEVENT_DOCK_NONE);
		} else if (acc->current_accessory == ACCESSORY_CARMOUNT) {
			env_ptr = "ACCESSORY=carmount";
#ifdef CONFIG_CAMERON_HEALTH
		} else if (acc->current_accessory == ACCESSORY_CAMERON) {
			env_ptr = "ACCESSORY=cameron";
#endif
		} else {
			env_ptr = "ACCESSORY=unknown";
		}

		stat_ptr = "STATE=offline";
		envp[0] = env_ptr;
		envp[1] = stat_ptr;
		envp[2] = NULL;
		kobject_uevent_env(&acc->acc_dev->kobj, KOBJ_CHANGE, envp);
		if ((acc->current_accessory == ACCESSORY_OTG) &&
			acc->pdata->otg_en)
			acc->pdata->otg_en(0);
#ifdef CONFIG_CAMERON_HEALTH
		else if (acc->current_accessory == ACCESSORY_CAMERON)
			acc->pdata->cameron_health_en(0);
#endif
		acc->current_accessory = ACCESSORY_NONE;
		ACC_CONDEV_DBG("%s : %s", env_ptr, stat_ptr);
	}
}

static void acc_dock_check(struct acc_con_info *acc, bool connected)
{
	char *env_ptr;
	char *stat_ptr;
	char *envp[3];

	if (acc->current_dock == DOCK_KEYBOARD)
		env_ptr = "DOCK=keyboard";
	else if (acc->current_dock == DOCK_DESK)
		env_ptr = "DOCK=desk";
	else
		env_ptr = "DOCK=unknown";

	if (!connected) {
		stat_ptr = "STATE=offline";
		acc->current_dock = DOCK_NONE;
	} else {
		stat_ptr = "STATE=online";
	}

	envp[0] = env_ptr;
	envp[1] = stat_ptr;
	envp[2] = NULL;
	kobject_uevent_env(&acc->acc_dev->kobj, KOBJ_CHANGE, envp);
	ACC_CONDEV_DBG("%s : %s", env_ptr, stat_ptr);
}

static void check_acc_dock(struct acc_con_info *acc)
{
	if (gpio_get_value(acc->pdata->accessory_irq_gpio)) {
		if (acc->current_dock == DOCK_NONE)
			return;
		ACC_CONDEV_DBG("docking station detached!!!");
		switch_set_state(&acc->dock_switch, UEVENT_DOCK_NONE);
#ifdef CONFIG_SEC_KEYBOARD_DOCK
		acc->pdata->check_keyboard(false);
#endif
#if defined(CONFIG_VIDEO_MHL_TAB_V2)
		/*call MHL deinit */
		mhl_onoff_ex(false);
#endif
		acc_dock_check(acc, false);
	} else {
		ACC_CONDEV_DBG("docking station attached!!!");

		switch_set_state(&acc->dock_switch, UEVENT_DOCK_CONNECTED);
		msleep(100);
		wake_lock(&acc->wake_lock);

#ifdef CONFIG_SEC_KEYBOARD_DOCK
		if (acc->pdata->check_keyboard(true)) {
			acc->current_dock = DOCK_KEYBOARD;
			ACC_CONDEV_DBG("[30PIN] keyboard dock "
				"station attached!!!");
			switch_set_state(&acc->dock_switch,
				UEVENT_DOCK_KEYBOARD);
		} else
#endif /* CONFIG_SEC_KEYBOARD_DOCK */
		{
			switch_set_state(&acc->dock_switch, UEVENT_DOCK_DESK);
			acc->current_dock = DOCK_DESK;
		}
#if defined(CONFIG_VIDEO_MHL_TAB_V2)
		mhl_onoff_ex(true);
#endif

		acc_dock_check(acc, true);

		wake_unlock(&acc->wake_lock);

	}
}

static irqreturn_t acc_con_interrupt(int irq, void *ptr)
{
	struct acc_con_info *acc = ptr;

	ACC_CONDEV_DBG("");

	check_acc_dock(acc);

	return IRQ_HANDLED;
}

#define DET_CHECK_TIME_MS 100
#define DET_SLEEP_TIME_MS 10
#define DETECTION_DELAY_MS	200

static irqreturn_t acc_ID_interrupt(int irq, void *dev_id)
{
	struct acc_con_info *acc = (struct acc_con_info *)dev_id;
	int acc_ID_val = 0, pre_acc_ID_val = 0;
	int time_left_ms = DET_CHECK_TIME_MS;

	ACC_CONDEV_DBG("");

	while (time_left_ms > 0) {
		acc_ID_val = acc->pdata->get_dock_state();

		if (acc_ID_val == pre_acc_ID_val)
			time_left_ms -= DET_SLEEP_TIME_MS;
		else
			time_left_ms = DET_CHECK_TIME_MS;

		pre_acc_ID_val = acc_ID_val;
		msleep(DET_SLEEP_TIME_MS);
	}

	ACC_CONDEV_DBG("IRQ_DOCK_GPIO is %d", acc_ID_val);
	if (acc_ID_val == 0) {
		ACC_CONDEV_DBG("Accessory detached");
		acc_notified(acc, false);
	} else {
		wake_lock(&acc->wake_lock);
		schedule_delayed_work(&acc->acc_id_dwork,
			msecs_to_jiffies(DETECTION_DELAY_MS));
	}
	return IRQ_HANDLED;
}

static int acc_con_interrupt_init(struct acc_con_info *acc)
{
	int ret = 0;
	ACC_CONDEV_DBG("");

	ret = gpio_request(acc->pdata->accessory_irq_gpio, "accessory");
	if (ret) {
		ACC_CONDEV_DBG("gpio_request(accessory_irq)return: %d\n", ret);
		return ret;
	}
	gpio_direction_input(acc->pdata->accessory_irq_gpio);
	acc->accessory_irq = gpio_to_irq(acc->pdata->accessory_irq_gpio);
	ret = request_threaded_irq(acc->accessory_irq, NULL, acc_con_interrupt,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
			"accessory_detect", acc);
	if (ret)
		ACC_CONDEV_DBG("request_irq(accessory_irq) return : %d\n", ret);

	ret = enable_irq_wake(acc->accessory_irq);
	if (ret)
		ACC_CONDEV_DBG("enable_irq_wake(accessory_irq) "
			"return : %d\n", ret);
	return ret;
}

static int acc_ID_interrupt_init(struct acc_con_info *acc)
{
	int ret = 0;
	ACC_CONDEV_DBG("");

	ret = gpio_request(acc->pdata->dock_irq_gpio, "dock");
	if (ret) {
		ACC_CONDEV_DBG("gpio_request(dock_irq)return: %d\n", ret);
		return ret;
	}
	gpio_direction_input(acc->pdata->dock_irq_gpio);
	acc->dock_irq = gpio_to_irq(acc->pdata->dock_irq_gpio);
	ret = request_threaded_irq(acc->dock_irq, NULL, acc_ID_interrupt,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
			"dock_detect", acc);
	if (ret)
		ACC_CONDEV_DBG("request_irq(dock_irq) return : %d\n", ret);

	ret = enable_irq_wake(acc->dock_irq);
	if (ret)
		ACC_CONDEV_DBG("enable_irq_wake(dock_irq) return : %d\n", ret);
	return ret;
}

static void acc_delay_work(struct work_struct *work)
{
	struct acc_con_info *acc = container_of(work,
		struct acc_con_info, acc_dwork.work);
	int retval;

	retval = acc_con_interrupt_init(acc);
	if (retval) {
		ACC_CONDEV_DBG(" failed to initialize "
			"the accessory detecty irq");
		goto err_irq_dock;
	}

	retval = acc_ID_interrupt_init(acc);
	if (retval) {
		ACC_CONDEV_DBG(" failed to initialize the accessory ID irq");
		goto err_irq_acc;
	}

	if (!gpio_get_value_cansleep(acc->pdata->accessory_irq_gpio))
		check_acc_dock(acc);

	if (acc->pdata->get_dock_state()) {
		wake_lock(&acc->wake_lock);
		schedule_delayed_work(&acc->acc_id_dwork,
			msecs_to_jiffies(DETECTION_DELAY_MS));
	}

	return ;

err_irq_acc:
	free_irq(acc->accessory_irq, acc);
err_irq_dock:
	switch_dev_unregister(&acc->ear_jack_switch);
	return ;
}

static void acc_id_delay_work(struct work_struct *work)
{
	struct acc_con_info *acc = container_of(work,
		struct acc_con_info, acc_id_dwork.work);
	int  adc_val = 0;
	if (!acc->pdata->get_dock_state()) {
		ACC_CONDEV_DBG("ACCESSORY detached\n");
		wake_unlock(&acc->wake_lock);
		return;
	} else {
		ACC_CONDEV_DBG("Accessory attached");
		adc_val = connector_detect_change();
		ACC_CONDEV_DBG("adc_val : %d", adc_val);
		acc_notified(acc, adc_val);
	}
	wake_unlock(&acc->wake_lock);
}

static int acc_con_probe(struct platform_device *pdev)
{
	struct acc_con_info *acc;
	struct acc_con_platform_data *pdata = pdev->dev.platform_data;
	int	retval;

	ACC_CONDEV_DBG("");

	if (pdata == NULL) {
		pr_err("%s: no pdata\n", __func__);
		return -ENODEV;
	}

	acc = kzalloc(sizeof(*acc), GFP_KERNEL);
	if (!acc)
		return -ENOMEM;

	acc->pdata = pdata;
	acc->current_dock = DOCK_NONE;
	acc->current_accessory = ACCESSORY_NONE;

	mutex_init(&acc->lock);

	retval = dev_set_drvdata(&pdev->dev, acc);
	if (retval < 0)
		goto err_sw_dock;

	acc->acc_dev = &pdev->dev;

	acc->dock_switch.name = "dock";
	retval = switch_dev_register(&acc->dock_switch);
	if (retval < 0)
		goto err_sw_dock;

	acc->ear_jack_switch.name = "usb_audio";
	retval = switch_dev_register(&acc->ear_jack_switch);
	if (retval < 0)
		goto err_sw_jack;

	wake_lock_init(&acc->wake_lock, WAKE_LOCK_SUSPEND, "30pin_con");
	INIT_DELAYED_WORK(&acc->acc_dwork, acc_delay_work);
	schedule_delayed_work(&acc->acc_dwork, msecs_to_jiffies(24000));
	INIT_DELAYED_WORK(&acc->acc_id_dwork, acc_id_delay_work);

	return 0;

err_sw_jack:
	switch_dev_unregister(&acc->dock_switch);
err_sw_dock:
	kfree(acc);

	return retval;

}

static int acc_con_remove(struct platform_device *pdev)
{
	struct acc_con_info *acc = platform_get_drvdata(pdev);

	ACC_CONDEV_DBG("");

	disable_irq_wake(acc->accessory_irq);
	disable_irq_wake(acc->dock_irq);
	free_irq(acc->accessory_irq, acc);
	free_irq(acc->dock_irq, acc);
	switch_dev_unregister(&acc->dock_switch);
	switch_dev_unregister(&acc->ear_jack_switch);
	kfree(acc);

	return 0;
}

static int acc_con_suspend(struct platform_device *pdev, pm_message_t state)
{
	ACC_CONDEV_DBG("");
	return 0;
}

static int acc_con_resume(struct platform_device *pdev)
{
	ACC_CONDEV_DBG("");
	return 0;
}

static struct platform_driver acc_con_driver = {
	.probe		= acc_con_probe,
	.remove		= acc_con_remove,
	.suspend		= acc_con_suspend,
	.resume		= acc_con_resume,
	.driver		= {
		.name		= "acc_con",
		.owner		= THIS_MODULE,
	},
};

static int __init acc_con_init(void)
{
	ACC_CONDEV_DBG("");
	return platform_driver_register(&acc_con_driver);
}

static void __exit acc_con_exit(void)
{
	platform_driver_unregister(&acc_con_driver);
}

late_initcall(acc_con_init);
module_exit(acc_con_exit);

MODULE_AUTHOR("Kyungrok Min <gyoungrok.min@samsung.com>");
MODULE_DESCRIPTION("acc connector driver");
MODULE_LICENSE("GPL");
