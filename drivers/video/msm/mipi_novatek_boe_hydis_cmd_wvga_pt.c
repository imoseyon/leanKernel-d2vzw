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
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_novatek.h"


static struct msm_panel_info pinfo;

static struct mipi_panel_data mipi_pd;
/*************************BOE Panel Init Sequence  /////Start    **********/
/* Power Sequence(Page 1) */
static char etc_cond_set_1_1_boe[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x01
};
static char etc_cond_set_1_2_boe[] = {
	0xB0,
	0x09, 0x09, 0x09
};
static char etc_cond_set_1_3_boe[] = {
	0xB6,
	0x34, 0x34, 0x34
};
static char etc_cond_set_1_4_boe[] = {
	0xB1,
	0x09, 0x09, 0x09
};
static char etc_cond_set_1_5_boe[] = {
	0xB7,
	0x24, 0x24, 0x24
};
static char etc_cond_set_1_6_boe[] = {
	0xB3,
	0x05, 0x05, 0x05
};
static char etc_cond_set_1_7_boe[] = {
	0xB9,
	0x24, 0x24, 0x24
};
static char etc_cond_set_1_8_boe[] = {
	0xBF,
	0x01
};
static char etc_cond_set_1_9_boe[] = {
	0xB5,
	0x0B, 0x0B, 0x0B
};
static char etc_cond_set_1_101_boe[] = {
	0xBA,
	0x34, 0x34, 0x34
};
static char etc_cond_set_1_10_boe[] = {
	0xBC,
	0x00, 0xA3, 0x00
};
static char etc_cond_set_1_11_boe[] = {
	0xBD,
	0x00, 0xA3, 0x00
};
/* Init Sequence Begin(Page 0) */
static char etc_cond_set_1_12_boe[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x00
};
static char etc_cond_set_1_13_boe[] = {
	0xB6,
	0x0A
};
static char etc_cond_set_1_14_boe[] = {
	0xB7,
	0x00, 0x00
};
static char etc_cond_set_1_15_boe[] = {
	0xB8,
	0x01, 0x05, 0x05, 0x05
};
static char etc_cond_set_1_16_boe[] = {
	0xBA,
	0x01,
};

static char etc_cond_set_1_18_boe[] = {
	0xBD,
	0x01, 0x4D, 0x07, 0x32, 0x00
};
static char etc_cond_set_1_19_boe[] = {
	0xBE,
	0x01, 0x84, 0x07, 0x31, 0x00
};
static char etc_cond_set_1_20_boe[] = {
	0xBF,
	0x01, 0x84, 0x07, 0x31, 0x00
};
static char etc_cond_set_1_21_boe[] = {
	0xCC,
	0x03, 0x00, 0x00,
};
static char etc_cond_set_1_22_boe[] = {
	0x36,
	0x00
};
static char etc_cond_set_1_23_boe[] = {
	0x35,
	0x00
};
static char etc_cond_set_1_24_boe[] = {
	0xB1,
	0x4C, 0x06
};
static char etc_cond_set_1_25_boe[] = {
	0x44,
	0x00, 0x06
};

/* gamma settings */


