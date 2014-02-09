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
 */

#define pr_fmt(fmt)     "%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/workqueue.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/mfd/pm8xxx/core.h>
#include <linux/mfd/pm8xxx/pwm.h>
#include <linux/leds-pm8xxx.h>
#include <linux/sched.h>

#define SSBI_REG_ADDR_LED_CTRL_BASE     0x131
#define SSBI_REG_ADDR_LED_CTRL(n)       (SSBI_REG_ADDR_LED_CTRL_BASE + (n))

#define PM8XXX_LED_OFFSET(id) ((id) - PM8XXX_ID_LED_0)

#define PM8XXX_LED_PWM_FLAGS    (PM_PWM_LUT_LOOP | PM_PWM_LUT_RAMP_UP |\
		PM_PWM_LUT_REVERSE | PM_PWM_LUT_PAUSE_HI_EN | \
		PM_PWM_LUT_PAUSE_LO_EN)

/**
 * struct pm8xxx_led_data - internal led data structure
 * @led_classdev - led class device
 * @id - led index
 * @work - workqueue for led
 * @lock - to protect the transactions
 * @reg - cached value of led register
 * @pwm_dev - pointer to PWM device if LED is driven using PWM
 * @pwm_channel - PWM channel ID
 * @pwm_period_us - PWM period in micro seconds
 * @pwm_duty_cycles - struct that describes PWM duty cycles info
 */
struct pm8xxx_led_data {
	struct led_classdev     cdev;
	int                     id;
	u8                      reg;
	struct device           *dev;
	struct pwm_device       *pwm_dev;
	int                     pwm_channel;
	u32                     pwm_period_us;
	struct pm8xxx_pwm_duty_cycles *pwm_duty_cycles;
};

struct leds_dev_data {
	struct pm8xxx_led_data *led;
	struct pm8xxx_led_platform_data *pdata ;
	atomic_t op_flag;
	struct mutex led_work_lock;
	struct work_struct pattern_work;

	/* color1 is optional; color2 is expected */
	u32 color1, color2; // X8 R8 G8 B8
	int time1, time2, timetrans;
	u8 pattern;
};

// 20-point sine curve.  All 3 channels must fit into the 64-point LUT.
static u8 breathe_duty_pcts[20] = {
	0, 2, 7, 15, 27, 41, 58, 76, 96, 117,
	138, 159, 179, 197, 214, 228, 240, 248, 253, 255,
};
static u8 lpm_mult = 100;

/* Generate the transition between color1 and color2
 */
static int generate_duty_table(int *target,
			       int time,
			       u8 color1,
			       u8 color2,
			       bool anim) {
	int i, c = 0;
	/* !color2: this led should stay off */
	if (!color2)
		return 0;
	/* Non-animated; either steady or plain blinking */
	if (!(anim && time)) {
		target[0] = DIV_ROUND_UP(color1 * lpm_mult, 255);
		target[1] = DIV_ROUND_UP(color2 * lpm_mult, 255);
		return 2;
	}
	/* Animate between two colors (color1 likely black) */
	for (i = 0; i < 20; i++) {
		if (unlikely(color1))
			c = color1 * (255 - breathe_duty_pcts[i]);
		target[i] = (color2 * breathe_duty_pcts[i] + c) *
			lpm_mult / (255 * 255);
		if (breathe_duty_pcts[i] == 255) {
			i++;
			break;
		}
	}
	return i;
}

/* The stock source has an absurd amount of code to manage this.  It's fed
 * values in all different scales, and doesn't make much sense.  It turns out
 * that it's just a binary value.
 */
static void led_set(struct pm8xxx_led_data *led, int brightness) {
	brightness = brightness ? 1 : 0;

	led->reg = (led->reg & ~0xf8) | (brightness << 3);
	pm8xxx_writeb(led->dev->parent,
		SSBI_REG_ADDR_LED_CTRL(PM8XXX_LED_OFFSET(led->id)),
		led->reg);
}

static void pm8xxx_led_set(struct led_classdev *led_cdev,
		enum led_brightness value) {
	struct pm8xxx_led_data *led =
		container_of(led_cdev, struct pm8xxx_led_data, cdev);
	if (value < LED_OFF || value > led_cdev->max_brightness)
		return;
	led_set(led, value);
}
static enum led_brightness pm8xxx_led_get(struct led_classdev *led_cdev) {
	return led_cdev->brightness ? LED_FULL : LED_OFF;
}

