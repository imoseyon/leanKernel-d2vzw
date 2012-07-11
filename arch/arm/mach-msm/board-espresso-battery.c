/*
 * Copyright (C) 2012 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/gpio.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/msm8960-gpio.h>

#include "devices-msm8x60.h"
#include "board-8960.h"

#ifdef CONFIG_ADC_STMPE811
#include <linux/stmpe811.h>
#endif

#if defined(CONFIG_BATTERY_SAMSUNG)
#include <linux/battery/sec_battery.h>
#include <linux/battery/sec_fuelgauge.h>
#include <linux/battery/sec_charger.h>

#define SEC_BATTERY_PMIC_NAME ""

static int msm_otg_pmic_gpio_config(int gpio, int direction,
			int pullup, char *gpio_id, int req_sel)
{
	struct pm_gpio param;
	int rc = 0;
	int out_strength = 0;

	if (direction == PM_GPIO_DIR_IN)
		out_strength = PM_GPIO_STRENGTH_NO;
	else
		out_strength = PM_GPIO_STRENGTH_HIGH;

	param.direction = direction;
	param.output_buffer = PM_GPIO_OUT_BUF_CMOS;
	param.output_value = 0;
	param.pull = pullup;
	param.vin_sel = PM_GPIO_VIN_S4;
	param.out_strength = out_strength;
	param.function = PM_GPIO_FUNC_NORMAL;
	param.inv_int_pol = 0;
	param.disable_pin = 0;

	rc = pm8xxx_gpio_config(
		PM8921_GPIO_PM_TO_SYS(gpio), &param);
	if (rc < 0) {
		pr_err("failed to configure vbus_in gpio\n");
		return rc;
	}

	if (req_sel) {
		rc = gpio_request(PM8921_GPIO_PM_TO_SYS(gpio),
						gpio_id);
		if (rc < 0)
			pr_err("failed to request vbus_in gpio\n");
	}

	return rc;
}

static bool sec_bat_adc_none_init(struct platform_device *pdev) {return true; }
static bool sec_bat_adc_none_exit(void) {return true; }
static int sec_bat_adc_none_read(unsigned int channel) {return 0; }

static bool sec_bat_adc_ap_init(struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ap_exit(void) {return true; }
static int sec_bat_adc_ap_read(unsigned int channel)
{
	int rc, data;
	struct pm8xxx_adc_chan_result result;

	data = 0;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_FULL_CHECK:
		rc = pm8xxx_adc_mpp_config_read(
			PM8XXX_AMUX_MPP_9, ADC_MPP_1_AMUX6, &result);
		if (rc) {
			pr_err("error reading mpp %d, rc = %d\n",
				PM8XXX_AMUX_MPP_9, rc);
			return rc;
		}

		/* use measurement, no need to scale */
		data = (int)result.measurement;
		break;
	}

	return data;
}

#define SMTPE811_CHANNEL_ADC_CHECK_1	6
#define SMTPE811_CHANNEL_VICHG		0

static bool sec_bat_adc_ic_init(struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ic_exit(void) {return true; }
static int sec_bat_adc_ic_read(unsigned int channel)
{
	int data;
	int max_voltage;

	data = 0;
	if (system_rev == BOARD_REV02)
		max_voltage = 3300;
	else
		max_voltage = 3000;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_CABLE_CHECK:
		data = stmpe811_adc_get_value(
			SMTPE811_CHANNEL_ADC_CHECK_1);
		data = data * max_voltage / 4096;
		break;
	case SEC_BAT_ADC_CHANNEL_FULL_CHECK:
		data = stmpe811_adc_get_value(
			SMTPE811_CHANNEL_VICHG);
		data = data * max_voltage / 4096;
		break;
	}

	return data;
}

static bool sec_bat_gpio_init(void)
{
	msm_otg_pmic_gpio_config(PMIC_GPIO_TA_nCONNECTED, PM_GPIO_DIR_IN,
			PM_GPIO_PULL_UP_30, "TA_nCONNECTED", 1);

	if (system_rev == BOARD_REV02)
		gpio_tlmm_config(GPIO_CFG(GPIO_IF_CON_SENSE,  0, GPIO_CFG_INPUT,
			 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	else if (system_rev >= BOARD_REV03)
		msm_otg_pmic_gpio_config(PMIC_GPIO_IF_CON_SENSE, PM_GPIO_DIR_IN,
				PM_GPIO_PULL_NO, "IF_CON_SENSE", 1);

	return true;
}

static bool sec_fg_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SCL, 0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SDA,  0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SCL, 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SDA, 1);

	gpio_tlmm_config(GPIO_CFG(GPIO_FUEL_INT,  0, GPIO_CFG_INPUT,
		GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);

	return true;
}

static bool sec_chg_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SCL, 0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SDA,  0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SCL, 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SDA, 1);

	return true;
}