static char gamma_cond_set_1_1r_boe[] = {
	0xD1,
	0x00,
	0x01, 0x00, 0x60, 0x00, 0x82,
	0x00, 0xAE, 0x00, 0xC6, 0x00,
	0xE9, 0x01, 0x0E, 0x01, 0x3E,
	0x01, 0x64, 0x01, 0xA1, 0x01,
	0xD4, 0x02, 0x1D, 0x02, 0x7C,
	0x02, 0x71, 0x02, 0x91, 0x02,
	0xC9, 0x02, 0xEC, 0x03, 0x18,
	0x03, 0x36, 0x03, 0x61, 0x03,
	0x77, 0x03, 0x95, 0x03, 0xA4, 0x03, 0xC3,
	0x03, 0xD3, 0x03, 0xFE
};
static char gamma_cond_set_1_1g_boe[] = {
	0xD2,
	0x00,
	0x01, 0x00, 0x60, 0x00, 0x82,
	0x00, 0xAE, 0x00, 0xC6, 0x00,
	0xE9, 0x01, 0x0E, 0x01, 0x3E,
	0x01, 0x64, 0x01, 0xA1, 0x01,
	0xD4, 0x02, 0x1D, 0x02, 0x7C,
	0x02, 0x71, 0x02, 0x91, 0x02,
	0xC9, 0x02, 0xEC, 0x03, 0x18,
	0x03, 0x36, 0x03, 0x61, 0x03,
	0x77, 0x03, 0x95, 0x03, 0xA4, 0x03, 0xc3,
	0x03, 0xD3, 0x03, 0xFE
};
static char gamma_cond_set_1_1b_boe[] = {
	0xD3,
	0x00,
	0x01, 0x00, 0x60, 0x00, 0x82,
	0x00, 0xAE, 0x00, 0xC6, 0x00,
	0xE9, 0x01, 0x0E, 0x01, 0x3E,
	0x01, 0x64, 0x01, 0xA1, 0x01,
	0xD4, 0x02, 0x1D, 0x02, 0x7C,
	0x02, 0x71, 0x02, 0x91, 0x02,
	0xC9, 0x02, 0xEC, 0x03, 0x18,
	0x03, 0x36, 0x03, 0x61, 0x03,
	0x77, 0x03, 0x95, 0x03, 0xA4, 0x03, 0xc3,
	0x03, 0xD3, 0x03, 0xFE
};
static char gamma_cond_set_1_2r_boe[] = {
	0xD4,
	0x00,
	0x01, 0x00, 0x60, 0x00, 0x82,
	0x00, 0xAE, 0x00, 0xC6, 0x00,
	0xE9, 0x01, 0x0E, 0x01, 0x3E,
	0x01, 0x64, 0x01, 0xA1, 0x01,
	0xD4, 0x02, 0x1D, 0x02, 0x3C,
	0x02, 0x49, 0x02, 0x91, 0x02,
	0xC9, 0x02, 0xEC, 0x03, 0x18,
	0x03, 0x36, 0x03, 0x61, 0x03,
	0x77, 0x03, 0x95, 0x03, 0xA4, 0x03, 0xc3,
	0x03, 0xD3, 0x03, 0xFE
};
static char gamma_cond_set_1_2g_boe[] = {
	0xD5,
	0x00,
	0x01, 0x00, 0x60, 0x00, 0x82,
	0x00, 0xAE, 0x00, 0xC6, 0x00,
	0xE9, 0x01, 0x0E, 0x01, 0x3E,
	0x01, 0x64, 0x01, 0xA1, 0x01,
	0xD4, 0x02, 0x1D, 0x02, 0x3C,
	0x02, 0x49, 0x02, 0x91, 0x02,
	0xC9, 0x02, 0xEC, 0x03, 0x18,
	0x03, 0x36, 0x03, 0x61, 0x03,
	0x77, 0x03, 0x95, 0x03, 0xA4, 0x03, 0xc3,
	0x03, 0xD3, 0x03, 0xFE
};
static char gamma_cond_set_1_2b_boe[] = {
	0xD6,
	0x00,
	0x01, 0x00, 0x60, 0x00, 0x82,
	0x00, 0xAE, 0x00, 0xC6, 0x00,
	0xE9, 0x01, 0x0E, 0x01, 0x3E,
	0x01, 0x64, 0x01, 0xA1, 0x01,
	0xD4, 0x02, 0x1D, 0x02, 0x3C,
	0x02, 0x49, 0x02, 0x91, 0x02,
	0xC9, 0x02, 0xEC, 0x03, 0x18,
	0x03, 0x36, 0x03, 0x61, 0x03,
	0x77, 0x03, 0x95, 0x03, 0xA4, 0x03, 0xc3,
	0x03, 0xD3, 0x03, 0xFE
};

