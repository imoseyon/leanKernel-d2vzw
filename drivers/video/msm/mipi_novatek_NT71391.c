/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
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
 */

#include <linux/lcd.h>
#include <linux/leds.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_novatek_NT71391.h"
#include "mdp4.h"
#include "mdp4_video_enhance.h"

#include <linux/delay.h>
#include <linux/pwm.h>
#ifdef CONFIG_SAMSUNG_CMC624
#include "samsung_cmc624.h"
#endif
#ifdef CONFIG_BACKLIGHT_LP8556
#include "backlight_LP8556.h"
#endif

static struct delayed_work  det_work;

static struct mipi_novatek_driver_data msd;
#define PWM_FREQ_HZ	(2500)	/* 33 KHZ */
#define PWM_LEVEL 400
#define PWM_PERIOD_USEC (USEC_PER_SEC / PWM_FREQ_HZ)
#define PWM_DUTY_LEVEL (PWM_PERIOD_USEC / PWM_LEVEL)

#define PWM_DUTY_MAX   PWM_DUTY_LEVEL

#define DUTY_DIM 3
#define DUTY_MIN 4
#define DUTY_25  15
#define DUTY_DEFAULT 168
#define DUTY_MAX 278



/* Backlight levels */
#define BRIGHTNESS_OFF   0
#define BRIGHTNESS_DIM   20
#define BRIGHTNESS_MIN   30
#define BRIGHTNESS_25   86
#define BRIGHTNESS_DEFAULT  140
#define BRIGHTNESS_MAX   255

#define CMD_DELAY 100
#define DSI_MAX_LANES 4
#define KHZ 1000
#define MHZ (1000*1000)


static int led_pwm;
static struct pwm_device *bl_pwm;




static int bl_level;



static int scale_pwm_dutycycle(int level)
{
	int scaled_level = 0;
	if (level == BRIGHTNESS_OFF)
		scaled_level = BRIGHTNESS_OFF;
	else if (level <= BRIGHTNESS_DIM)
		scaled_level = PWM_DUTY_MAX*DUTY_DIM;
	else if (level <= BRIGHTNESS_MIN)
		scaled_level = (level - BRIGHTNESS_DIM) *
			(PWM_DUTY_MAX * DUTY_MIN - PWM_DUTY_MAX * DUTY_DIM) /
			(BRIGHTNESS_MIN - BRIGHTNESS_DIM) +
			PWM_DUTY_MAX * DUTY_DIM;
	else if (level <= BRIGHTNESS_25)
		scaled_level = (level - BRIGHTNESS_MIN) *
			(PWM_DUTY_MAX * DUTY_25 - PWM_DUTY_MAX * DUTY_MIN) /
			(BRIGHTNESS_25 - BRIGHTNESS_MIN) +
			PWM_DUTY_MAX * DUTY_MIN;
	else if (level <= BRIGHTNESS_DEFAULT)
		scaled_level = (level - BRIGHTNESS_25) *
			(PWM_DUTY_MAX * DUTY_DEFAULT - PWM_DUTY_MAX * DUTY_25)
			/ (BRIGHTNESS_DEFAULT - BRIGHTNESS_25) +
			PWM_DUTY_MAX * DUTY_25;
	else if (level <= BRIGHTNESS_MAX)
		scaled_level = (level - BRIGHTNESS_DEFAULT) *
			(PWM_DUTY_MAX * DUTY_MAX - PWM_DUTY_MAX * DUTY_DEFAULT)
			/ (BRIGHTNESS_MAX - BRIGHTNESS_DEFAULT) +
			PWM_DUTY_MAX * DUTY_DEFAULT;
	 pr_debug("%s: level: %d, scaled_level: %d, proc:%s, pid: %d, tgid:%d\n",
		__func__, level, scaled_level, current->comm,
		current->pid, current->tgid);
	return scaled_level;
}

	/* Set backlight level via PWM */