static bool sec_bat_is_lpm(void) {return false; }

static void sec_bat_initial_check(void)
{
	union power_supply_propval value;

	if (!gpio_get_value_cansleep(
		PM8921_GPIO_PM_TO_SYS(PMIC_GPIO_TA_nCONNECTED)))
		psy_do_property("battery", set,
			POWER_SUPPLY_PROP_ONLINE, value);
}

static bool sec_bat_check_jig_status(void)
{
	if ((system_rev == BOARD_REV02 &&
		gpio_get_value(GPIO_IF_CON_SENSE)) ||
		(system_rev >= BOARD_REV03 &&
		gpio_get_value_cansleep(
		PM8921_GPIO_PM_TO_SYS(
		PMIC_GPIO_IF_CON_SENSE))))
		return false;
	else
		return true;
}

static void sec_bat_switch_to_check(void)
{
	pr_debug("%s\n", __func__);

	gpio_tlmm_config(GPIO_CFG(GPIO_USB_SEL, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), 1);

	mdelay(200);

	gpio_set_value(GPIO_USB_SEL, 1);

	mdelay(10);
}

static void sec_bat_switch_to_normal(void)
{
	pr_debug("%s\n", __func__);

	gpio_tlmm_config(GPIO_CFG(GPIO_USB_SEL, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), 0);
	gpio_set_value(GPIO_USB_SEL, 0);
}

int current_cable_type = POWER_SUPPLY_TYPE_BATTERY;
EXPORT_SYMBOL(current_cable_type);

static int sec_bat_check_cable_callback(void)
{
	return current_cable_type;
}

static bool sec_bat_check_cable_result_callback(
				int cable_type)
{
	current_cable_type = cable_type;

	switch (cable_type) {
	case POWER_SUPPLY_TYPE_USB:
		pr_info("%s set vbus applied\n",
			__func__);
		break;

	case POWER_SUPPLY_TYPE_BATTERY:
		pr_info("%s set vbus cut\n",
			__func__);
		msm_otg_set_charging_state(0);
		break;
	case POWER_SUPPLY_TYPE_MAINS:
		msm_otg_set_charging_state(1);
		break;
	default:
		pr_err("%s cable type (%d)\n",
			__func__, cable_type);
		return false;
	}
	return true;
}

/* callback for battery check
 * return : bool
 * true - battery detected, false battery NOT detected
 */
static bool sec_bat_check_callback(void) {return true; }
static bool sec_bat_check_result_callback(void) {return true; }

/* callback for OVP/UVLO check
 * return : int
 * battery health
 */
static int sec_bat_ovp_uvlo_callback(void)
{
	int health;
	health = POWER_SUPPLY_HEALTH_GOOD;

	return health;
}

static bool sec_bat_ovp_uvlo_result_callback(int health) {return true; }

/*
 * val.intval : temperature
 */
static bool sec_bat_get_temperature_callback(
		enum power_supply_property psp,
		union power_supply_propval *val) {return true; }

static bool sec_fg_fuelalert_process(bool is_fuel_alerted) {return true; }

/* ADC region should be exclusive */
static sec_bat_adc_region_t cable_adc_value_table[] = {
	{0,	0},
	{0,	0},
	{1000,	1350},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
};

static sec_charging_current_t charging_current_table[] = {
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1500,	1500,	200,	0},
	{1500,	500,	200,	0},
	{1500,	500,	200,	0},
	{1500,	500,	200,	0},
	{1500,	500,	200,	0},
	{1500,	700,	200,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
};

static int polling_time_table[] = {
	10,	/* BASIC */
	30,	/* CHARGING */
	30,	/* DISCHARGING */
	30,	/* NOT_CHARGING */
	300,	/* SLEEP */
};

/* for MAX17050 */
static struct battery_data_t espresso_battery_data[] = {
	/* SDI battery data */
	{
		.Capacity = 0x2008,
		.low_battery_comp_voltage = 3600,
		.low_battery_table = {
			/* range, slope, offset */
			{-5000,	0,	0},	/* dummy for top limit */
			{-1250, 0,	3320},
			{-750, 97,	3451},
			{-100, 96,	3461},
			{0, 0,	3456},
		},
		.temp_adjust_table = {
			/* range, slope, offset */
			{47000, 122,	8950},
			{60000, 200,	51000},
			{100000, 0,	0},	/* dummy for top limit */
		},
		.type_str = "SDI",
	}
};

