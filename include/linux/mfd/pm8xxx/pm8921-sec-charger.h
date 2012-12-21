/*
 * include/linux/mfd/pm8xxx/pm8921-sec-charger.h
 *
 * Copyright (c) 2011 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PM8XXX_SEC_CHARGER_H
#define __PM8XXX_SEC_CHARGER_H

/*******************************************
** Feature definition
********************************************/
#undef QUALCOMM_TEMPERATURE_CONTROL
#undef QUALCOMM_POWERSUPPLY_PROPERTY

/*******************************************
** Other definition
*******************************************/
#if defined(CONFIG_BATTERY_MAX17040) || \
	defined(CONFIG_BATTERY_MAX17042)
#define RCOMP0_TEMP	20	/* 'C */

#define FG_T_SOC	0
#define FG_T_VCELL	1
#define FG_T_PSOC	2
#define FG_T_RCOMP	3
#define FG_T_FSOC	4
#define FG_RESET	5
#endif

enum cable_type_t {
	CABLE_TYPE_NONE = 0,
	CABLE_TYPE_USB,
	CABLE_TYPE_AC,
	CABLE_TYPE_MISC,
	CABLE_TYPE_CARDOCK,
	CABLE_TYPE_UARTOFF,
	CABLE_TYPE_JIG,
	CABLE_TYPE_UNKNOWN,
	CABLE_TYPE_CDP,
	CABLE_TYPE_SMART_DOCK,
#ifdef CONFIG_WIRELESS_CHARGING
	CABLE_TYPE_WPC = 10,
#endif
};

#define VUBS_IN_CURR_NONE	2
#define VBUS_IN_CURR_USB	500
#define BATT_IN_CURR_USB	475
#define	VBUS_IN_CURR_TA		1100
#define	BATT_IN_CURR_TA		1025
#ifdef CONFIG_WIRELESS_CHARGING
#define VBUS_IN_CURR_WPC	700
#define BATT_IN_CURR_WPC	675
#endif

#define TEMP_GPIO	PM8XXX_AMUX_MPP_7
#define TEMP_ADC_CHNNEL	ADC_MPP_1_AMUX6

static const int temper_table[][2] = {
	{1600, -200},
	{1550, -150},
	{1500, -100},
	{1400, -50},
	{1300, 0},
	{1150, 100},
	{1050, 150},
	{950, 200},
	{850, 250},
	{750, 300},
	{650, 350},
	{600, 400},
	{500, 450},
	{400, 500},
	{330, 550},
	{300, 600},
};

/*******************************************
** for Debug screen
*****************************************/
struct pm8921_reg {
	u8	chg_cntrl;
	u8	chg_cntrl_2;
	u8	chg_cntrl_3;
	u8	pbl_access1;
	u8	pbl_access2;
	u8	sys_config_1;
	u8	sys_config_2;
	u8	chg_vdd_max;
	u8	chg_vdd_safe;
	u8	chg_ibat_max;
	u8	chg_ibat_safe;
	u8	chg_iterm;
};
struct pm8921_irq {
	int	usbin_valid;
	int	usbin_ov;
	int usbin_uv;
	int batttemp_hot;
	int batttemp_cold;
	int batt_inserted;
	int trklchg;
	int fastchg;
	int batfet;
	int batt_removed;
	int vcp;
	int bat_temp_ok;
};


#endif