static int lux_tbl_boe[] = {
	 7, 7, 20, 35, 43, 48, 50, 55, 60, 65, 70, 75, 80, 85, 90,
	95, 100, 105, 110, 120, 130, 140, 150, 168, 185, 203
};

static int get_candela_index_boe(int bl_level)
{

	int cd;
	int div;
	int count;

	count = MAX_GAMMA_VALUE + 1;
	div = 225 / count;
	cd = (bl_level / div) - (30 / div) - 1;

	if (cd >= count)
		cd = count - 1;
	if (cd < 0)
		cd = 0;
	if (cd == 0)
		cd = 1;

	cd = lux_tbl_boe[cd];

	return cd;

}

/**********************BOE Panel Init sequence ////END *********************/



/*********************  HYDIS Panel Init Sequence //START *******************/
static char etc_cond_set_1_1_hydis[] = {
	0xFF,
	0xAA, 0x55, 0x25, 0x01,
};
static char etc_cond_set_1_2_hydis[] = {
	0xF3,
	0x00, 0x32, 0x00, 0x38,
	0x31, 0x08, 0x11, 0x00
};
static char etc_cond_set_1_3_hydis[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x00
};
static char etc_cond_set_1_4_hydis[] = {
	0xB0,
	0x04, 0x0A, 0x0E, 0x09, 0x04
};
#ifdef CONFIG_MACH_APEXQ
static char etc_cond_set_1_5_hydis[] = {
	0xB1,
	0xCC, 0x04
};
#else
static char etc_cond_set_1_5_hydis[] = {
	0xB1,
	0xCC, 0x0C
};
#endif

/*
 * For Revision 0.0 of ApexQ, the display orientation is changed
 * Hence changing the filling order on panel side using Novatek's
 * MADCTL: Memory Data Access Control (3600h) register.
 */
#ifdef CONFIG_MACH_APEXQ
static char etc_cond_set_1_6_hydis[] = {
	0x36,
	0x11,
};
#else
static char etc_cond_set_1_6_hydis[] = {
	0x36,
	0x0A,
};
#endif /* CONFIG_MACH_APEXQ */
static char etc_cond_set_1_7_hydis[] = {
	0xB3,
	0x00
};
static char etc_cond_set_1_8_hydis[] = {
	0xB6,
	0x03
};
static char etc_cond_set_1_9_hydis[] = {
	0xB7,
	0x70,
	0x70
};
static char etc_cond_set_1_10_hydis[] = {
	0xB8,
	0x00, 0x06, 0x06, 0x06,
};
static char etc_cond_set_1_11_hydis[] = {
	0xBC,
	0x00, 0x00, 0x00,
};
static char etc_cond_set_1_12_hydis[] = {
	0xBD,
	0x01, 0x84, 0x06, 0x50, 0x00
};
static char etc_cond_set_1_13_hydis[] = {
	0xCC,
	0x03, 0x2A, 0x06
};
static char etc_cond_set_1_14_hydis[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x01
};
static char etc_cond_set_1_15_hydis[] = {
	0xB0,
	0x05, 0x05, 0x05,
};
static char etc_cond_set_1_16_hydis[] = {
	0xB1,
	0x05, 0x05, 0x05,
};
static char etc_cond_set_1_17_hydis[] = {
	0xB2,
	0x03, 0x03, 0x03,
};
static char etc_cond_set_1_18_hydis[] = {
	0xB8,
	0x24, 0x24, 0x24,
};

#ifdef CONFIG_MACH_APEXQ
/*
 * In ApexQ, horizontal lines were observed on panel which was
 * not appearing in Jasper. But by changing the VGH voltage
 * value from 18V to 15V, these lines were not visible.
 *
 * The reason for this behaviour has to be explored, until then
 * this is more like a work around rather than a solution.
 */
