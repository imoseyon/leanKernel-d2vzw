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

#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <media/msm_camera.h>
#include <media/v4l2-subdev.h>
#include <mach/gpio.h>
#include <mach/camera.h>

#include <asm/mach-types.h>
#include <mach/vreg.h>
#include <linux/io.h>

#include "msm.h"
#include "isx012.h"
#include "isx012_regs.h"
#include "msm.h"
#include "msm_ispif.h"


#undef CONFIG_LOAD_FILE

#ifdef CONFIG_LOAD_FILE

#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static char *isx012_regs_table;
static int isx012_regs_table_size;
static int isx012_write_regs_from_sd(char *name);
#endif

#define ISX012_WRITE_LIST(A)	\
	isx012_i2c_write_list(A, (sizeof(A) / sizeof(A[0])), #A);

#define MOVIEMODE_FLASH	17
#define FLASHMODE_FLASH	18

#define ERROR 1

#define IN_AUTO_MODE 1
#define IN_MACRO_MODE 2

struct isx012_work {
	struct work_struct work;
};

static struct  isx012_work *isx012_sensorw;
static struct  i2c_client *isx012_client;

struct isx012_ctrl {
	const struct msm_camera_sensor_info *sensordata;
	struct isx012_userset settings;

	struct v4l2_subdev *sensor_dev;

	int op_mode;
	int dtp_mode;
	int app_mode;
	int cam_mode;
	int vtcall_mode;
	int started;
	int isCapture;
	int flash_mode;
	int lowLight;
	int dtpTest;
};

static unsigned int config_csi2;
static struct isx012_ctrl *isx012_ctrl;

struct isx012_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};
static int32_t isx012_sensor_setting(int update_type, int rt);
static DECLARE_WAIT_QUEUE_HEAD(isx012_wait_queue);

/**
 * isx012_i2c_read_multi: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 * @r_data: buffer where data is read
 * @r_len: number of bytes to read
 *
 * Returns 0 on success, <0 on error
 */

