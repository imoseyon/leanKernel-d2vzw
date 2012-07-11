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

static char etc_cond_set_1_1[] = {
	0xFF,
	0xAA, 0x55, 0x25, 0x01,
};
static char etc_cond_set_1_2[] = {
	0xF3,
	0x00, 0x32, 0x00, 0x38,
	0x31, 0x08, 0x11, 0x00
};
static char etc_cond_set_1_3[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x00
};
static char etc_cond_set_1_4[] = {
	0xB0,
	0x04, 0x0A, 0x0E, 0x09, 0x04
};
#ifdef CONFIG_MACH_APEXQ
static char etc_cond_set_1_5[] = {
	0xB1,
	0xCC, 0x04
};
#else
static char etc_cond_set_1_5[] = {
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
static char etc_cond_set_1_6[] = {
	0x36,
	0x11,
};
#else
static char etc_cond_set_1_6[] = {
	0x36,
	0x0A,
};
#endif /* CONFIG_MACH_APEXQ */
static char etc_cond_set_1_7[] = {
	0xB3,
	0x00
};
static char etc_cond_set_1_8[] = {
	0xB6,
	0x03
};
static char etc_cond_set_1_9[] = {
	0xB7,
	0x70,
	0x70
};
static char etc_cond_set_1_10[] = {
	0xB8,
	0x00, 0x06, 0x06, 0x06,
};
static char etc_cond_set_1_11[] = {
	0xBC,
	0x00, 0x00, 0x00,
};
static char etc_cond_set_1_12[] = {
	0xBD,
	0x01, 0x84, 0x06, 0x50, 0x00
};
static char etc_cond_set_1_13[] = {
	0xCC,
	0x03, 0x2A, 0x06
};
static char etc_cond_set_1_14[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x01
};
static char etc_cond_set_1_15[] = {
	0xB0,
	0x05, 0x05, 0x05,
};
static char etc_cond_set_1_16[] = {
	0xB1,
	0x05, 0x05, 0x05,
};
static char etc_cond_set_1_17[] = {
	0xB2,
	0x03, 0x03, 0x03,
};
static char etc_cond_set_1_18[] = {
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
static char etc_cond_set_1_19[] = {
	0xB3,
	0x08, 0x08, 0x08,
};
#else
static char etc_cond_set_1_19[] = {
	0xB3,
	0x0A, 0x0A, 0x0A,
};
#endif
static char etc_cond_set_1_20[] = {
	0xB9,
	0x24, 0x24, 0x24,
};
static char etc_cond_set_1_21[] = {
	0xBF,
	0x01
};
static char etc_cond_set_1_22[] = {
	0xB5,
	0x08, 0x08, 0x08, 0x08,
};
static char etc_cond_set_1_24[] = {
	0xB4,
	0x2D, 0x2D, 0x2D
};
static char etc_cond_set_1_25[] = {
	0xBC,
	0x00, 0x50, 0x00
};
static char etc_cond_set_1_26[] = {
	0xBD,
	0x00, 0x60, 0x00,
};
static char etc_cond_set_1_27[] = {
	0xBE,
	0x00, 0x3D,
};
static char etc_cond_set_1_28[] = {
	0xCE,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* gamma settings */
static char gamma_cond_set_1_0[] = {
	0xD0,
	0x0D, 0x15, 0x08, 0x0C
};

static char gamma_cond_set_1_1r[] = {
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
static char gamma_cond_set_1_1g[] = {
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
static char gamma_cond_set_1_1b[] = {
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
static char gamma_cond_set_1_2r[] = {
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
static char gamma_cond_set_1_2g[] = {
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
static char gamma_cond_set_1_2b[] = {
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
static struct dsi_cmd_desc novatek_panel_ready_to_on_cmds_nt[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_1), etc_cond_set_1_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_2), etc_cond_set_1_2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_3), etc_cond_set_1_3},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_4), etc_cond_set_1_4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_5), etc_cond_set_1_5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_6), etc_cond_set_1_6},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_7), etc_cond_set_1_7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_8), etc_cond_set_1_8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_9), etc_cond_set_1_9},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_10), etc_cond_set_1_10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_11), etc_cond_set_1_11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_12), etc_cond_set_1_12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_13), etc_cond_set_1_13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_14), etc_cond_set_1_14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_15), etc_cond_set_1_15},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_16), etc_cond_set_1_16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_17), etc_cond_set_1_17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_18), etc_cond_set_1_18},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_19), etc_cond_set_1_19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_20), etc_cond_set_1_20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_21), etc_cond_set_1_21},
				{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_22), etc_cond_set_1_22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_24), etc_cond_set_1_24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_25), etc_cond_set_1_25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_26), etc_cond_set_1_26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_27), etc_cond_set_1_27},
			{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(etc_cond_set_1_28), etc_cond_set_1_28},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_0), gamma_cond_set_1_0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_1r), gamma_cond_set_1_1r},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_1g), gamma_cond_set_1_1g},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_1b), gamma_cond_set_1_1b},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_2r), gamma_cond_set_1_2r},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_2g), gamma_cond_set_1_2g},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
			sizeof(gamma_cond_set_1_2b), gamma_cond_set_1_2b},

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

#ifdef CONFIG_MACH_APEXQ
static int lux_tbl[] = {
	0, 8, 13, 15, 20, 30, 40, 50, 60, 70, 80, 95, 100,
	105, 110, 115, 120, 125, 130, 140, 150, 160, 170, 180, 190, 200, 205,
};
#else
static int lux_tbl[] = {
	6, 6, 6, 6, 6, 6, 6, 10, 15, 20,
	25, 30, 35, 40, 45, 50, 55, 60, 65, 70,
	75, 80, 83, 87, 92, 95, 98, 102, 105, 108,
	110, 110, 110, 120, 130, 135, 140, 150, 160, 170,
	180, 190, 200, 210, 220, 225, 230, 235, 240, 240

	};

#endif

static int get_candela_index(int bl_level)
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

	cd = lux_tbl[cd];

	return cd;

}

static struct mipi_panel_data mipi_pd = {
	.panel_name = "NOVATEK_NT35510\n",
	.ready_to_on_hydis = {novatek_panel_ready_to_on_cmds_nt
			, ARRAY_SIZE(novatek_panel_ready_to_on_cmds_nt)},
	.ready_to_off = {novatek_panel_ready_to_off_cmds
			, ARRAY_SIZE(novatek_panel_ready_to_off_cmds)},
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