static void led_pwm_config(struct pm8xxx_led_data *led, int time1, int time2) {
	if (unlikely(!led->pwm_duty_cycles)) {
		pr_err("no duty cycles for led?\n");
		return;
	}

	led->pwm_dev = pwm_request(led->pwm_channel, led->cdev.name);

	if (unlikely(IS_ERR_OR_NULL(led->pwm_dev))) {
		pr_err("can't get led\n");
		led->pwm_dev = NULL;
		return;
	}

	if (unlikely(led->pwm_duty_cycles->start_idx +
	    led->pwm_duty_cycles->num_duty_pcts > PM_PWM_LUT_SIZE)) {
		pr_err("lut too large\n");
		pwm_free(led->pwm_dev);
		led->pwm_dev = NULL;
		return;
	}

	pm8xxx_pwm_lut_config(led->pwm_dev, led->pwm_period_us,
		led->pwm_duty_cycles->duty_pcts,
		led->pwm_duty_cycles->duty_ms,
		led->pwm_duty_cycles->start_idx,
		led->pwm_duty_cycles->num_duty_pcts,
		time1, time2, PM8XXX_LED_PWM_FLAGS);
}

static void led_pattern_work(struct work_struct *work) {
	struct leds_dev_data *info =
		container_of(work, struct leds_dev_data, pattern_work);
	struct pm8xxx_led_config *led_cfg;
	int i;
	u32 color1, color2;
	u8 chan1, chan2, count;
	struct pwm_device *pwms[4] = { };
	int pwm = 0;

	mutex_lock(&info->led_work_lock);

	/* Stop LEDs while reprogramming
	 * TODO: fade out the steady color?
	 */
	if (info->pdata->led_power_on)
		info->pdata->led_power_on(0);
	for (i = 0; i < info->pdata->led_core->num_leds; i++) {
		if (!info->led[i].pwm_dev)
			continue;

		pwm_free(info->led[i].pwm_dev);
		info->led[i].pwm_dev = NULL;
		led_set(&info->led[i], 0);
	}

	color1 = info->color1;
	color2 = info->color2;

	for (i = 2; i >= 0; i--) {
		led_cfg = &info->pdata->configs[i];

		chan1 = color1 & 0xff;
		chan2 = color2 & 0xff;
		color1 >>= 8;
		color2 >>= 8;

		count = generate_duty_table(
			led_cfg->pwm_duty_cycles->duty_pcts,
			info->timetrans,
			chan1, chan2,
			/*info->time1 ||*/ info->time2);

		if (!count)
			continue;

		led_cfg->pwm_duty_cycles->num_duty_pcts = count;

		if (count > 2)
			led_cfg->pwm_duty_cycles->duty_ms =
				DIV_ROUND_UP(info->timetrans, count);
		else
			led_cfg->pwm_duty_cycles->duty_ms = 1;

		led_pwm_config(&info->led[i], info->time1, info->time2);
		led_set(&info->led[i], led_cfg->max_current);
		/* Queue up all the pwms we'll be using... */
		if (info->led[i].pwm_dev)
			pwms[pwm++] = info->led[i].pwm_dev;
	}

	if (pwm) {
		if (info->pdata->led_power_on)
			info->pdata->led_power_on(1);
		/* ...and enable them in one shot. */
		pm8xxx_pwm_lut_enable_all(pwms);
	}

	mutex_unlock(&info->led_work_lock);
}

static ssize_t led_pattern_show(struct device *dev,
		struct device_attribute *attr, char *buf) {
	struct leds_dev_data *info = dev_get_drvdata(dev);
	return snprintf(buf, 4, "%u\n", info->pattern);
}

// The stock code takes more than 1000 lines of code to accomplish this.  WTF?
static ssize_t led_pattern_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
	struct leds_dev_data *info = dev_get_drvdata(dev);

	mutex_lock(&info->led_work_lock);

	info->color1 = 0;
	info->color2 = 0;
	info->time1 = 0;
	info->time2 = 0;
	info->timetrans = 0;
	info->pattern = buf[0] - '0';

	switch (buf[0]) {
	case '1':
		info->color2 = 0xff0000;
		break;
	case '2':
		info->color2 = 0xff0000;
		//info->time1 = 500;
		//info->time2 = 500;
		info->time1 = 400;
		info->time2 = 400;
		info->timetrans = 100;
		break;
	case '3':
		info->color2 = 0xff;
		//info->time1 = 5000;
		//info->time2 = 500;
		info->time1 = 4750;
		info->time2 = 250;
		info->timetrans = 250;
		break;
	case '4':
		info->color2 = 0xff0000;
		//info->time1 = 5000;
		//info->time2 = 500;
		info->time1 = 4750;
		info->time2 = 250;
		info->timetrans = 250;
		break;
	case '5':
		info->color2 = 0xff00;
		break;
	case '6':
		info->color1 = 0x14ca;
		info->color2 = 0x82ff;
		info->time1 = 200;
		info->time2 = 200;
		info->timetrans = 800;
		break;
	default:
		info->pattern = 0;
		break;
	}

	if (!work_busy(&info->pattern_work))
		schedule_work(&info->pattern_work);

	mutex_unlock(&info->led_work_lock);
	return size;
}