static int isx012_i2c_read_multi(unsigned short subaddr, unsigned long *data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {isx012_client->addr, 0, 2, buf};

	int err = 0;

	if (!isx012_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xff;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	/*
	 * Data comes in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */
	*data = *(unsigned long *)(&buf);

	return err;
}


/**
 * isx012_i2c_read: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @data: data to be read
 *
 * Returns 0 on success, <0 on error
 */
static int isx012_i2c_read(unsigned short subaddr, unsigned short *data)
{
	unsigned char buf[2];
	struct i2c_msg msg = {isx012_client->addr, 0, 2, buf};

	int err = 0;

	if (!isx012_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xff;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	/*
	 * Data comes in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */
	*data = *(unsigned short *)(&buf);

	return err;
}

/**
 * isx012_i2c_write_multi: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static int isx012_i2c_write_multi(unsigned short addr,
unsigned int w_data, unsigned int w_len)
{
	unsigned char buf[w_len+2];
	struct i2c_msg msg = {isx012_client->addr, 0, w_len+2, buf};

	int retry_count = 5;
	int err = 0;

	if (!isx012_client->adapter)
		return -EIO;

	buf[0] = addr >> 8;
	buf[1] = addr & 0xff;

	/*
	 * Data should be written in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */
	if (w_len == 1)
		buf[2] = (unsigned char)w_data;
	else if (w_len == 2)
		*((unsigned short *)&buf[2]) = (unsigned short)w_data;
	else
		*((unsigned int *)&buf[2]) = w_data;

	while (retry_count--) {
		err  = i2c_transfer(isx012_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
	}

	return (err == 1) ? 0 : -EIO;
}

static int isx012_i2c_write_list(struct isx012_short_t regs[], int size,
					char *name)
{
#ifdef CONFIG_LOAD_FILE
	isx012_write_regs_from_sd(name);
#else
	int err = 0;
	int i = 0;

/*	printk(KERN_DEBUG "[isx012] %s, %d\n", __func__, __LINE__);*/

	if (!isx012_client->adapter) {
		printk(KERN_ERR "%s: %d can't search i2c client adapter\n",
			__func__, __LINE__);
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		if (regs[i].subaddr == 0xFFFF) {
			msleep(regs[i].value);
			printk(KERN_DEBUG "delay 0x%04x, value 0x%04x\n",
				regs[i].subaddr, regs[i].value);
		} else {
				err = isx012_i2c_write_multi(regs[i].subaddr,
				regs[i].value, regs[i].len);

			if (unlikely(err < 0)) {
				printk(KERN_ERR "%s: register set failed\n",
					__func__);
				return -EIO;
			}
		}
	}
#endif

	return 0;
}

#ifdef CONFIG_LOAD_FILE
void isx012_regs_table_init(void)
{
	struct file *filp;
	char *dp;
	long l;
	loff_t pos;
	int ret;
	mm_segment_t fs = get_fs();

	printk(KERN_DEBUG "%s %d\n", __func__, __LINE__);

	set_fs(get_ds());

	filp = filp_open("/mnt/sdcard/isx012_regs.h", O_RDONLY, 0);

	if (IS_ERR_OR_NULL(filp)) {
		printk(KERN_DEBUG "file open error\n");
		return PTR_ERR(filp);
	}

	l = filp->f_path.dentry->d_inode->i_size;
	printk(KERN_DEBUG "l = %ld\n", l);
	dp = vmalloc(l);
	if (dp == NULL) {
		printk(KERN_DEBUG "Out of Memory\n");
		filp_close(filp, current->files);
	}

	pos = 0;
	memset(dp, 0, l);
	ret = vfs_read(filp, (char __user *)dp, l, &pos);

	if (ret != l) {
		printk(KERN_DEBUG "Failed to read file ret = %d\n", ret);
		/*kfree(dp);*/
		vfree(dp);
		filp_close(filp, current->files);
		return -EINVAL;
	}

	filp_close(filp, current->files);

	set_fs(fs);

	isx012_regs_table = dp;

	isx012_regs_table_size = l;

	*((isx012_regs_table + isx012_regs_table_size) - 1) = '\0';

	printk(KERN_DEBUG "isx012_reg_table_init\n");
	return 0;
}


void isx012_regs_table_exit(void)
{
	printk(KERN_DEBUG "%s %d\n", __func__, __LINE__);

	if (isx012_regs_table) {
		vfree(isx012_regs_table);
		isx012_regs_table = NULL;
	}
}

static int isx012_write_regs_from_sd(char *name)
{
	char *start, *end, *reg, *size;
	unsigned short addr;
	unsigned int len, value;
	char reg_buf[7], data_buf1[5], data_buf2[7], len_buf[5];

	*(reg_buf + 6) = '\0';
	*(data_buf1 + 4) = '\0';
	*(data_buf2 + 6) = '\0';
	*(len_buf + 4) = '\0';

	printk(KERN_DEBUG "isx012_regs_table_write start!\n");
	printk(KERN_DEBUG "E string = %s\n", name);

	start = strstr(isx012_regs_table, name);
	end = strstr(start, "};");

	while (1) {
		/* Find Address */
		reg = strstr(start, "{0x");

		if ((reg == NULL) || (reg > end))
			break;

		/* Write Value to Address */
		if (reg != NULL) {
			memcpy(reg_buf, (reg + 1), 6);
			memcpy(data_buf2, (reg + 9), 6);
			size = strstr(data_buf2, ",");
			if (size) { /* 1 byte write */
				memcpy(data_buf1, (reg + 9), 4);
				memcpy(len_buf, (reg + 15), 4);
				kstrtoint(reg_buf, 16, &addr);
				kstrtoint(data_buf1, 16, &value);
				kstrtoint(len_buf, 16, &len);
				if (reg)
					start = (reg + 22);
			} else {/* 2 byte write */
				memcpy(len_buf, (reg + 17), 4);
				kstrtoint(reg_buf, 16, &addr);
				kstrtoint(data_buf2, 16, &value);
				kstrtoint(len_buf, 16, &len);
				if (reg)
					start = (reg + 24);
			}
			size = NULL;
			printk(KERN_DEBUG "delay 0x%04x, value 0x%04x, , len 0x%02x\n",
				addr, value, len);
			if (addr == 0xFFFF)
				msleep(value);
			else
				isx012_i2c_write_multi(addr, value, len);
		}
	}

	printk(KERN_DEBUG "isx005_regs_table_write end!\n");

	return 0;
}
#endif


static int isx012_get_LowLightCondition()
{
	int err = -1;
	short unsigned int r_data2[2] = {0, 0};
	unsigned char l_data[2] = {0, 0}, h_data[2] = {0, 0};
	unsigned int LowLight_value = 0;
	unsigned int ldata_temp = 0, hdata_temp = 0;

	unsigned short read_val = 0;
	int read_val2;
	err = isx012_i2c_read(0x01A5, &read_val);
	LowLight_value = 0x00FF & read_val;

	if (err < 0)
		pr_err("%s: isx012_get_LowLightCondition() returned error, %d\n"
			, __func__, err);

	CAM_DEBUG("auto iso %d, read_val = 0x%x LowLight_value = 0x%x\n",
		isx012_ctrl->settings.iso, read_val, LowLight_value);
	if (isx012_ctrl->settings.iso == CAMERA_ISO_MODE_AUTO) { /*auto iso*/
		if (LowLight_value >= LOWLIGHT_DEFAULT)
			isx012_ctrl->lowLight = 1;
		else
			isx012_ctrl->lowLight = 0;
	} else {	/*manual iso*/
		err = isx012_i2c_read_multi(0x019C, l_data); /*SHT_TIME_OUT_L*/
		ldata_temp = (l_data[1] << 8 | l_data[0]);

		err = isx012_i2c_read(0x019E, h_data); /*SHT_TIME_OUT_H*/
		hdata_temp = (h_data[1] << 8 | h_data[0]);
		CAM_DEBUG("ldata_temp = 0x%x, hdata_temp = 0x%x",
			ldata_temp, hdata_temp);
		LowLight_value = (h_data[1] << 24 | h_data[0] << 16 |
					 l_data[1] << 8 | l_data[0]);

		switch (isx012_ctrl->settings.iso) {
		case CAMERA_ISO_MODE_100:
			if (LowLight_value >= LOWLIGHT_ISO100)
				isx012_ctrl->lowLight = 1;
			else
				isx012_ctrl->lowLight = 0;
			break;

		case CAMERA_ISO_MODE_200:
			if (LowLight_value >= LOWLIGHT_ISO200)
				isx012_ctrl->lowLight = 1;
			else
				isx012_ctrl->lowLight = 0;
			break;

		case CAMERA_ISO_MODE_400:
			if (LowLight_value >= LOWLIGHT_ISO400)
				isx012_ctrl->lowLight = 1;
			else
				isx012_ctrl->lowLight = 0;
			break;

		default:
			printk(KERN_DEBUG "[ISX012][%s:%d] invalid iso[%d]\n",
				__func__, __LINE__, isx012_ctrl->settings.iso);
			break;

		}
	}
	CAM_DEBUG("lowLight : %d\n", isx012_ctrl->lowLight);

	return err;
}


void isx012_mode_transtion_OM(void)
{
	int count = 0;
	int status = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	for (count = 0; count < 100 ; count++) {
		isx012_i2c_read(0x000E, (unsigned short *)&status);
		printk(KERN_DEBUG "[isx012] 0x000E (1) read : %x\n", status);

		if ((status & 0x1) == 0x1)
			break;

		usleep(1*1000);
	}
		isx012_i2c_write_multi(0x0012, 0x01, 0x01);

	for (count = 0; count < 100 ; count++) {
			isx012_i2c_read(0x000E, (unsigned short *)&status);

		printk(KERN_DEBUG "[isx012] 0x000E (2) read : %x\n", status);

		if ((status & 0x1) == 0x0)
			break;

		usleep(1*1000);
	}
}


void isx012_mode_transtion_CM(void)
{
	int count = 0;
	int status = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	for (count = 0; count < 100 ; count++) {
		isx012_i2c_read(0x000E, (unsigned short *)&status);

		printk(KERN_DEBUG "[isx012] 0x000E (1) read : %x\n", status);

		if ((status & 0x2) == 0x2)
			break;

		usleep(1*1000);
	}
		isx012_i2c_write_multi(0x0012, 0x02, 0x01);

	for (count = 0; count < 100 ; count++) {
			isx012_i2c_read(0x000E, (unsigned short *)&status);

		printk(KERN_DEBUG "[isx012] 0x000E (2) read : %x\n", status);

		if ((status & 0x2) == 0x0)
			break;

		usleep(1*1000);
	}
}

void isx012_Sensor_Calibration(void)
{
	int count = 0;
	int status = 0;
	int temp = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

/* Read OTP1 */
	isx012_i2c_read(0x004F, (unsigned short *)&status);
	printk(KERN_DEBUG "0x004F read : %x\n", status);

	if ((status & 0x1) == 0x1) {
		/* Read ShadingTable */
		isx012_i2c_read(0x005C, (unsigned short *)&status);
		temp = (status&0x03C0)>>6;
		printk(KERN_DEBUG "Read ShadingTable read : %x\n", temp);

		/* Write Shading Table */
		if (temp == 0x0) {
			printk(KERN_DEBUG "ISX012_Shading_0\n");
			ISX012_WRITE_LIST(ISX012_Shading_0);
		} else if (temp == 0x1) {
			printk(KERN_DEBUG "ISX012_Shading_1\n");
			ISX012_WRITE_LIST(ISX012_Shading_1);
		} else if (temp == 0x2) {
			printk(KERN_DEBUG "ISX012_Shading_2\n");
			ISX012_WRITE_LIST(ISX012_Shading_2);
		}

		/* Write NorR */
		isx012_i2c_read(0x0054, (unsigned short *)&status);
		temp = status&0x3FFF;
		printk(KERN_DEBUG "NorR read : %x\n", temp);
		isx012_i2c_write_multi(0x6804, temp, 0x02);

		/* Write NorB */
		isx012_i2c_read(0x0056, (unsigned short *)&status);
		temp = status&0x3FFF;
		printk(KERN_DEBUG "NorB read : %x\n", temp);
		isx012_i2c_write_multi(0x6806, temp, 0x02);

		/* Write PreR */
		isx012_i2c_read(0x005A, (unsigned short *)&status);
		temp = (status&0x0FFC)>>2;
		printk(KERN_DEBUG "PreR read : %x\n", temp);
		isx012_i2c_write_multi(0x6808, temp, 0x02);

		/* Write PreB */
		isx012_i2c_read(0x005B, (unsigned short *)&status);
		temp = (status&0x3FF0)>>4;
		printk(KERN_DEBUG "PreB read : %x\n", temp);
		isx012_i2c_write_multi(0x680A, temp, 0x02);
	} else {
		/* Read OTP0 */
		isx012_i2c_read(0x0040, (unsigned short *)&status);
		printk(KERN_DEBUG "0x0040 read : %x\n", status);

		if ((status & 0x1) == 0x1) {
			/* Read ShadingTable */
			isx012_i2c_read(0x004D, (unsigned short *)&status);
			temp = (status&0x03C0)>>6;
			printk(KERN_DEBUG "Read ShadingTable read : %x\n",
				temp);

			/* Write Shading Table */
			if (temp == 0x0) {
				printk(KERN_DEBUG "ISX012_Shading_0\n");
				ISX012_WRITE_LIST(ISX012_Shading_0);
			} else if (temp == 0x1) {
				printk(KERN_DEBUG "ISX012_Shading_1\n");
				ISX012_WRITE_LIST(ISX012_Shading_1);
			} else if (temp == 0x2) {
				printk(KERN_DEBUG "ISX012_Shading_2\n");
				ISX012_WRITE_LIST(ISX012_Shading_2);
			}

			/* Write NorR */
			isx012_i2c_read(0x0045, (unsigned short *)&status);
			temp = status&0x3FFF;
			printk(KERN_DEBUG "NorR read : %x\n", temp);
			isx012_i2c_write_multi(0x6804, temp, 0x02);

			/* Write NorB */
			isx012_i2c_read(0x0047, (unsigned short *)&status);
			temp = status&0x3FFF;
			printk(KERN_DEBUG "NorB read : %x\n", temp);
			isx012_i2c_write_multi(0x6806, temp, 0x02);

			/* Write PreR */
			isx012_i2c_read(0x004B, (unsigned short *)&status);
			temp = (status&0x0FFC)>>2;
			printk(KERN_DEBUG "PreR read : %x\n", temp);
			isx012_i2c_write_multi(0x6808, temp, 0x02);

			/* Write PreB */
			isx012_i2c_read(0x004C, (unsigned short *)&status);
			temp = (status&0x3FF0)>>4;
			printk(KERN_DEBUG "PreB read : %x\n", temp);
			isx012_i2c_write_multi(0x680A, temp, 0x02);
		} else
			ISX012_WRITE_LIST(ISX012_Shading_Nocal);
	}
}

void isx012_jpeg_update(void)
{
	int count = 0;
	int status = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	for (count = 0; count < 100 ; count++) {
		isx012_i2c_read(0x000E, (unsigned short *)&status);
		printk(KERN_DEBUG "[isx012] 0x000E (1) %d read : %x\n",
			count, status);

		if ((status & 0x4) == 0x4)
			break;

		usleep(1*1000);
	}
	isx012_i2c_write_multi(0x0012, 0x04, 0x01);
	for (count = 0; count < 100 ; count++) {
		isx012_i2c_read(0x000E, (unsigned short *)&status);
		printk(KERN_DEBUG "[isx012] 0x000E (2) read : %x\n", status);

		if ((status & 0x4) == 0x0)
			break;

		usleep(1*1000);
	}
}

static int isx012_sensor_af_status(void)
{
	int ret = 0;
	int status = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	isx012_i2c_read(0x8B8A, (unsigned short *)&status);
	if ((status & 0x8) == 0x8) {
		ret = 1;
		printk(KERN_DEBUG "[isx012] success\n");
	}

	return ret;
}

static int isx012_sensor_af_result(void)
{
	int ret = 0;
	int status = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	isx012_i2c_read(0x8B8B, (unsigned short *)&status);
	if ((status & 0x1) == 0x1) {
		printk(KERN_DEBUG "[isx012] AF success\n");
		ret = 1;
	} else if ((status & 0x1) == 0x0) {
		printk(KERN_DEBUG "[isx012] AF fail\n");
		ret = 2;
	}
	return ret;
}

static void isx012_set_flash(int lux_val)
{
	int i = 0;
	printk(KERN_DEBUG "%s, flash set is %d, %d\n",
		__func__, lux_val, isx012_ctrl->flash_mode);

	if (isx012_ctrl->flash_mode == 0)
		return;

	if (lux_val == MOVIEMODE_FLASH) {
		printk(KERN_DEBUG "[ASWOOGI] FLASH MOVIE MODE\n");
		gpio_set_value_cansleep(
			isx012_ctrl->sensordata
			->sensor_platform_info->flash_en, 0);

		for (i = 5; i > 1; i--) {
			gpio_set_value_cansleep(
				isx012_ctrl->sensordata
				->sensor_platform_info->flash_set, 1);
			udelay(1);
			gpio_set_value_cansleep(
				isx012_ctrl->sensordata
				->sensor_platform_info->flash_set, 0);
			udelay(1);
		}
		gpio_set_value_cansleep(
			isx012_ctrl->sensordata
			->sensor_platform_info->flash_set, 1);
		usleep(2*1000);
	} else if (lux_val == FLASHMODE_FLASH) {
		printk(KERN_DEBUG "[ASWOOGI] FLASH ON : %d\n", lux_val);
		gpio_set_value_cansleep(
			isx012_ctrl->sensordata
			->sensor_platform_info->flash_en, 1);
		gpio_set_value_cansleep(
			isx012_ctrl->sensordata
			->sensor_platform_info->flash_set, 0);
	} else {
		printk(KERN_DEBUG "[ASWOOGI] FLASH OFF\n");
		gpio_set_value_cansleep(
			isx012_ctrl->sensordata
			->sensor_platform_info->flash_en, 0);
		gpio_set_value_cansleep(
			isx012_ctrl->sensordata
			->sensor_platform_info->flash_set, 0);
	}
}

void isx012_set_preview(void)
{
	/*printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);*/
	CAM_DEBUG("cam_mode = %d", isx012_ctrl->cam_mode);

	if (isx012_ctrl->cam_mode == MOVIE_MODE) {
		ISX012_WRITE_LIST(ISX012_Camcorder_Mode);
	} else {
		ISX012_WRITE_LIST(ISX012_Preview_Mode);
	}
	isx012_mode_transtion_CM();
}

void isx012_set_capture(void)
{
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);
	ISX012_WRITE_LIST(ISX012_Capture_SizeSetting);

	isx012_get_LowLightCondition();

	if ((isx012_ctrl->flash_mode == CAMERA_FLASH_AUTO &&
		isx012_ctrl->lowLight) ||
		isx012_ctrl->flash_mode == CAMERA_FLASH_ON) {
			isx012_set_flash(FLASHMODE_FLASH);
	}

	ISX012_WRITE_LIST(ISX012_Capture_Mode);

	isx012_mode_transtion_CM();

/*frame capture*/
	isx012_ctrl->isCapture = 1;
}

static int32_t isx012_sensor_setting(int update_type, int rt)
{
	int32_t rc = 0;
	struct msm_camera_csid_params isx012_csid_params;
	struct msm_camera_csiphy_params isx012_csiphy_params;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	switch (update_type) {
	case REG_INIT:
		break;

	case UPDATE_PERIODIC:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			printk(KERN_DEBUG "[isx012] UPDATE_PERIODIC\n");

			v4l2_subdev_notify(isx012_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX0, ISPIF_OFF_IMMEDIATELY));

			/* stop streaming */
			printk(KERN_DEBUG "[isx012] change standby state\n");
			isx012_i2c_write_multi(0x0005, 0x01, 0x01);

			msleep(100);

			if (config_csi2 == 0) {
				struct msm_camera_csid_vc_cfg isx012_vccfg[] = {
					{0, 0x1E, CSI_DECODE_8BIT},
					{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
				};
				isx012_csid_params.lane_cnt = 2;
				isx012_csid_params.lane_assign = 0xe4;
				isx012_csid_params.lut_params.num_cid =
					ARRAY_SIZE(isx012_vccfg);
				isx012_csid_params.lut_params.vc_cfg =
					&isx012_vccfg[0];
				isx012_csiphy_params.lane_cnt = 2;
				isx012_csiphy_params.settle_cnt = 0x5;
				v4l2_subdev_notify(isx012_ctrl->sensor_dev,
						NOTIFY_CSID_CFG,
						&isx012_csid_params);
				v4l2_subdev_notify(isx012_ctrl->sensor_dev,
						NOTIFY_CID_CHANGE, NULL);
				mb();
				v4l2_subdev_notify(isx012_ctrl->sensor_dev,
						NOTIFY_CSIPHY_CFG,
						&isx012_csiphy_params);
				mb();
				/*isx012_delay_msecs_stdby*/
				msleep(100);
				config_csi2 = 1;
			}

			if (rc < 0)
				return rc;

			v4l2_subdev_notify(isx012_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX0, ISPIF_ON_FRAME_BOUNDARY));

			/*start stream*/
			printk(KERN_DEBUG "[isx012] change stream state\n");
			isx012_i2c_write_multi(0x0005, 0x00, 0x01);

			msleep(100);

		}
		break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int32_t isx012_video_config(int mode)
{
	int32_t	rc = 0;
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);
	if (isx012_sensor_setting(UPDATE_PERIODIC, RES_PREVIEW) < 0)
		return rc;
	return rc;
}