static int mipi_novatek_set_backlight_level(struct pwm_device *pwm, int level)
{
	int ret = 0;

	pr_info("%s: level=%d.\n", __func__, level);

	if ((pwm == NULL) || (level > BRIGHTNESS_MAX) || (level < 0)) {
		pr_err("%s.pwm=NULL.\n", __func__);
		return -EINVAL;
	}

	level = scale_pwm_dutycycle(level);

	ret = pwm_config(pwm, PWM_DUTY_LEVEL * level, PWM_PERIOD_USEC);
	if (ret) {
		pr_err("%s: pwm_config() failed err=%d.\n", __func__, ret);
		return ret;
	}

	ret = pwm_enable(pwm);
	if (ret) {
		pr_err("%s: pwm_enable() failed err=%d\n",
		       __func__, ret);
		return ret;
	}

	return 0;
	}

static int mipi_novatek_disp_send_cmd(struct msm_fb_data_type *mfd,
				       enum mipi_novatek_cmd_list cmd,
				       unsigned char lock)
{
	struct dsi_cmd_desc *cmd_desc;
	int cmd_size = 0;

	if (lock)
		mutex_lock(&mfd->dma->ov_mutex);
		pr_info("%s cmd = 0x%x\n", __func__, cmd);

	switch (cmd) {
	case PANEL_ON:
		cmd_desc = msd.mpd->on.cmd;
		cmd_size = msd.mpd->on.size;
		break;
	case PANEL_OFF:
		cmd_desc = msd.mpd->off.cmd;
		cmd_size = msd.mpd->off.size;
		break;
	default:
		goto unknown_command;
		;
	}

	if (!cmd_size)
		goto unknown_command;

	if (lock) {
		/* mdp4_dsi_cmd_busy_wait: will turn on dsi clock also */
		/*mdp4_dsi_cmd_dma_busy_wait(mfd);
		mdp4_dsi_blt_dmap_busy_wait(mfd);*/

		mipi_dsi_mdp_busy_wait();

		mipi_dsi_cmds_tx(&msd.novatek_tx_buf, cmd_desc, cmd_size);

		mutex_unlock(&mfd->dma->ov_mutex);
	} else {
		mipi_dsi_cmds_tx(&msd.novatek_tx_buf, cmd_desc, cmd_size);
	}

	return 0;

unknown_command:
	if (lock)
		mutex_unlock(&mfd->dma->ov_mutex);

	return 0;
}


static void blenable_work_func(struct work_struct *work)
{
	int ret = 0;
	/* Set backlight via PWM */
#ifdef CONFIG_SAMSUNG_CMC624
			if (samsung_has_cmc624())
				cmc624_set_pwm_backlight(bl_level);
			else if (bl_pwm) {
			ret = mipi_novatek_set_backlight_level(bl_pwm, bl_level);
				if (ret)
				pr_err("%s.mipi_novatek_set_backlight_level.ret=%d",
					   __func__, ret);
			}
#else
			if (bl_pwm) {
			ret = mipi_novatek_set_backlight_level(bl_pwm, bl_level);
				if (ret)
				pr_err("%s.mipi_novatek_set_backlight_level.ret=%d",
					   __func__, ret);
			}
#endif

}
/**
 * LCD ON.
 *
 * Set LCD On via MIPI interface .
 * Set Backlight on.
*/
static int mipi_novatek_disp_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	int ret = 0;

	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	mipi = &mfd->panel_info.mipi;
#ifdef CONFIG_BACKLIGHT_LP8556

	ret = bl_I2cWrite8(0x01, 0x80);
	if (ret != 0)
		pr_info("[BL:ERROR]:bl_I2cWrite16 failed:01: %d\n",ret);
	ret = bl_I2cWrite8(0xA3, 0x5E);
	if (ret != 0)
		pr_info("[BL:ERROR]:bl_I2cWrite16 failed:A3: %d\n",ret);
	ret = bl_I2cWrite8(0xA5, 0x24);
	if (ret != 0)
		pr_info("[BL:ERROR]:bl_I2cWrite16 failed:A5: %d\n",ret);