static char etc_cond_set_1_19_hydis[] = {
	0xB3,
	0x08, 0x08, 0x08,
};
#else
static char etc_cond_set_1_19_hydis[] = {
	0xB3,
	0x0A, 0x0A, 0x0A,
};
#endif
static char etc_cond_set_1_20_hydis[] = {
	0xB9,
	0x24, 0x24, 0x24,
};
static char etc_cond_set_1_21_hydis[] = {
	0xBF,
	0x01
};
static char etc_cond_set_1_22_hydis[] = {
	0xB5,
	0x08, 0x08, 0x08, 0x08,
};
static char etc_cond_set_1_24_hydis[] = {
	0xB4,
	0x2D, 0x2D, 0x2D
};
static char etc_cond_set_1_25_hydis[] = {
	0xBC,
	0x00, 0x50, 0x00
};
static char etc_cond_set_1_26_hydis[] = {
	0xBD,
	0x00, 0x60, 0x00,
};
static char etc_cond_set_1_27_hydis[] = {
	0xBE,
	0x00, 0x3D,
};
static char etc_cond_set_1_28_hydis[] = {
	0xCE,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* gamma settings */
static char gamma_cond_set_1_0_hydis[] = {
	0xD0,
	0x0D, 0x15, 0x08, 0x0C
};

static char gamma_cond_set_1_1r_hydis[] = {
	0xD1,
	0x00,
	0x37, 0x00, 0x71, 0x00, 0xA2, 0x00,
	0xC4, 0x00, 0xDB, 0x01, 0x01, 0x01,
	0x40, 0x01, 0x84, 0x01, 0xA9, 0x01,
	0xD8, 0x02, 0x0A, 0x02, 0x44, 0x02,
	0x85, 0x02, 0x87, 0x02, 0xBF, 0x02,
	0xE5, 0x03, 0x0F, 0x03, 0x34, 0x03,
	0x4F, 0x03, 0x73, 0x03, 0x77, 0x03,
	0x94, 0x03, 0x9E, 0x03, 0xAC, 0x03,
	0xBD, 0x03, 0xF1
};
static char gamma_cond_set_1_1g_hydis[] = {
	0xD2,
	0x00,
	0x37, 0x00, 0x71, 0x00, 0xA2, 0x00,
	0xC4, 0x00, 0xDB, 0x01, 0x01, 0x01,
	0x40, 0x01, 0x84, 0x01, 0xA9, 0x01,
	0xD8, 0x02, 0x0A, 0x02, 0x44, 0x02,
	0x85, 0x02, 0x87, 0x02, 0xBF, 0x02,
	0xE5, 0x03, 0x0F, 0x03, 0x34, 0x03,
	0x4F, 0x03, 0x73, 0x03, 0x77, 0x03,
	0x94, 0x03, 0x9E, 0x03, 0xAC, 0x03,
	0xBD, 0x03, 0xF1
};
static char gamma_cond_set_1_1b_hydis[] = {
	0xD3,
	0x00,
	0x37, 0x00, 0x71, 0x00, 0xA2, 0x00,
	0xC4, 0x00, 0xDB, 0x01, 0x01, 0x01,
	0x40, 0x01, 0x84, 0x01, 0xA9, 0x01,
	0xD8, 0x02, 0x0A, 0x02, 0x44, 0x02,
	0x85, 0x02, 0x87, 0x02, 0xBF, 0x02,
	0xE5, 0x03, 0x0F, 0x03, 0x34, 0x03,
	0x4F, 0x03, 0x73, 0x03, 0x77, 0x03,
	0x94, 0x03, 0x9E, 0x03, 0xAC, 0x03,
	0xBD, 0x03, 0xF1
};
static char gamma_cond_set_1_2r_hydis[] = {
	0xD4,
	0x00,
	0x37, 0x00, 0x46, 0x00, 0x7E, 0x00,
	0x9E, 0x00, 0xC2, 0x01, 0x01, 0x01,
	0x14, 0x01, 0x4A, 0x01, 0x73, 0x01,
	0xB8, 0x01, 0xDF, 0x02, 0x2F, 0x02,
	0x68, 0x02, 0x6A, 0x02, 0xA3, 0x02,
	0xE0, 0x02, 0xF9, 0x03, 0x25, 0x03,
	0x43, 0x03, 0x6E, 0x03, 0x77, 0x03,
	0x94, 0x03, 0x9E, 0x03, 0xAC, 0x03,
	0xBD, 0x03, 0xF1
};
static char gamma_cond_set_1_2g_hydis[] = {
	0xD5,
	0x00,
	0x37, 0x00, 0x46, 0x00, 0x7E, 0x00,
	0x9E, 0x00, 0xC2, 0x01, 0x01, 0x01,
	0x14, 0x01, 0x4A, 0x01, 0x73, 0x01,
	0xB8, 0x01, 0xDF, 0x02, 0x2F, 0x02,
	0x68, 0x02, 0x6A, 0x02, 0xA3, 0x02,
	0xE0, 0x02, 0xF9, 0x03, 0x25, 0x03,
	0x43, 0x03, 0x6E, 0x03, 0x77, 0x03,
	0x94, 0x03, 0x9E, 0x03, 0xAC, 0x03,
	0xBD, 0x03, 0xF1
};
static char gamma_cond_set_1_2b_hydis[] = {
	0xD6,
	0x00,
	0x37, 0x00, 0x46, 0x00, 0x7E, 0x00,
	0x9E, 0x00, 0xC2, 0x01, 0x01, 0x01,
	0x14, 0x01, 0x4A, 0x01, 0x73, 0x01,
	0xB8, 0x01, 0xDF, 0x02, 0x2F, 0x02,
	0x68, 0x02, 0x6A, 0x02, 0xA3, 0x02,
	0xE0, 0x02, 0xF9, 0x03, 0x25, 0x03,
	0x43, 0x03, 0x6E, 0x03, 0x77, 0x03,
	0x94, 0x03, 0x9E, 0x03, 0xAC, 0x03,
	0xBD, 0x03, 0xF1
};

#ifdef CONFIG_MACH_APEXQ
static int lux_tbl_hydis[] = {
	0, 8, 13, 15, 20, 30, 40, 50, 60, 70, 80, 95, 100,
	105, 110, 115, 120, 125, 130, 140, 150, 160, 170, 180, 190, 200, 205,
};
#else
static int lux_tbl_hydis[] = {
	6, 6, 6, 6, 6, 6, 6, 10, 15, 20,
	25, 30, 35, 40, 45, 50, 55, 60, 65, 70,
	75, 80, 83, 87, 92, 95, 98, 102, 105, 108,
	110, 110, 110, 120, 130, 135, 140, 150, 160, 170,
	180, 190, 200, 210, 220, 225, 230, 235, 240, 240

	};

#endif
static int get_candela_index_hydis(int bl_level)
{

	int cd;
	int div;
	int count;

	count = MAX_GAMMA_VALUE + 1;
	div = MAX_BL_LEVEL / count;
	cd = (bl_level / div) - (MIN_BL_LEVEL / div) - INDEX_OFFSET;

	if (cd >= count)
		cd = count - 1;
	if (cd < 0)
		cd = 0;
	if (cd == 0)
		cd = 1;

	cd = lux_tbl_hydis[cd];

	return cd;

}
/*********************  HYDIS Panel Init Sequence //END *******************/

/* PWM settings */
#ifdef CONFIG_FB_MSM_BACKLIGHT_AAT1402IUQ
static char pwm_cond_set_1_0[] = {
	0x51,
	0xFF
};
#else
static char pwm_cond_set_1_0[] = {
	0x51,
	0x6C
};
#endif /* CONFIG_FB_MSM_BACKLIGHT_AAT1402IUQ */
static char pwm_cond_set_2_0[] = {
	0x53,
	0x2C
};
static char pwm_cond_off[] = {
	0x51,
	0x00
};


static char sw_reset[2] = {0x01, 0x00}; /* DTYPE_DCS_WRITE */
static char all_pixel_off[] = { 0x22, /* no param */ };
static char normal_mode_on[] = { 0x13, /* no parm */ };
static char display_on[] = { 0x29, /* no param */ };
static char display_off[] = { 0x28, /* no param */ };
static char sleep_in[] = { 0x10, /* no param */ };
static char sleep_out[] = { 0x11, /* no param */ };

static struct dsi_cmd_desc novatek_panel_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc novatek_panel_late_on_cmds[] = {

	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(normal_mode_on), normal_mode_on},
	{DTYPE_DCS_WRITE, 1, 0, 0, 5,
		sizeof(display_on), display_on},
};


