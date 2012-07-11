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
#include <linux/keyreset.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/platform_data/mms_ts.h>
#include <asm/mach-types.h>
#include <linux/regulator/consumer.h>
#include <mach/msm8960-gpio.h>
#include "board-8960.h"

#define MSM_8960_GSBI3_QUP_I2C_BUS_ID 3

int touch_is_pressed;
EXPORT_SYMBOL(touch_is_pressed);

static int melfas_mux_fw_flash(bool to_gpios)
{
	if (to_gpios) {
		gpio_direction_output(GPIO_MXT_TS_IRQ, 0);
		gpio_tlmm_config(GPIO_CFG(GPIO_MXT_TS_IRQ, 0,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
		gpio_direction_output(GPIO_TOUCH_SCL, 0);
		gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_SCL, 0,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
		gpio_direction_output(GPIO_TOUCH_SDA, 0);
		gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_SDA, 0,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	} else {
		gpio_direction_output(GPIO_MXT_TS_IRQ, 1);
		gpio_tlmm_config(GPIO_CFG(GPIO_MXT_TS_IRQ, 0,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
		gpio_direction_output(GPIO_TOUCH_SCL, 1);
		gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_SCL, 1,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
		gpio_direction_output(GPIO_TOUCH_SDA, 1);
		gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_SDA, 1,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	}

	return 0;
}

void  melfas_vdd_on(bool onoff)
{
	int ret = 0;
	/* 3.3V */
	static struct regulator *reg_l17;

	/* 1.8V */
	static struct regulator *reg_lvs6;

	if (!reg_lvs6) {
		reg_lvs6 = regulator_get(NULL, "8921_lvs6");
		if (IS_ERR(reg_lvs6)) {
			pr_err("could not get 8921_lvs6, rc = %ld\n",
				PTR_ERR(reg_lvs6));
			return;
		}
	}

	if (onoff) {
		ret = regulator_enable(reg_lvs6);
		if (ret) {
			pr_err("enable lvs6 failed, rc=%d\n", ret);
			return;
		}
		pr_info("tsp 1.8V on is finished.\n");
	} else {
		if (regulator_is_enabled(reg_lvs6))
			ret = regulator_disable(reg_lvs6);
		if (ret) {
			pr_err("enable lvs6 failed, rc=%d\n", ret);
			return;
		}
		pr_info("tsp 1.8V off is finished.\n");
	}

	if (!reg_l17) {
		reg_l17 = regulator_get(NULL, "8921_l17");
		if (IS_ERR(reg_l17)) {
			pr_err("could not get 8921_l17, rc = %ld\n",
				PTR_ERR(reg_l17));
			return;
		}
		ret = regulator_set_voltage(reg_l17, 3300000, 3300000);
		if (ret) {
			pr_err("%s: unable to set ldo17 voltage to 3.3V\n",
				__func__);
			return;
		}
	}

	if (onoff) {
		ret = regulator_enable(reg_l17);
		if (ret) {
			pr_err("enable l17 failed, rc=%d\n", ret);
			return;
		}
		pr_info("tsp 3.3V on is finished.\n");
	} else {
		if (regulator_is_enabled(reg_l17))
			ret = regulator_disable(reg_l17);

		if (ret) {
			pr_err("disable l17 failed, rc=%d\n", ret);
			return;
		}
		pr_info("tsp 3.3V off is finished.\n");
	}
}

int is_melfas_vdd_on(void)
{
	int ret;
	/* 3.3V */
	static struct regulator *reg_l17;
	if (!reg_l17) {
		reg_l17 = regulator_get(NULL, "8921_l17");
		if (IS_ERR(reg_l17)) {
			ret = PTR_ERR(reg_l17);
			pr_err("could not get 8921_l17, rc = %d\n",
				ret);
			return ret;
		}
		ret = regulator_set_voltage(reg_l17, 3300000, 3300000);
		if (ret) {
			pr_err("%s: unable to set ldo17 voltage to 3.3V\n",
				__func__);
			return ret;
		}
	}
	if (regulator_is_enabled(reg_l17))
		return 1;
	else
		return 0;
}

static void melfas_register_callback(struct tsp_callbacks *cb)
{
	charger_callbacks = cb;
	pr_err("[TSP] melfas_register_callback\n");
}

#define USE_TOUCHKEY 0

static struct mms_ts_platform_data mms_ts_pdata = {
	.max_x		= 480,
	.max_y		= 800,
	.use_touchkey = USE_TOUCHKEY,
	.mux_fw_flash	= melfas_mux_fw_flash,
	.vdd_on		= melfas_vdd_on,
	.is_vdd_on	= is_melfas_vdd_on,
	.gpio_resetb	= GPIO_MXT_TS_IRQ,
	.gpio_scl	= GPIO_TOUCH_SCL,
	.gpio_sda	= GPIO_TOUCH_SDA,
	.check_module_type = true,
	.register_cb = melfas_register_callback,
};

static struct i2c_board_info __initdata mms_i2c3_boardinfo_final[] = {
	{
		I2C_BOARD_INFO("mms_ts", 0x48),
		.flags = I2C_CLIENT_WAKE,
		.platform_data = &mms_ts_pdata,
		.irq = MSM_GPIO_TO_INT(GPIO_MXT_TS_IRQ),
	},
};

void __init mms_tsp_input_init(void)
{
	int ret;

	ret = gpio_request(GPIO_MXT_TS_IRQ, "tsp_int");
	if (ret != 0) {
		pr_err("tsp int request failed, ret=%d", ret);
		goto err_int_gpio_request;
	}
	gpio_tlmm_config(GPIO_CFG(GPIO_MXT_TS_IRQ, 0,
			GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	ret = gpio_request(GPIO_TOUCH_SCL, "tsp_scl");
	if (ret != 0) {
		pr_err("tsp scl request failed, ret=%d", ret);
		goto err_scl_gpio_request;
	}
	ret = gpio_request(GPIO_TOUCH_SDA, "tsp_sda");
	if (ret != 0) {
		pr_err("tsp sda request failed, ret=%d", ret);
		goto err_sda_gpio_request;
	}
	melfas_vdd_on(1);

	i2c_register_board_info(MSM_8960_GSBI3_QUP_I2C_BUS_ID,
				mms_i2c3_boardinfo_final,
				ARRAY_SIZE(mms_i2c3_boardinfo_final));

err_sda_gpio_request:
	gpio_free(GPIO_TOUCH_SCL);
err_scl_gpio_request:
	gpio_free(GPIO_MXT_TS_IRQ);
err_int_gpio_request:
	;
}