static long isx012_set_sensor_mode(int mode)
{
	printk(KERN_DEBUG "[isx012] %s : %d /%d\n", __func__, mode, __LINE__);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	case SENSOR_VIDEO_MODE:
		isx012_set_flash(0);

		if (isx012_ctrl->isCapture == 1) {
			isx012_ctrl->isCapture = 0;
			isx012_set_preview();
		}

		isx012_video_config(mode);
		break;

	case SENSOR_SNAPSHOT_MODE:
	case SENSOR_RAW_SNAPSHOT_MODE:
		isx012_set_flash(FLASHMODE_FLASH);
		isx012_set_capture();
		break;

	default:
		return 0;
	}
	return 0;
}

static int isx012_set_effect(int effect)
{
	switch (effect) {
	case CAMERA_EFFECT_OFF:
		ISX012_WRITE_LIST(ISX012_Effect_Normal);
		break;

	case CAMERA_EFFECT_MONO:
		ISX012_WRITE_LIST(ISX012_Effect_Monotone);
		break;

	case CAMERA_EFFECT_NEGATIVE:
		ISX012_WRITE_LIST(ISX012_Effect_Negative);
		break;

	case CAMERA_EFFECT_SEPIA:
		ISX012_WRITE_LIST(ISX012_Effect_Sepia);
		break;

	case CAMERA_EFFECT_WHITEBOARD:
		ISX012_WRITE_LIST(ISX012_Effect_Pastel);
		break;

	default:
		printk(KERN_DEBUG "[isx012] default effect\n");
		ISX012_WRITE_LIST(ISX012_Effect_Normal);
		return 0;
	}

	return 0;
}