static struct dsi_cmd_desc novatek_panel_ready_to_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(display_off), display_off},
};

static struct dsi_cmd_desc novatek_panel_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(sleep_in), sleep_in},
};


static struct dsi_cmd_desc novatek_panel_early_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(all_pixel_off), all_pixel_off},
};

static struct dsi_cmd_desc novatek_panel_ready_to_on_cmds_nt_boe[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 150,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_1_boe), etc_cond_set_1_1_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_2_boe), etc_cond_set_1_2_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_3_boe), etc_cond_set_1_3_boe},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_4_boe), etc_cond_set_1_4_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_5_boe), etc_cond_set_1_5_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_6_boe), etc_cond_set_1_6_boe},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_7_boe), etc_cond_set_1_7_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_8_boe), etc_cond_set_1_8_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_9_boe), etc_cond_set_1_9_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_101_boe), etc_cond_set_1_101_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_10_boe), etc_cond_set_1_10_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 100,
		sizeof(etc_cond_set_1_11_boe), etc_cond_set_1_11_boe},
	/*GAMMA SETTINGS*/
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_1r_boe), gamma_cond_set_1_1r_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_1g_boe), gamma_cond_set_1_1g_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_1b_boe), gamma_cond_set_1_1b_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_2r_boe), gamma_cond_set_1_2r_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_2g_boe), gamma_cond_set_1_2g_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_2b_boe), gamma_cond_set_1_2b_boe},
		/*INIT SEQUENCE*/
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_12_boe), etc_cond_set_1_12_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_13_boe), etc_cond_set_1_13_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_14_boe), etc_cond_set_1_14_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_15_boe), etc_cond_set_1_15_boe},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_16_boe), etc_cond_set_1_16_boe},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_18_boe), etc_cond_set_1_18_boe},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_19_boe), etc_cond_set_1_19_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_20_boe), etc_cond_set_1_20_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_21_boe), etc_cond_set_1_21_boe},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_22_boe), etc_cond_set_1_22_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_23_boe), etc_cond_set_1_23_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_24_boe), etc_cond_set_1_24_boe},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_25_boe), etc_cond_set_1_25_boe},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 150,
		sizeof(sleep_out), sleep_out},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(pwm_cond_off), pwm_cond_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(pwm_cond_set_2_0), pwm_cond_set_2_0},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc novatek_panel_ready_to_on_cmds_nt_hydis[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_1_hydis), etc_cond_set_1_1_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_2_hydis), etc_cond_set_1_2_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_3_hydis), etc_cond_set_1_3_hydis},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_4_hydis), etc_cond_set_1_4_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_5_hydis), etc_cond_set_1_5_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_6_hydis), etc_cond_set_1_6_hydis},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_7_hydis), etc_cond_set_1_7_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_8_hydis), etc_cond_set_1_8_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_9_hydis), etc_cond_set_1_9_hydis},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_10_hydis), etc_cond_set_1_10_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_11_hydis), etc_cond_set_1_11_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_12_hydis), etc_cond_set_1_12_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_13_hydis), etc_cond_set_1_13_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_14_hydis), etc_cond_set_1_14_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_15_hydis), etc_cond_set_1_15_hydis},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_16_hydis), etc_cond_set_1_16_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_17_hydis), etc_cond_set_1_17_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_18_hydis), etc_cond_set_1_18_hydis},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_19_hydis), etc_cond_set_1_19_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_20_hydis), etc_cond_set_1_20_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_21_hydis), etc_cond_set_1_21_hydis},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_22_hydis), etc_cond_set_1_22_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_24_hydis), etc_cond_set_1_24_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_25_hydis), etc_cond_set_1_25_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_26_hydis), etc_cond_set_1_26_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_27_hydis), etc_cond_set_1_27_hydis},
			{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_28_hydis), etc_cond_set_1_28_hydis},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_0_hydis), gamma_cond_set_1_0_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_1r_hydis), gamma_cond_set_1_1r_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_1g_hydis), gamma_cond_set_1_1g_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_1b_hydis), gamma_cond_set_1_1b_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_2r_hydis), gamma_cond_set_1_2r_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_2g_hydis), gamma_cond_set_1_2g_hydis},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_cond_set_1_2b_hydis), gamma_cond_set_1_2b_hydis},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 120,
		sizeof(sleep_out), sleep_out},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(pwm_cond_off), pwm_cond_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(pwm_cond_set_2_0), pwm_cond_set_2_0},


	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(pwm_cond_set_1_0), pwm_cond_set_1_0},
};

