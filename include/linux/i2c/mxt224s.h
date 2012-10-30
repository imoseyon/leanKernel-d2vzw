/*
 *  Copyright (C) 2010, Samsung Electronics Co. Ltd. All Rights Reserved.
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
 *
 */

#ifndef __MXT224S_H__
#define __MXT224S_H__

#define MXT224S_MAX_MT_FINGERS		0x0A
#define MXT224S_DEV_NAME "Atmel MXT224S"
#define SPT_USERDATA_T38	38
#define SPT_DIGITIZER_T43	43
#define SPARE_T51			51
#define TOUCH_PROXIMITY_KEY_T52	52
#define GEN_DATASOURCE_T53		53
#define SPARE_T54				54
#define PROCI_ADAPTIVETHRESHOLD_T55	55
#define PROCI_SHIELDLESS_T56	56
#define PROCI_EXTRATOUCHSCREENDATA_T57	57
#define SPARE_T58	58
#define SPARE_T59	59
#define SPARE_T60	60
#define SPT_TIMER_T61	61
#define PROCG_NOISESUPPRESSION_T62	62
#define CHECK_ANTITOUCH         1

extern int touch_is_pressed;

struct mxt224s_platform_data {
	int max_finger_touches;
	const u8 **config;
	const u8 **config_e;
	int gpio_read_done;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int min_z;
	int max_z;
	int min_w;
	int max_w;
	u8 chrgtime_batt;
	u8 chrgtime_charging;
	u8 atchcalst;
	u8 atchcalsthr;
	u8 tchthr_batt;
	u8 tchthr_charging;
	u8 tchthr_batt_e;
	u8 tchthr_charging_e;
	u8 calcfg_batt_e;
	u8 calcfg_charging_e;
	u8 atchcalsthr_e;
	u8 atchfrccalthr_e;
	u8 atchfrccalratio_e;
	u8 idlesyncsperx_batt;
	u8 idlesyncsperx_charging;
	u8 actvsyncsperx_batt;
	u8 actvsyncsperx_charging;
	u8 idleacqint_batt;
	u8 idleacqint_charging;
	u8 actacqint_batt;
	u8 actacqint_charging;
	u8 xloclip_batt;
	u8 xloclip_charging;
	u8 xhiclip_batt;
	u8 xhiclip_charging;
	u8 yloclip_batt;
	u8 yloclip_charging;
	u8 yhiclip_batt;
	u8 yhiclip_charging;
	u8 xedgectrl_batt;
	u8 xedgectrl_charging;
	u8 xedgedist_batt;
	u8 xedgedist_charging;
	u8 yedgectrl_batt;
	u8 yedgectrl_charging;
	u8 yedgedist_batt;
	u8 yedgedist_charging;
	u8 tchhyst_batt;
	u8 tchhyst_charging;
#if CHECK_ANTITOUCH
	u8 check_antitouch;
	u8 check_timer;
	u8 check_autocal;
	u8 check_calgood;
#endif
	const u8 *t62_config_batt_e;
	const u8 *t62_config_chrg_e;
	const u8 *t46_config_batt_e;
	const u8 *t46_config_chrg_e;
	const u8 *t35_config_batt_e;
	const u8 *t35_config_chrg_e;
	void (*power_onoff) (int);
	void (*register_cb) (void *);
	void (*read_ta_status) (void *);
	const u8 *config_fw_version;
};
#endif

