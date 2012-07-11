/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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

#ifndef MIPI_NOVATEK_BLUE_H
#define MIPI_NOVATEK_BLUE_H

#define NOVATEK_TWO_LANE
#include "mipi_dsi.h"

#if defined(CONFIG_MACH_APEXQ) || defined(CONFIG_MACH_GOGH)
#define MAX_BL_LEVEL 225
#define MAX_GAMMA_VALUE 25
#define MIN_BL_LEVEL 30
#define INDEX_OFFSET 1
#else
#define MAX_BL_LEVEL 250
#define MAX_GAMMA_VALUE 49
#define MIN_BL_LEVEL 0
#define INDEX_OFFSET 0
#endif
#define JASPER_MANUFACTURE_ID 0x556CC0

enum mipi_novatek_cmd_list {
	PANEL_READY_TO_ON,
	PANEL_READY_TO_OFF,
	PANEL_ON,
	PANEL_OFF,
	PANEL_LATE_ON,
	PANEL_EARLY_OFF,

};

struct cmd_set {
	struct dsi_cmd_desc *cmd;
	int size;
};

struct mipi_panel_data {
	const char panel_name[20];
	struct cmd_set ready_to_on_hydis;
	struct cmd_set ready_to_on_boe;
	struct cmd_set ready_to_off;
	struct cmd_set on;
	struct cmd_set off;
	struct cmd_set late_on;
	struct cmd_set early_off;
	struct mipi_novatek_driver_data *msd;
	int (*set_brightness_level)(int bl_level);
	unsigned int manufacture_id;


};

struct display_status {
	unsigned char acl_on;
	unsigned char gamma_mode; /* 1: 1.9 gamma, 0: 2.2 gamma */
	unsigned char is_smart_dim_loaded;
	unsigned char is_elvss_loaded;
};


struct mipi_novatek_driver_data {
	struct dsi_buf novatek_tx_buf;
	struct dsi_buf novatek_rx_buf;
	struct msm_panel_common_pdata *mipi_novatek_disp_pdata;
	struct mipi_panel_data *mpd;
	struct display_status dstat;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	struct platform_device *msm_pdev;
#endif
#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
	boolean esd_refresh;
#endif

};
#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
extern void set_esd_refresh(boolean stat);
extern void set_esd_enable();
extern void set_esd_disable();
#endif
#if defined(CONFIG_FB_MSM_MIPI_NOVATEK_CMD_WVGA_PT) || \
	defined(CONFIG_FB_MSM_MIPI_NOVATEK_BOE_CMD_WVGA_PT)
int mipi_novatek_disp_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd);
#else
int mipi_novatek_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);
#endif
#endif  /* MIPI_NOVATEK_BLUE_H */
