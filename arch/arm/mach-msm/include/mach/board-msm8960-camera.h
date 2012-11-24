/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
#ifndef _SEC_CAM_PMIC_H
#define _SEC_CAM_PMIC_H

#define	ISX012_DEBUG
#ifdef ISX012_DEBUG
#define CAM_DEBUG(fmt, arg...)	\
		do {\
			printk(KERN_INFO "[ISX012] %s:%d: " fmt "\n", \
				__func__, __LINE__, ##arg); } \
		while (0)

#define cam_info(fmt, arg...)	\
		do {\
			printk(KERN_INFO "[ISX012]" fmt "\n", ##arg); } \
		while (0)

#define cam_err(fmt, arg...)	\
		do {\
			printk(KERN_ERR "[ISX012] %s:%d:" fmt "\n", \
				__func__, __LINE__, ##arg); } \
		while (0)

#else
#define CAM_DEBUG(fmt, arg...)
#define cam_info(fmt, arg...)
#define cam_err(fmt, arg...)
#endif

#endif