static int get_candela_index(int bl_level)
{
	if (mipi_pd.manufacture_id == JASPER_MANUFACTURE_ID)
		return get_candela_index_hydis(bl_level);
	else
		return get_candela_index_boe(bl_level);

}

static struct mipi_panel_data mipi_pd = {
	.panel_name = "NOVATEK_NT35510\n",
	.ready_to_on_hydis = {novatek_panel_ready_to_on_cmds_nt_hydis
			, ARRAY_SIZE(novatek_panel_ready_to_on_cmds_nt_hydis)},
	.ready_to_off = {novatek_panel_ready_to_off_cmds
			, ARRAY_SIZE(novatek_panel_ready_to_off_cmds)},
	.ready_to_on_boe = {novatek_panel_ready_to_on_cmds_nt_boe
			, ARRAY_SIZE(novatek_panel_ready_to_on_cmds_nt_boe)},
	.on = {novatek_panel_on_cmds
			, ARRAY_SIZE(novatek_panel_on_cmds)},
	.off = {novatek_panel_off_cmds
			, ARRAY_SIZE(novatek_panel_off_cmds)},
	.late_on = {novatek_panel_late_on_cmds
			, ARRAY_SIZE(novatek_panel_late_on_cmds)},
	.early_off = {novatek_panel_early_off_cmds
			, ARRAY_SIZE(novatek_panel_early_off_cmds)},
	.set_brightness_level = get_candela_index,


};