static DEVICE_ATTR(led_pattern, S_IRUGO | S_IWUSR | S_IWGRP,
			led_pattern_show, led_pattern_store);

static ssize_t led_lowpower_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 4, "%d\n", lpm_mult == 11);
}

static ssize_t led_lowpower_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct leds_dev_data *info = dev_get_drvdata(dev);

	mutex_lock(&info->led_work_lock);

	lpm_mult = buf[0] == '1' ? 11 : 100;

	if (!work_busy(&info->pattern_work))
		schedule_work(&info->pattern_work);

	mutex_unlock(&info->led_work_lock);

	return size;
}

static DEVICE_ATTR(led_lowpower, S_IRUGO | S_IWUSR | S_IWGRP,
			led_lowpower_show, led_lowpower_store);

static ssize_t led_blink_show(struct device *dev,
		struct device_attribute *attr, char *buf) {
	struct leds_dev_data *info = dev_get_drvdata(dev);
	return snprintf(buf, 10, "%06x\n", info->color2);
}

#define channel_attr(_c, _s) \
static ssize_t led_##_c##_store(struct device *dev, \
		struct device_attribute *attr, const char *buf, size_t size) { \
	struct leds_dev_data *info = dev_get_drvdata(dev); \
	u8 v; \
	if (buf[0] == '0' && buf[1] == 'x') \
		buf += 2; \
	if (sscanf(buf, "%hhx\n", &v) != 1) \
		return -EINVAL; \
	mutex_lock(&info->led_work_lock); \
	info->color2 = (info->color2 & ~(0xff << _s)) | v; \
	if (!work_busy(&info->pattern_work)) \
		schedule_work(&info->pattern_work); \
	mutex_unlock(&info->led_work_lock); \
	return size; \
} \
static ssize_t led_##_c##_show(struct device *dev, \
		struct device_attribute *attr, char *buf) { \
	struct leds_dev_data *info = dev_get_drvdata(dev); \
	return snprintf(buf, 4, "%hhx\n", \
		(info->color2 >> _s) & 0xff); \
} \
static DEVICE_ATTR(led_##_c, S_IRUGO | S_IWUSR | S_IWGRP, \
			led_##_c##_show, led_##_c##_store)

channel_attr(r, 16);
channel_attr(g, 8);
channel_attr(b, 0);

static ssize_t led_blink_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
	struct leds_dev_data *info = dev_get_drvdata(dev);
	u32 color = 0;
	unsigned int delayon, delayoff, animtime;

	printk(KERN_ALERT "[LED_blink_store] is \"%s\" (pid %i)\n",
		current->comm, current->pid);

	if (size < 10)
		return -EINVAL;
	if (buf[0] == '0' && buf[1] == 'x')
		buf += 2;
	if (sscanf(buf, "%x %u %u\n", &color, &delayon, &delayoff) < 3) {
		printk(KERN_DEBUG "led_blink: Invlid input\n");
		return -EINVAL;
	}

	/* TODO: adjustablize this? */
	if (delayon || delayoff) {
		animtime = 500;

		if (delayon && (animtime > delayon / 2))
			animtime = delayon / 2;
		if (delayoff && (animtime > delayoff / 2))
			animtime = delayoff / 2;

		if (animtime < 40) {
			animtime = 0;
		} else {
			if (delayon)
				delayon -= animtime;
			if (delayoff)
				delayoff -= animtime;
		}
	} else {
		animtime = 0;
	}

	mutex_lock(&info->led_work_lock);

	info->color1 = 0;
	info->color2 = color;
	info->time1 = delayoff;
	info->time2 = delayon;
	info->timetrans = animtime;
	info->pattern = 0;
	if (!work_busy(&info->pattern_work))
		schedule_work(&info->pattern_work);

	mutex_unlock(&info->led_work_lock);

	return size;
}

static DEVICE_ATTR(led_blink, S_IRUGO | S_IWUSR | S_IWGRP,
			led_blink_show, led_blink_store);