static sec_battery_platform_data_t sec_battery_pdata = {
	/* NO NEED TO BE CHANGED */
	.initial_check = sec_bat_initial_check,
	.bat_gpio_init = sec_bat_gpio_init,
	.fg_gpio_init = sec_fg_gpio_init,
	.chg_gpio_init = sec_chg_gpio_init,

	.is_lpm = sec_bat_is_lpm,
	.check_jig_status = sec_bat_check_jig_status,
	.check_cable_callback =
		sec_bat_check_cable_callback,
	.cable_switch_check = sec_bat_switch_to_check,
	.cable_switch_normal = sec_bat_switch_to_normal,
	.check_cable_result_callback =
		sec_bat_check_cable_result_callback,
	.check_battery_callback =
		sec_bat_check_callback,
	.check_battery_result_callback =
		sec_bat_check_result_callback,
	.ovp_uvlo_callback = sec_bat_ovp_uvlo_callback,
	.ovp_uvlo_result_callback =
		sec_bat_ovp_uvlo_result_callback,
	.fuelalert_process = sec_fg_fuelalert_process,
	.get_temperature_callback =
		sec_bat_get_temperature_callback,

	.adc_api[SEC_BATTERY_ADC_TYPE_NONE] = {
		.init = sec_bat_adc_none_init,
		.exit = sec_bat_adc_none_exit,
		.read = sec_bat_adc_none_read
		},
	.adc_api[SEC_BATTERY_ADC_TYPE_AP] = {
		.init = sec_bat_adc_ap_init,
		.exit = sec_bat_adc_ap_exit,
		.read = sec_bat_adc_ap_read
		},
	.adc_api[SEC_BATTERY_ADC_TYPE_IC] = {
		.init = sec_bat_adc_ic_init,
		.exit = sec_bat_adc_ic_exit,
		.read = sec_bat_adc_ic_read
		},
	.cable_adc_value = cable_adc_value_table,
	.charging_current = charging_current_table,
	.polling_time = polling_time_table,
	/* NO NEED TO BE CHANGED */

	.pmic_name = SEC_BATTERY_PMIC_NAME,

	.adc_check_count = 6,
	.adc_type = {
		SEC_BATTERY_ADC_TYPE_IC,	/* CABLE_CHECK */
		SEC_BATTERY_ADC_TYPE_NONE,	/* BAT_CHECK */
		SEC_BATTERY_ADC_TYPE_NONE,	/* TEMP */
		SEC_BATTERY_ADC_TYPE_NONE,	/* TEMP_AMB */
		SEC_BATTERY_ADC_TYPE_AP,	/* FULL_CHECK */
	},

	/* Battery */
	.vendor = "SDI SDI",
	.technology = POWER_SUPPLY_TECHNOLOGY_LION,
	.battery_data = (void *)espresso_battery_data,
	.bat_gpio_ta_nconnected =
		PM8921_GPIO_PM_TO_SYS(PMIC_GPIO_TA_nCONNECTED),
	.bat_polarity_ta_nconnected = 0,
	.bat_irq =
		PM8921_GPIO_IRQ(PM8921_IRQ_BASE, PMIC_GPIO_TA_nCONNECTED),
	.bat_irq_attr =
		IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.cable_check_type =
		SEC_BATTERY_CABLE_CHECK_NOUSBCHARGE |
		SEC_BATTERY_CABLE_CHECK_INT,
	.cable_source_type = SEC_BATTERY_CABLE_SOURCE_ADC,

	.event_check = true,
	.event_waiting_time = 600,

	/* Monitor setting */
	.polling_type = SEC_BATTERY_MONITOR_ALARM,
	.monitor_initial_count = 3,

	/* Battery check */
	.battery_check_type = SEC_BATTERY_CHECK_NONE,
	.check_count = 3,
	/* Battery check by ADC */
	.check_adc_max = 0,
	.check_adc_min = 0,

	/* OVP/UVLO check */
	.ovp_uvlo_check_type = SEC_BATTERY_OVP_UVLO_CHGINT,

	/* Temperature check */
	.thermal_source = SEC_BATTERY_THERMAL_SOURCE_FG,

	.temp_check_type = SEC_BATTERY_TEMP_CHECK_TEMP,
	.temp_check_count = 3,
	.temp_high_threshold_event = 638,
	.temp_high_recovery_event = 428,
	.temp_low_threshold_event = -20,
	.temp_low_recovery_event = 6,
	.temp_high_threshold_normal = 533,
	.temp_high_recovery_normal = 428,
	.temp_low_threshold_normal = -20,
	.temp_low_recovery_normal = 6,
	.temp_high_threshold_lpm = 533,
	.temp_high_recovery_lpm = 428,
	.temp_low_threshold_lpm = -20,
	.temp_low_recovery_lpm = 6,

	.full_check_type = SEC_BATTERY_FULLCHARGED_ADC,
	.full_check_count = 2,
	.full_check_adc_1st = 26500,
	.full_check_adc_2nd = 25800,
	.chg_gpio_full_check =
		PM8921_GPIO_PM_TO_SYS(PMIC_GPIO_CHG_STAT),
	.chg_polarity_full_check = 1,
	.full_condition_type =
		SEC_BATTERY_FULL_CONDITION_SOC |
		SEC_BATTERY_FULL_CONDITION_VCELL,
	.full_condition_soc = 99,
	.full_condition_vcell = 4000,

	.recharge_condition_type =
		SEC_BATTERY_RECHARGE_CONDITION_VCELL,
	.recharge_condition_vcell = 4140,

	.charging_total_time = 8 * 60 * 60,
	.recharging_total_time = 2 * 60 * 60,
	.charging_reset_time = 10 * 60,

	/* Fuel Gauge */
	.fg_irq = 0,
	.fg_irq_attr = IRQF_TRIGGER_LOW | IRQF_ONESHOT,
	.fuel_alert_soc = 1,
	.repeated_fuelalert = false,
	.capacity_calculation_type =
		SEC_FUELGAUGE_CAPACITY_TYPE_RAW,
		/* SEC_FUELGAUGE_CAPACITY_TYPE_SCALE | */
		/* SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC, */
	.capacity_max = 1000,
	.capacity_min = 0,

	/* Charger */
	.chg_gpio_en = 0,
	.chg_polarity_en = 0,
	.chg_gpio_status =
		PM8921_GPIO_PM_TO_SYS(PMIC_GPIO_CHG_STAT),
	.chg_polarity_status = 0,
	.chg_irq = 0,
	.chg_irq_attr = 0,
	.chg_float_voltage = 4200,
};