static int isx012_set_whitebalance(int wb)
{
	switch (wb) {
	case CAMERA_WHITE_BALANCE_AUTO:
		ISX012_WRITE_LIST(ISX012_WB_Auto);
		break;

	case CAMERA_WHITE_BALANCE_INCANDESCENT:
		ISX012_WRITE_LIST(ISX012_WB_Incandescent);
		break;

	case CAMERA_WHITE_BALANCE_FLUORESCENT:
		ISX012_WRITE_LIST(ISX012_WB_Fluorescent);
		break;

	case CAMERA_WHITE_BALANCE_DAYLIGHT:
		ISX012_WRITE_LIST(ISX012_WB_Daylight);
		break;

	case CAMERA_WHITE_BALANCE_CLOUDY_DAYLIGHT:
		ISX012_WRITE_LIST(ISX012_WB_Cloudy);
		break;

	default:
		printk(KERN_DEBUG "[isx012] unexpected WB mode %s/%d\n",
			__func__, __LINE__);
		return 0;
	}
	return 0;
}
static void isx012_check_dataline(int val)
{
	if (val) {
		printk(KERN_DEBUG "DTP ON\n");
		ISX012_WRITE_LIST(ISX012_DTP_Init);
		isx012_ctrl->dtpTest = 1;

	} else {
		printk(KERN_DEBUG "DTP OFF\n");
		ISX012_WRITE_LIST(ISX012_DTP_Stop);
		isx012_ctrl->dtpTest = 0;
	}
}