static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0xb9, 0x8e, 0x1f, 0x00, 0x98, 0x9c, 0x22, 0x90,
	0x18, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},

	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_cmd_novatek_wvga_pt_init(void)
{
	int ret;

	if (msm_fb_detect_client("mipi_cmd_novatek_wvga"))
		return 0;

	pinfo.xres = 480;
	pinfo.yres = 800;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch = 16;
	pinfo.lcdc.h_front_porch = 16;
	pinfo.lcdc.h_pulse_width = 2;
	pinfo.lcdc.v_back_porch = 0x2;
	pinfo.lcdc.v_front_porch = 0x4D;
	pinfo.lcdc.v_pulse_width = 0x1;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 320000000;
	/* Max Clock rate support for Novatek= 500MHz;*/
	/* Clock Rate = H x V x Bits per color)x FrameRate x overhead*/
	pinfo.is_3d_panel = 0;
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6000; /* adjust refx100 to prevent tearing */
	pinfo.lcd.v_back_porch = pinfo.lcdc.v_back_porch;
	pinfo.lcd.v_front_porch = pinfo.lcdc.v_front_porch;
	pinfo.lcd.v_pulse_width = pinfo.lcdc.v_pulse_width;

	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x22;
	pinfo.mipi.t_clk_pre = 0x3f;
	pinfo.mipi.stream = 0;	/* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;


	ret = mipi_novatek_disp_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WVGA_PT,
						&mipi_pd);


	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_cmd_novatek_wvga_pt_init);
