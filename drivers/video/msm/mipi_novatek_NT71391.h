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

enum mipi_novatek_cmd_list {
	PANEL_ON,
	PANEL_OFF,
};

struct cmd_set {
	struct dsi_cmd_desc *cmd;
	int size;
};

struct mipi_panel_data {
	const char panel_name[20];
	struct cmd_set on;
	struct cmd_set off;
	struct mipi_novatek_driver_data *msd;
};

struct mipi_novatek_driver_data {
	struct dsi_buf novatek_tx_buf;
	struct dsi_buf novatek_rx_buf;
	struct msm_panel_common_pdata *mipi_novatek_disp_pdata;
	struct mipi_panel_data *mpd;
#if defined(CONFIG_LCD_CLASS_DEVICE)
	struct platform_device *msm_pdev;
#endif

};

int mipi_novatek_disp_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd);

#endif  /* MIPI_NOVATEK_BLUE_H */