static void isx012_set_ev(int ev)
{
	printk(KERN_DEBUG "[isx012] %s : %d\n", __func__, ev);

	switch (ev) {
	case CAMERA_EV_M2:
		ISX012_WRITE_LIST(ISX012_ExpSetting_M4Step);
		break;

	case CAMERA_EV_M1:
		ISX012_WRITE_LIST(ISX012_ExpSetting_M2Step);
		break;

	case CAMERA_EV_DEFAULT:
		ISX012_WRITE_LIST(ISX012_ExpSetting_Default);
		break;

	case CAMERA_EV_P1:
		ISX012_WRITE_LIST(ISX012_ExpSetting_P2Step);
		break;

	case CAMERA_EV_P2:
		ISX012_WRITE_LIST(ISX012_ExpSetting_P4Step);
		break;

	default:
		printk(KERN_DEBUG "[isx012] unexpected ev mode %s/%d\n",
			__func__, __LINE__);
		break;
	}
}

static void isx012_set_scene_mode(int mode)
{
	printk(KERN_DEBUG "[isx012] %s : %d\n", __func__, mode);

	switch (mode) {
	case CAMERA_SCENE_AUTO:
		ISX012_WRITE_LIST(ISX012_Scene_Auto);
		break;

	case CAMERA_SCENE_LANDSCAPE:
		ISX012_WRITE_LIST(ISX012_Scene_Landscape);
		break;

	case CAMERA_SCENE_DAWN:
		ISX012_WRITE_LIST(ISX012_Scene_Dawn);
		break;

	case CAMERA_SCENE_BEACH:
		ISX012_WRITE_LIST(ISX012_Scene_Beach_Snow);
		break;

	case CAMERA_SCENE_SUNSET:
		ISX012_WRITE_LIST(ISX012_Scene_Sunset);
		break;

	case CAMERA_SCENE_NIGHT:
		ISX012_WRITE_LIST(ISX012_Scene_Nightmode);
		break;

	case CAMERA_SCENE_PORTRAIT:
		ISX012_WRITE_LIST(ISX012_Scene_Portrait);
		break;

	case CAMERA_SCENE_AGAINST_LIGHT:
		ISX012_WRITE_LIST(ISX012_Scene_Backlight);
		break;

	case CAMERA_SCENE_SPORT:
		ISX012_WRITE_LIST(ISX012_Scene_Sports);
		break;

	case CAMERA_SCENE_FALL:
		ISX012_WRITE_LIST(ISX012_Scene_Fallcolor);
		break;

	case CAMERA_SCENE_TEXT:
		ISX012_WRITE_LIST(ISX012_Scene_Document);
		break;

	case CAMERA_SCENE_CANDLE:
		ISX012_WRITE_LIST(ISX012_Scene_CandleLight);
		break;

	case CAMERA_SCENE_FIRE:
		ISX012_WRITE_LIST(ISX012_Scene_Firework);
		break;

	case CAMERA_SCENE_PARTY:
		ISX012_WRITE_LIST(ISX012_Scene_Indoor);
		break;

	default:
		printk(KERN_DEBUG "[isx012] unexpected scene mode %s/%d\n",
			__func__, __LINE__);
		break;
	}
}