static struct platform_device sec_device_battery = {
	.name = "sec-battery",
	.id = -1,
	.dev.platform_data = &sec_battery_pdata,
};

static struct i2c_gpio_platform_data gpio_i2c_data_fgchg = {
	.sda_pin = GPIO_FUELGAUGE_I2C_SDA,
	.scl_pin = GPIO_FUELGAUGE_I2C_SCL,
};

struct platform_device sec_device_fgchg = {
	.name = "i2c-gpio",
	.id = MSM_FUELGAUGE_I2C_BUS_ID,
	.dev.platform_data = &gpio_i2c_data_fgchg,
};

static struct i2c_board_info sec_brdinfo_fgchg[] __initdata = {
	{
		I2C_BOARD_INFO("sec-charger",
			SEC_CHARGER_I2C_SLAVEADDR),
		.platform_data	= &sec_battery_pdata,
	},
	{
		I2C_BOARD_INFO("sec-fuelgauge",
			SEC_FUELGAUGE_I2C_SLAVEADDR),
		.platform_data	= &sec_battery_pdata,
	},
};

static struct platform_device *msm8960_battery_devices[] __initdata = {
	&sec_device_fgchg,
	&sec_device_battery,
};

void __init msm8960_init_battery(void)
{
	/* board dependent changes in booting */
	if (system_rev >= BOARD_REV04) {
		sec_battery_pdata.adc_type[SEC_BAT_ADC_CHANNEL_FULL_CHECK] =
			SEC_BATTERY_ADC_TYPE_IC;
		sec_battery_pdata.full_check_adc_1st = 160;
		sec_battery_pdata.full_check_adc_2nd = 150;
	}

	platform_add_devices(
		msm8960_battery_devices,
		ARRAY_SIZE(msm8960_battery_devices));

	i2c_register_board_info(
		MSM_FUELGAUGE_I2C_BUS_ID,
		sec_brdinfo_fgchg,
		ARRAY_SIZE(sec_brdinfo_fgchg));
}

#endif

