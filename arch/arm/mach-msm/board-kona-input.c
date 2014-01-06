/*
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <asm/mach-types.h>
#include <linux/regulator/consumer.h>
#include <mach/msm8960-gpio.h>
#include <linux/delay.h>
#include "board-8960.h"
#include "devices-msm8x60.h"
#ifdef CONFIG_WACOM_W9001
#include <linux/wacom_i2c.h>
#endif

#ifdef CONFIG_WACOM_W9001
static void wacom_request_gpio(void)
{
	int ret;
	pr_info("[WACOM] request gpio\n");

	gpio_tlmm_config(GPIO_CFG(GPIO_PEN_IRQ, 0,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);

	gpio_tlmm_config(GPIO_CFG(8,
				0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(9,
				0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_PEN_LDO_EN, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_PEN_PDCT, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_PEN_DETECT, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_PEN_FWE1, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 0);


	ret = gpio_request(GPIO_PEN_LDO_EN,
				"GPIO_PEN_LDO_EN");
	if (ret != 0) {
		pr_err("Wacom ldo enble request failed, ret=%d", ret);
		return;
	}

	ret = gpio_request(GPIO_PEN_IRQ,
		"GPIO_PEN_IRQ");
	if (ret != 0) {
		pr_err("Wacom pen irq request failed, ret=%d", ret);
		return;
	}

	ret = gpio_request(GPIO_PEN_PDCT,
						"GPIO_PEN_PDCT");
	if (ret != 0) {
		pr_err("Wacom pen pdct request failed, ret=%d", ret);
		return;
	}

	ret = gpio_request(GPIO_PEN_DETECT,
						"GPIO_PEN_DETECT");
	if (ret != 0) {
		pr_err("Wacom pen detect request failed, ret=%d", ret);
		return;
	}

	ret = gpio_request(GPIO_PEN_FWE1,
						"GPIO_PEN_FWE1");

	if (ret != 0) {
		pr_err("Wacom pen fwe1 request failed, ret=%d", ret);
		return;
	}
}

static void wacom_power_on(bool on)
{
	pr_info("[WACOM] power %s\n", on ? "on" : "off");

	pr_info("[WACOM] power on, before power %d\n",
			gpio_get_value(GPIO_PEN_LDO_EN));

	gpio_set_value(GPIO_PEN_LDO_EN, on);

	pr_info("[WACOM] power on, after power %d\n",
			gpio_get_value(GPIO_PEN_LDO_EN));
/*	
	if (on != gpio_get_value(GPIO_PEN_LDO_EN)) {
		gpio_set_value(GPIO_PEN_LDO_EN, on);
		pr_info("[WACOM] power on, power %d\n",
			gpio_get_value(GPIO_PEN_LDO_EN));
	}
*/
}


static struct wacom_platform_data wacom_pdata = {
	.x_invert = false,
	.y_invert = false,

	.xy_switch = false,

	.boot_on = true,
	.boot_addr = 0x09,
	.binary_fw_path = "wacom/n5100.fw",
	.file_fw_path = "/sdcard/firmware/wacom_firm.bin",
	.fw_version = 0x501,
	.fw_checksum = {0x1F, 0x1E, 0xF2, 0xAD, 0x5D},
	.power = wacom_power_on,
};

static struct i2c_board_info wacom_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO(WACNAME, 0x56),
		.platform_data = &wacom_pdata,
		.irq = MSM_GPIO_TO_INT(GPIO_PEN_IRQ),
	},
};

static struct i2c_gpio_platform_data  wacom_w9001_i2c_gpio_data = {
	.sda_pin		= 8,
	.scl_pin		= 9,
	.udelay			= 2,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.scl_is_output_only	= 0,
};

static struct platform_device wacom_i2c_gpio_device = {
	.name			= "i2c-gpio",
	.id			= MSM_8960_GSBI11_QUP_I2C_BUS_ID,
	.dev.platform_data	= &wacom_w9001_i2c_gpio_data,
};

static struct platform_device *input_wacom_devices[] __initdata = {
	&wacom_i2c_gpio_device,
};

void __init input_wacom_init(void)
{
	pr_info("[WACOM] init........\n");
	wacom_request_gpio();
	wacom_power_on(1);

	wacom_pdata.gpio_pendct = GPIO_PEN_PDCT;
	wacom_pdata.gpio_pen_insert = GPIO_PEN_DETECT;
	wacom_pdata.gpio_pen_fwe1 = GPIO_PEN_FWE1;

	i2c_register_board_info(MSM_8960_GSBI11_QUP_I2C_BUS_ID,
				wacom_boardinfo,
				ARRAY_SIZE(wacom_boardinfo));

	platform_add_devices(input_wacom_devices,
				ARRAY_SIZE(input_wacom_devices));
}
#endif