static void isx012_set_iso(int iso)
{
	printk(KERN_DEBUG "[isx012] %s : %d\n", __func__, iso);

	switch (iso) {
	case CAMERA_ISO_MODE_AUTO:
		ISX012_WRITE_LIST(ISX012_ISO_AUTO);
		break;

	case CAMERA_ISO_MODE_100:
		ISX012_WRITE_LIST(ISX012_ISO_100);
		break;

	case CAMERA_ISO_MODE_200:
		ISX012_WRITE_LIST(ISX012_ISO_200);
		break;

	case CAMERA_ISO_MODE_400:
		ISX012_WRITE_LIST(ISX012_ISO_400);
		break;

	case CAMERA_ISO_MODE_800:
		ISX012_WRITE_LIST(ISX012_ISO_800);
		break;

	default:
		printk(KERN_DEBUG "[isx012] unexpected iso mode %s/%d\n",
			__func__, __LINE__);
		break;
	}

	isx012_ctrl->settings.iso = iso;
}

static void isx012_set_metering(int mode)
{
	printk(KERN_DEBUG "[isx012] %s : %d\n", __func__, mode);

	switch (mode) {
	case CAMERA_CENTER_WEIGHT:
		ISX012_WRITE_LIST(ISX012_Potometry_CenterWeight);
		break;

	case CAMERA_AVERAGE:
		ISX012_WRITE_LIST(ISX012_Potometry_Average);
		break;

	case CAMERA_SPOT:
		ISX012_WRITE_LIST(ISX012_Potometry_Spot);
		break;

	default:
		printk(KERN_DEBUG "[isx012] unexpected metering mode %s/%d\n",
			__func__, __LINE__);
		break;
	}
}


static int isx012_set_movie_mode(int mode)
{
	CAM_DEBUG("E");

	if (mode == MOVIE_MODE) {
		printk(KERN_DEBUG "isx012_set_movie_mode Camcorder_Mode_ON\n");
/*		ISX012_WRITE_LIST(ISX012_Camcorder_Mode_ON);
		isx012_ctrl->status.camera_status = 1;*/
	} else {
		/*isx012_ctrl->status.camera_status = 0;*/
	}
/*
	if ((mode != SENSOR_CAMERA) && (mode != SENSOR_MOVIE)) {
		return -EINVAL;
	}
*/
	return 0;
}


static void isx012_set_focus(int mode)
{
	printk(KERN_DEBUG "[isx012] %s : %d\n", __func__, mode);

	switch (mode) {
	case CAMERA_AF_AUTO:
		ISX012_WRITE_LIST(ISX012_AF_Macro_OFF);
		if (isx012_ctrl->settings.focus_mode == IN_MACRO_MODE)
			ISX012_WRITE_LIST(ISX012_AF_ReStart);
		isx012_ctrl->settings.focus_mode = IN_AUTO_MODE;
		break;

	case CAMERA_AF_MACRO:
		ISX012_WRITE_LIST(ISX012_AF_Macro_ON);
		if (isx012_ctrl->settings.focus_mode == IN_AUTO_MODE)
			ISX012_WRITE_LIST(ISX012_AF_ReStart);
		isx012_ctrl->settings.focus_mode = IN_MACRO_MODE;
		break;

	default:
		printk(KERN_DEBUG "[isx012] unexpected focus mode %s/%d\n",
			__func__, __LINE__);
		break;
	}
}

static void isx012_set_preview_size(int32_t width)
{
	printk(KERN_DEBUG "width %d\n", width);

	if (width == 1920) {
		printk(KERN_DEBUG "1920*1080\n");
		ISX012_WRITE_LIST(ISX012_1920_Preview_SizeSetting);
	} else if (width == 1280) {
		printk(KERN_DEBUG "1280*720\n");
		ISX012_WRITE_LIST(ISX012_1280_Preview_SizeSetting);
	} else if (width == 800) {
		printk(KERN_DEBUG "800*480\n");
		ISX012_WRITE_LIST(ISX012_800_Preview_SizeSetting);
	} else if (width == 720) {
		printk(KERN_DEBUG "720*480\n");
		ISX012_WRITE_LIST(ISX012_720_Preview_SizeSetting);
	} else {
		printk(KERN_DEBUG "640*480\n");
		ISX012_WRITE_LIST(ISX012_640_Preview_SizeSetting);
	}

	ISX012_WRITE_LIST(ISX012_Preview_Mode);
	isx012_mode_transtion_CM();
}