static int __devinit pm8xxx_led_probe(struct platform_device *pdev)
{
	const struct led_platform_data *pcore_data;
	struct led_info *curr_led;
	struct pm8xxx_led_config *led_cfg;
	struct pm8xxx_led_data *led, *led_dat;
	struct leds_dev_data *info;
	struct pm8xxx_led_platform_data *pdata ;
	struct device *sec_led;
	int rc = -1, i = 0;
	pdata = pdev->dev.platform_data;
	if (pdata == NULL) {
		dev_err(&pdev->dev, "platform data not supplied\n");
		return -EINVAL;
	}

	pcore_data = pdata->led_core;

	if (pcore_data->num_leds != pdata->num_configs) {
		dev_err(&pdev->dev, "#no. of led configs and #no. of led"
				"entries are not equal\n");
		return -EINVAL;
	}

	led = kcalloc(pcore_data->num_leds, sizeof(*led), GFP_KERNEL);
	if (led == NULL) {
		dev_err(&pdev->dev, "failed to alloc memory\n");
		return -ENOMEM;
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "fail to memory allocation.\n");
		rc = -ENOMEM;
		goto fail_mem_check;
	}

	info->pdata = pdata;
	info->led = led;

	for (i = 0; i < pcore_data->num_leds; i++) {
		curr_led = &pcore_data->leds[i];
		led_dat	 = &led[i];
		led_cfg	 = &pdata->configs[i];

		if (led_cfg->id < PM8XXX_ID_LED_0 ||
		    led_cfg->id > PM8XXX_ID_LED_2) {
			dev_err(&pdev->dev, "invalid LED ID (%d) specified\n",
						 led_dat->id);
			rc = -EINVAL;
			goto fail_id_check;
		}

		led_dat->id = led_cfg->id;
		led_dat->pwm_channel = led_cfg->pwm_channel;
		led_dat->pwm_period_us = led_cfg->pwm_period_us;
		led_dat->pwm_duty_cycles = led_cfg->pwm_duty_cycles;
		led_dat->cdev.name = curr_led->name;
		led_dat->cdev.default_trigger = curr_led->default_trigger;
		led_dat->cdev.brightness_set = pm8xxx_led_set;
		led_dat->cdev.brightness_get = pm8xxx_led_get;
		led_dat->cdev.brightness = 0;
		led_dat->cdev.max_brightness = LED_FULL;
		led_dat->cdev.flags = curr_led->flags;
		led_dat->reg = led_cfg->mode;
		led_dat->dev = &pdev->dev;

		led_set(led_dat, 0);
	}

	lpm_mult = 100;
	mutex_init(&info->led_work_lock);
	INIT_WORK(&info->pattern_work, led_pattern_work);

	platform_set_drvdata(pdev, info);
        sec_led = device_create(sec_class, NULL, 0, NULL, "led");
        dev_set_drvdata(sec_led, info);
        device_create_file(sec_led, &dev_attr_led_pattern);
        device_create_file(sec_led, &dev_attr_led_lowpower);
        device_create_file(sec_led, &dev_attr_led_r);
        device_create_file(sec_led, &dev_attr_led_g);
        device_create_file(sec_led, &dev_attr_led_b);
        device_create_file(sec_led, &dev_attr_led_blink);

	return 0;

fail_id_check:
	if (i > 0) {
		for (i = i - 1; i >= 0; i--) {
			led_classdev_unregister(&led[i].cdev);
			if (led[i].pwm_dev)
				pwm_free(led[i].pwm_dev);
		}
	}
	kfree(info);
fail_mem_check:
	kfree(led);
	return rc;
}

static int __devexit pm8xxx_led_remove(struct platform_device *pdev)
{
	int i;
	const struct led_platform_data *pdata =
				pdev->dev.platform_data;
	struct leds_dev_data *info = platform_get_drvdata(pdev);

	for (i = 0; i < pdata->num_leds; i++) {
		led_classdev_unregister(&info->led[i].cdev);
		if (info->led[i].pwm_dev)
			pwm_free(info->led[i].pwm_dev);
	}

	kfree(info->led);
	kfree(info);
	return 0;
}

static struct platform_driver pm8xxx_led_driver = {
	.probe	  = pm8xxx_led_probe,
	.remove	 = __devexit_p(pm8xxx_led_remove),
	.driver	 = {
		.name   = PM8XXX_LEDS_DEV_NAME,
		.owner  = THIS_MODULE,
	},
};

static int __init pm8xxx_led_init(void)
{
	return platform_driver_register(&pm8xxx_led_driver);
}
subsys_initcall(pm8xxx_led_init);

static void __exit pm8xxx_led_exit(void)
{
	platform_driver_unregister(&pm8xxx_led_driver);
}
module_exit(pm8xxx_led_exit);

MODULE_DESCRIPTION("PM8XXX LEDs driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:pm8xxx-led");