#endif

	if (mipi->mode == DSI_VIDEO_MODE)
		mipi_novatek_disp_send_cmd(mfd, PANEL_ON, false);
	if (bl_level == 0) {
			bl_level = BRIGHTNESS_DEFAULT ; /* Default ON value */
			INIT_DELAYED_WORK(&det_work, blenable_work_func);
			schedule_delayed_work(&det_work, msecs_to_jiffies(250));
		}
	/* Set backlight via PWM */
#ifdef CONFIG_SAMSUNG_CMC624
	if (samsung_has_cmc624())
		cmc624_set_pwm_backlight(bl_level);
	else if (bl_pwm) {
		ret = mipi_novatek_set_backlight_level(bl_pwm, bl_level);
		if (ret)
			pr_err("%s.mipi_novatek_set_backlight_level.ret=%d",
			       __func__, ret);
	}
#else
	if (bl_pwm) {
		ret = mipi_novatek_set_backlight_level(bl_pwm, bl_level);
		if (ret)
			pr_err("%s.mipi_novatek_set_backlight_level.ret=%d",
			       __func__, ret);
		}
#endif
	pr_info("%s:Display on completed\n", __func__);
	return 0;
}
static int mipi_novatek_disp_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;
	msleep(20);
#ifdef CONFIG_SAMSUNG_CMC624
		if (samsung_has_cmc624())
			cmc624_set_pwm_backlight(0);
		else
			mipi_novatek_set_backlight_level(bl_pwm, 0);
#else
			mipi_novatek_set_backlight_level(bl_pwm, 0);
#endif
	pr_info("%s:Display off completed\n", __func__);
	return 0;
}

static void mipi_novatek_disp_set_backlight(struct msm_fb_data_type *mfd)
{
	int level = mfd->bl_level;
	pr_info("%s Back light level:%d\n", __func__, mfd->bl_level);
#ifdef CONFIG_SAMSUNG_CMC624
	if (samsung_has_cmc624())
		cmc624_set_pwm_backlight(level);
	else
		mipi_novatek_set_backlight_level(bl_pwm, level);
#else
		mipi_novatek_set_backlight_level(bl_pwm, level);
#endif

	bl_level = level;

	return;
}
#if defined(CONFIG_LCD_CLASS_DEVICE)
static ssize_t mipi_novatek_lcdtype_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char temp[20];

	snprintf(temp, strnlen(msd.mpd->panel_name, 20) + 1,
						msd.mpd->panel_name);
	strlcat(buf, temp, 20);
	return strnlen(buf, 20);
}
static struct lcd_ops mipi_novatek_disp_props;