static int isx012_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	int temp = 0;

	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	gpio_set_value_cansleep(
		data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	printk(KERN_DEBUG "[isx012] check VT standby : %d\n", temp);
	usleep(10*1000);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	printk(KERN_DEBUG "[isx012] check VT reset : %d\n", temp);
	usleep(10*1000);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	printk(KERN_DEBUG "[isx012] CAM_5M_RST : %d\n", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	printk(KERN_DEBUG "[isx012] CAM_5M_ISP_INIT : %d\n", temp);

	/*Power on the LDOs*/
	data->sensor_platform_info->sensor_power_on(0);
	usleep(5*1000);

	/*standy VT*/
	gpio_set_value_cansleep(
		data->sensor_platform_info->vt_sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	printk(KERN_DEBUG "[isx012] check VT standby : %d\n", temp);
	usleep(10*1000);


	/*Set Main clock*/
	gpio_tlmm_config(GPIO_CFG(
		data->sensor_platform_info->mclk, 1, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	msm_camio_clk_rate_set(24000000);
	usleep(10*1000);


	/*reset VT*/
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	printk(KERN_DEBUG "[isx012] check VT reset : %d\n", temp);
	usleep(10*1000);

	/*off standy VT*/
	gpio_set_value_cansleep(
		data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	printk(KERN_DEBUG "[isx012] check VT standby : %d\n", temp);
	usleep(10*1000);

	/*reset Main cam*/
	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	printk(KERN_DEBUG "[isx012] CAM_5M_RST : %d\n", temp);
	usleep(5*1000);

	/*I2C*/
	printk(KERN_DEBUG "[isx012] Mode Trandition 1\n");

	isx012_mode_transtion_OM();

	usleep(10*1000);
	ISX012_WRITE_LIST(ISX012_Pll_Setting_3);
	printk(KERN_DEBUG "[isx012] Mode Trandition 2\n");

	usleep(10*1000);

	ISX012_WRITE_LIST(ISX012_Pll_Setting_4);
	printk(KERN_DEBUG "[isx012] Mode Trandition 2\n");

	usleep(10*1000);

	isx012_mode_transtion_OM();

	printk(KERN_DEBUG "[isx012] MIPI write\n");

	isx012_i2c_write_multi(0x5008, 0x00, 0x01);
	isx012_Sensor_Calibration();
	ISX012_WRITE_LIST(ISX012_Init_Reg);
	ISX012_WRITE_LIST(ISX012_Preview_SizeSetting);
	ISX012_WRITE_LIST(ISX012_Preview_Mode);

	/*standby Main cam*/
	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	printk(KERN_DEBUG "[isx012] CAM_5M_ISP_INIT : %d\n", temp);
	msleep(20);

	isx012_mode_transtion_OM();

	isx012_mode_transtion_CM();

	msleep(50);

	return rc;
}


int isx012_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);
	if (!isx012_ctrl) {
		printk(KERN_DEBUG "isx012_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		isx012_ctrl->sensordata = data;

	isx012_ctrl->settings.iso = CAMERA_ISO_MODE_AUTO;
	config_csi2 = 0;
#ifdef CONFIG_LOAD_FILE
	isx012_regs_table_init();
#endif
	rc = isx012_sensor_init_probe(data);
	if (rc < 0) {
		printk(KERN_DEBUG "isx012_sensor_init failed!\n");
		goto init_fail;
	}
init_done:
	return rc;

init_fail:
	kfree(isx012_ctrl);
	return rc;
}

static int isx012_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&isx012_wait_queue);
	return 0;
}


void sensor_native_control(void __user *arg)
{
	struct ioctl_native_cmd ctrl_info;

	/*printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);*/

	if (copy_from_user((void *)&ctrl_info,
		(const void *)arg, sizeof(ctrl_info)))
		printk(KERN_DEBUG
			"[isx012] %s fail copy_from_user!\n", __func__);

	/*printk(KERN_DEBUG "[isx012] %d %d %d %d %d\n",
		ctrl_info.mode, ctrl_info.address, ctrl_info.value_1,
		ctrl_info.value_2, ctrl_info.value_3);*/

	switch (ctrl_info.mode) {
	case EXT_CAM_AF:
		if (ctrl_info.address == 0) {
			isx012_get_LowLightCondition();
			if ((isx012_ctrl->flash_mode ==
				CAMERA_FLASH_AUTO && isx012_ctrl->lowLight) ||
				 isx012_ctrl->flash_mode == CAMERA_FLASH_ON) {
				/*Flash On set*/
				ISX012_WRITE_LIST(ISX012_Flash_ON);
				mdelay(40);
				isx012_set_flash(MOVIEMODE_FLASH);
			}
			ISX012_WRITE_LIST(ISX012_Halfrelease_Mode);
		} else if (ctrl_info.address == 1) {
			ctrl_info.value_1 = isx012_sensor_af_status();
			if (ctrl_info.value_1 == 1)
				isx012_i2c_write_multi(0x0012, 0x10, 0x01);
		} else if (ctrl_info.address == 2) {
			ctrl_info.value_1 = isx012_sensor_af_result();
		}
		break;

	case EXT_CAM_FLASH:
		if (ctrl_info.value_1 == 0) {/*off*/
			isx012_set_flash(0);
		} else if (ctrl_info.value_1 == 1) {/*MOVIEMODE_FLASH*/
			isx012_set_flash(MOVIEMODE_FLASH);
		} else if (ctrl_info.value_1 == 2) {/*FLASHMODE_FLASH*/
			isx012_set_flash(FLASHMODE_FLASH);
		}
		break;

	case EXT_CAM_FLASH_MODE:
		isx012_ctrl->flash_mode = ctrl_info.value_1;
		printk(KERN_DEBUG "[isx012] Flash mode : %d\n",
			isx012_ctrl->flash_mode);
		break;

	case EXT_CAM_EV:
		isx012_set_ev(ctrl_info.value_1);
		break;

	case EXT_CAM_EFFECT:
		isx012_set_effect(ctrl_info.value_1);
		break;

	case EXT_CAM_SCENE_MODE:
		isx012_set_scene_mode(ctrl_info.value_1);
		break;

	case EXT_CAM_ISO:
		isx012_set_iso(ctrl_info.value_1);
		break;

	case EXT_CAM_METERING:
		isx012_set_metering(ctrl_info.value_1);
		break;

	case EXT_CAM_WB:
		isx012_set_whitebalance(ctrl_info.value_1);
		break;

	case EXT_CAM_FOCUS:
		isx012_set_focus(ctrl_info.value_1);
		break;

	case EXT_CAM_PREVIEW_SIZE:
		isx012_set_preview_size(ctrl_info.value_1);
		break;

	case  EXT_CAM_MOVIE_MODE:
		CAM_DEBUG("MOVIE mode : %d", ctrl_info.value_1);
		isx012_ctrl->cam_mode = ctrl_info.value_1;

		/* test code for movie mode*/
		ISX012_WRITE_LIST(ISX012_Camcorder_Mode);
		break;

	case EXT_CAM_DTP_TEST:
		isx012_check_dataline(ctrl_info.value_1);
		break;

	default:
		printk(KERN_DEBUG "[isx012] default mode\n");
		break;
	}

	if (copy_to_user((void *)arg,
		(const void *)&ctrl_info, sizeof(ctrl_info)))
		printk(KERN_DEBUG "[isx012] %s fail copy_to_user!\n", __func__);
}


int isx012_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	printk(KERN_DEBUG "[isx012] isx012_ioctl, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

	switch (cfg_data.cfgtype) {
	case CFG_SET_MODE:
		rc = isx012_set_sensor_mode(
					cfg_data.mode);
		break;

	case CFG_GET_AF_MAX_STEPS:
	default:
		rc = 0;
		printk(KERN_DEBUG "isx012_sensor_config : Invalid cfgtype ! %d\n",
			cfg_data.cfgtype);
		break;
	}

	return rc;
}

int isx012_sensor_release(void)
{
	int rc = 0;
	int temp = 0;
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	/*power off the LDOs*/
	isx012_ctrl->sensordata->sensor_platform_info->sensor_power_off(0);

	/*reset VT*/
	gpio_set_value_cansleep(
		isx012_ctrl->sensordata->sensor_platform_info->vt_sensor_reset,
		0);
	temp = gpio_get_value(
		isx012_ctrl->sensordata->sensor_platform_info->vt_sensor_reset);
	printk(KERN_DEBUG "[isx012] check VT reset : %d\n", temp);
	usleep(10*1000);

	/*standy VT*/
	gpio_set_value_cansleep(
		isx012_ctrl->sensordata->sensor_platform_info->vt_sensor_stby,
		0);
	temp = gpio_get_value(
		isx012_ctrl->sensordata->sensor_platform_info->vt_sensor_stby);
	printk(KERN_DEBUG "[isx012] check VT standby : %d\n", temp);
	usleep(10*1000);

	/*reset Main cam*/
	gpio_set_value_cansleep(
		isx012_ctrl->sensordata->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(
		isx012_ctrl->sensordata->sensor_platform_info->sensor_reset);
	printk(KERN_DEBUG "[isx012] CAM_5M_RST : %d\n", temp);

	/*standby Main cam*/
	gpio_set_value_cansleep(
		isx012_ctrl->sensordata->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(
		isx012_ctrl->sensordata->sensor_platform_info->sensor_stby);
	printk(KERN_DEBUG "[isx012] CAM_5M_ISP_INIT : %d\n", temp);

	/*I2C*/

#ifdef CONFIG_LOAD_FILE
	isx012_regs_table_exit();
#endif
	return rc;
}

static int isx012_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	isx012_sensorw =
		kzalloc(sizeof(struct isx012_work), GFP_KERNEL);

	if (!isx012_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, isx012_sensorw);
	isx012_init_client(client);
	isx012_client = client;


	printk(KERN_DEBUG "isx012_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(isx012_sensorw);
	isx012_sensorw = NULL;
	printk(KERN_DEBUG "isx012_probe failed!\n");
	return rc;
}

static const struct i2c_device_id isx012_i2c_id[] = {
	{ "isx012", 0},
	{ },
};

static struct i2c_driver isx012_i2c_driver = {
	.id_table = isx012_i2c_id,
	.probe  = isx012_i2c_probe,
	.remove = __exit_p(isx012_i2c_remove),
	.driver = {
		.name = "isx012",
	},
};


static int isx012_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	int rc = i2c_add_driver(&isx012_i2c_driver);
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	if (rc < 0 || isx012_client == NULL) {
		rc = -ENOTSUPP;
		goto probe_done;
	}

	msm_camio_clk_rate_set(24000000);

	s->s_init = isx012_sensor_init;
	s->s_release = isx012_sensor_release;
	s->s_config  = isx012_sensor_config;
	s->s_camera_type = BACK_CAMERA_2D;
	s->s_mount_angle = 90;


probe_done:
	printk(KERN_DEBUG "%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}


static struct isx012_format isx012_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static int isx012_enum_fmt(struct v4l2_subdev *sd, unsigned int index,
			   enum v4l2_mbus_pixelcode *code)
{
	printk(KERN_DEBUG "Index is %d\n", index);
	if ((unsigned int)index >= ARRAY_SIZE(isx012_subdev_info))
		return -EINVAL;

	*code = isx012_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops isx012_subdev_core_ops;
static struct v4l2_subdev_video_ops isx012_subdev_video_ops = {
	.enum_mbus_fmt = isx012_enum_fmt,
};

static struct v4l2_subdev_ops isx012_subdev_ops = {
	.core = &isx012_subdev_core_ops,
	.video  = &isx012_subdev_video_ops,
};

static int isx012_sensor_probe_cb(const struct msm_camera_sensor_info *info,
	struct v4l2_subdev *sdev, struct msm_sensor_ctrl *s)
{
	int rc = 0;
	printk(KERN_DEBUG "[isx012] %s/%d\n", __func__, __LINE__);

	rc = isx012_sensor_probe(info, s);
	if (rc < 0)
		return rc;

	isx012_ctrl = kzalloc(sizeof(struct isx012_ctrl), GFP_KERNEL);
	if (!isx012_ctrl) {
		printk(KERN_DEBUG "isx012_sensor_probe failed!\n");
		return -ENOMEM;
	}

	/* probe is successful, init a v4l2 subdevice */
	if (sdev) {
		v4l2_i2c_subdev_init(sdev, isx012_client,
						&isx012_subdev_ops);
		isx012_ctrl->sensor_dev = sdev;
	} else {
		printk(KERN_DEBUG "[isx012] sdev is null in probe_cb\n");
	}
	return rc;
}

static int __isx012_probe(struct platform_device *pdev)
{
	printk(KERN_DEBUG "############# ISX012 probe ##############\n");

	return msm_sensor_register(pdev, isx012_sensor_probe_cb);
}

static struct platform_driver msm_camera_driver = {
	.probe = __isx012_probe,
	.driver = {
		.name = "msm_camera_isx012",
		.owner = THIS_MODULE,
	},
};

static int __init isx012_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(isx012_init);
