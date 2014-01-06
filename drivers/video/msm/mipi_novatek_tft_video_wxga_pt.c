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
#include "mipi_novatek_NT71391.h"


static struct msm_panel_info pinfo;

static char topc[2] = {0x32, 0x00}; /* Turn On Peripheral Command */
static struct dsi_cmd_desc novatek_video_on_cmds[] = {
	{DTYPE_GEN_WRITE1, 1, 0, 0, 0,
		sizeof(topc), topc},
	};



static struct mipi_panel_data mipi_pd = {
	.panel_name = "NOVATEK_NT71391\n",
	.on = {novatek_video_on_cmds
			, ARRAY_SIZE(novatek_video_on_cmds)},
	};



static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSIPHY_REGULATOR_CTRL */
	.regulator = {0x03, 0x0a, 0x04, 0x00, 0x20}, /* common 8960 */
	/* DSIPHY_CTRL */
	.ctrl = {0x5f, 0x00, 0x00, 0x10}, /* common 8960 */
	/* DSIPHY_STRENGTH_CTRL */
	.strength = {0xff, 0x00, 0x06, 0x00}, /* common 8960 */
	/* DSIPHY_TIMING_CTRL */
	.timing = { 0x66, 0x2B, 0xD, /* panel specific */
	0, /* DSIPHY_TIMING_CTRL_3 = 0 */
	0x37, 0x3D, 0x12, 0x2F, 0x17, 0x03, 0x04},  /* panel specific */

	/* DSIPHY_PLL_CTRL */
	.pll = { 0x00, /* common 8960 */
	/* VCO */

	0x0E, 0x30, 0xC0, /* panel specific */

	0x00, 0x50, 0x48, 0x63,
	0x77, 0x88, 0x99, /* Auto update by dsi-mipi driver */
	0x00, 0x14, 0x03, 0x00, 0x02, /* common 8960 */
	0x00, 0x20, 0x00, 0x01 }, /* common 8960 */
};


static int __init mipi_novatek_tft_video_wxga_pt_init(void)
{
	int ret;

	if (msm_fb_detect_client("mipi_novatek_tft_video_wxga"))
		return 0;

	/* Landscape */
	pinfo.xres = 1280;
	pinfo.yres = 800;
	pinfo.type =  MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1; /* Primary Display */
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24; /* RGB888 = 24 bits-per-pixel */
	pinfo.fb_num = 2; /* using two frame buffers */
	pinfo.height = 108;
	pinfo.width = 172;

	/* bitclk */

	pinfo.clk_rate = 402940800;

	/*
	 * this panel is operated by DE,
	 * vsycn and hsync are ignored
	 */

	pinfo.lcdc.h_front_porch = 40;/* thfp */
	pinfo.lcdc.h_back_porch = 24;	/* thb */
	pinfo.lcdc.h_pulse_width = 16;	/* thpw */

	pinfo.lcdc.v_front_porch = 3;	/* tvfp */
	pinfo.lcdc.v_back_porch = 14;	/* tvb */
	pinfo.lcdc.v_pulse_width = 6;	/* tvpw */

	pinfo.lcdc.border_clr = 0;		/* black */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */

	pinfo.lcdc.hsync_skew = 0;

	/* Backlight levels - controled via PMIC pwm gpio */
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;

	/* mipi - general */
	pinfo.mipi.vc = 0; /* virtual channel */
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.tx_eot_append = true;
	pinfo.mipi.t_clk_post = 4;		/* Calculated */
	pinfo.mipi.t_clk_pre = 16;		/* Calculated */

	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;

	/* Four lanes are recomended for 1366x768 at 60 frames per second */
	pinfo.mipi.frame_rate = 60; /* 60 frames per second */
	pinfo.mipi.data_lane0 = true;
	pinfo.mipi.data_lane1 = true;
	pinfo.mipi.data_lane2 = true;
	pinfo.mipi.data_lane3 = true;

	pinfo.mipi.mode = DSI_VIDEO_MODE;

	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;

	/* mipi - video mode */
	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo.mipi.pulse_mode_hsa_he = false; /* sync mode */

	pinfo.mipi.hfp_power_stop = false;
	pinfo.mipi.hbp_power_stop = false;
	pinfo.mipi.hsa_power_stop = false;
	pinfo.mipi.eof_bllp_power_stop = false;
	pinfo.mipi.bllp_power_stop = true;
	pinfo.mipi.esc_byte_ratio = 4;
	pinfo.mipi.no_max_pkt_size = 1;
	pinfo.mipi.force_clk_lane_hs = 1;


	ret = mipi_novatek_disp_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WXGA, &mipi_pd);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_novatek_tft_video_wxga_pt_init);