static DEVICE_ATTR(lcd_type, S_IRUGO, mipi_novatek_lcdtype_show, NULL);
#endif
static int __devinit mipi_novatek_disp_probe(struct platform_device *pdev)
{	
	struct platform_device *msm_fb_added_dev;
		
#if defined(CONFIG_LCD_CLASS_DEVICE)
	int ret;
	struct lcd_device *lcd_device;
#endif
	pr_info("%s probe called:",__func__);
	if (pdev->id == 0) {
		msd.mipi_novatek_disp_pdata = pdev->dev.platform_data;

#ifdef CONFIG_SAMSUNG_CMC624
	if (samsung_has_cmc624()) {
		printk(KERN_DEBUG "Is_There_cmc624 : CMC624 is there!!!!");
		samsung_cmc624_init();
	} else {
		printk(KERN_DEBUG "Is_There_cmc624 : CMC624 is not there!!!!");
	}
#endif
#ifdef CONFIG_BACKLIGHT_LP8556
	samsung_bl_init();
#endif

		return 0;
		}
	/*Channel ID =1 for GPIO LED_BACKLIGHT_PWMof PMIC*/
	led_pwm=1;
	msm_fb_added_dev = msm_fb_add_device(pdev);

#if defined(CONFIG_LCD_CLASS_DEVICE)
	msd.msm_pdev = msm_fb_added_dev;
#endif
#ifdef CONFIG_SAMSUNG_CMC624
	if (!samsung_has_cmc624()){
		bl_pwm = NULL;
		if (led_pwm >= 0) {
		bl_pwm = pwm_request(led_pwm, "lcd-backlight");
		if (bl_pwm == NULL || IS_ERR(bl_pwm)) {
			pr_err("%s pwm_request() failed.id=%d.bl_pwm=%d.\n",
			       __func__, led_pwm, (int) bl_pwm);
			bl_pwm = NULL;
			return -EIO;
		} else {
			pr_debug("%s.pwm_request() ok.pwm-id=%d.\n",
			       __func__, led_pwm);
		}
		} else {
		pr_info("%s. led_pwm is invalid.\n", __func__);
		}
		}
#else
	bl_pwm = NULL;
	if (led_pwm >= 0) {
		bl_pwm = pwm_request(led_pwm, "lcd-backlight");
		if (bl_pwm == NULL || IS_ERR(bl_pwm)) {
			pr_err("%s pwm_request() failed.id=%d.bl_pwm=%d.\n",
			       __func__, led_pwm, (int) bl_pwm);
			bl_pwm = NULL;
			return -EIO;
		} else {
			pr_debug("%s.pwm_request() ok.pwm-id=%d.\n",
			       __func__, led_pwm);
		}
		} else {
		pr_info("%s. led_pwm is invalid.\n", __func__);
		}
#endif


#if defined(CONFIG_LCD_CLASS_DEVICE)
	lcd_device = lcd_device_register("panel", &pdev->dev, NULL,
					&mipi_novatek_disp_props);

	if (IS_ERR(lcd_device)) {
		ret = PTR_ERR(lcd_device);
		printk(KERN_ERR "lcd : failed to register device\n");
		return ret;
	}

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_type.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_lcd_type.attr.name);
	}
#endif
#ifdef CONFIG_SAMSUNG_CMC624
	if (samsung_has_cmc624()) {
			ret = cmc624_sysfs_init();
			if (ret < 0)
				pr_debug("CMC624 sysfs initialize FAILED\n");
		}
	else
		init_mdnie_class();
#endif
	pr_debug("%s:Display probe completed\n", __func__);
	return 0;
}
/**
 * Device removal notification handler.
 *
 * @param pdev
 *
 * @return int
 */
static int __devexit mipi_novatek_disp_remove(struct platform_device *pdev)
{
	/* Note: There are no APIs to remove fb device and free DSI buf. */
	pr_debug("%s.\n", __func__);

	if (bl_pwm) {
		pwm_free(bl_pwm);
		bl_pwm = NULL;
	}

	return 0;
}
static struct platform_driver this_driver = {
	.probe  = mipi_novatek_disp_probe,
	.remove = __devexit_p(mipi_novatek_disp_remove),
	.driver = {
		.name   = "mipi_novatek_nt71391",
	},
};

static struct msm_fb_panel_data novatek_panel_data = {
	.on		= mipi_novatek_disp_on,
	.off		= mipi_novatek_disp_off,
	.set_backlight	= mipi_novatek_disp_set_backlight,
};

static int ch_used[3];

int mipi_novatek_disp_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd)
{
	struct platform_device *pdev = NULL;
	int ret = 0;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_novatek_nt71391",
					   (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	novatek_panel_data.panel_info = *pinfo;
	msd.mpd = mpd;
	if (!msd.mpd) {
		printk(KERN_ERR
		  "%s: get mipi_panel_data failed!\n", __func__);
		goto err_device_put;
	}
	mpd->msd = &msd;
	ret = platform_device_add_data(pdev, &novatek_panel_data,
		sizeof(novatek_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return ret;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int __init mipi_novatek_disp_init(void)
{
	mipi_dsi_buf_alloc(&msd.novatek_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&msd.novatek_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}
module_init(mipi_novatek_disp_init);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Novatek NT71391 LCD driver");
