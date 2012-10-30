/*
 * Copyright (c) 2008 QUALCOMM USA, INC.
 * Author: Haibo Jeff Zhong <hzhong@qualcomm.com>
 *
 * All source code in this file is licensed under the following license
 * except where indicated.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can find it at http://www.fsf.org
 *
 */


#ifndef DB8131M_H
#define DB8131M_H


#define	DB8131M_DEBUG
#ifdef DB8131M_DEBUG
#define CAM_DEBUG(fmt, arg...)	\
		do {\
			printk(KERN_DEBUG "[DB8131M] %s : " \
fmt "\n", __func__, ##arg); } \
		while (0)

#define cam_info(fmt, arg...)	\
		do {\
			printk(KERN_INFO "[DB8131M]" fmt "\n", ##arg); } \
		while (0)

#define cam_err(fmt, arg...)	\
		do {\
			printk(KERN_ERR "[DB8131M] %s:%d:" \
fmt "\n", __func__, __LINE__, ##arg); } \
		while (0)

/*#define CAM_I2C_DEBUG*/

#else
#define CAM_DEBUG(fmt, arg...)
#define cam_info(fmt, arg...)
#define cam_err(fmt, arg...)
#endif

#define DB8131M_DELAY		0xE700

/* preview size idx*/
#define PREVIEW_SIZE_VGA   0     /* 640x480*/
#define PREVIEW_SIZE_D1     1    /* 720x480 */
#define PREVIEW_SIZE_WVGA   2    /* 800x480 */
#define PREVIEW_SIZE_XGA    3    /* 1024x768*/
#define PREVIEW_SIZE_HD   4    /* 1280x720*/
#define PREVIEW_SIZE_FHD   5    /* 1920x1080*/
#define PREVIEW_SIZE_MMS 6        /*176x144*/

#define PREVIEW_MODE	0
#define MOVIE_MODE		1

struct db8131m_userset {
	unsigned int focus_mode;
	unsigned int focus_status;
	unsigned int continuous_af;

	unsigned int	metering;
	unsigned int	exposure;
	unsigned int		wb;
	unsigned int		iso;
	int	contrast;
	int	saturation;
	int	sharpness;
	int	brightness;
	int	scene;
	unsigned int zoom;
	unsigned int effect;	/* Color FX (AKA Color tone) */
	unsigned int scenemode;
	unsigned int detectmode;
	unsigned int antishake;
	unsigned int fps;
	unsigned int flash_mode;
	unsigned int flash_state;
	unsigned int stabilize;	/* IS */
	unsigned int strobe;
	unsigned int jpeg_quality;
/*unsigned int preview_size;*/
/*struct m5mo_preview_size preview_size;*/
	unsigned int preview_size_idx;
	unsigned int capture_size;
	unsigned int thumbnail_size;
};

enum db8131m_setting {
	RES_PREVIEW,
	RES_CAPTURE
};
enum mt9p012_reg_update {
	/* Sensor egisters that need to be updated during initialization */
	REG_INIT,
	/* Sensor egisters that needs periodic I2C writes */
	UPDATE_PERIODIC,
	/* All the sensor Registers will be updated */
	UPDATE_ALL,
	/* Not valid update */
	UPDATE_INVALID
};

#endif /* DB8131M_H */

