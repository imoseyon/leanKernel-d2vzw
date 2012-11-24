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
#include "s5k8aay.h"

#include "msm.h"
#include "msm_ispif.h"
#include "msm_sensor.h"

/*#define CONFIG_LOAD_FILE */
#ifdef CONFIG_LOAD_FILE

#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static char *s5k8aay_regs_table;
static int s5k8aay_regs_table_size;
static int s5k8aay_write_regs_from_sd(char *name);
static int s5k8aay_i2c_write_multi(unsigned short addr, unsigned int w_data);
#define S5K8_BURST_WRT_LIST(A)	\
	s5k8_i2c_wrt_list(A, (sizeof(A) / sizeof(A[0])), #A)
#else
#define S5K8_BURST_WRT_LIST(A)	\
	s5k8_i2c_burst_wrt_list(A, (sizeof(A) / sizeof(A[0])), #A)
#endif
static int s5k8aay_sensor_config(struct msm_sensor_ctrl_t *s_ctrl,
	void __user *argp);
DEFINE_MUTEX(s5k8aay_mut);

#define S5K8_WRT_LIST(A)	\
	s5k8_i2c_wrt_list(A, (sizeof(A) / sizeof(A[0])), #A)

#define CAM_REV ((system_rev <= 1) ? 0 : 1)
#ifdef CAM_REV
	#include "s5k8aay_regs_v2.h"
#else
	#include "s5k8aay_regs.h"
#endif

struct s5k8aay_work {
	struct work_struct work;
};

static struct  i2c_client *s5k8aay_client;
static struct msm_sensor_ctrl_t s5k8aay_s_ctrl;
static struct device s5k8aay_dev;

struct s5k8aay_ctrl {
	const struct msm_camera_sensor_info *sensordata;
	struct s5k8aay_userset settings;
	struct msm_camera_i2c_client *sensor_i2c_client;
	struct v4l2_subdev *sensor_dev;
	struct v4l2_subdev sensor_v4l2_subdev;
	struct v4l2_subdev_info *sensor_v4l2_subdev_info;
	uint8_t sensor_v4l2_subdev_info_size;
	struct v4l2_subdev_ops *sensor_v4l2_subdev_ops;

	int op_mode;
	int cam_mode;
	int vtcall_mode;
	int dtpTest;
	int isHDSize;
	int samsungapp;

	unsigned short shutter_speed;
	unsigned short iso_speed;
};

static unsigned int config_csi2;
static struct s5k8aay_ctrl *s5k8aay_ctrl;

struct s5k8aay_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};

static bool g_bFrontCameraRunning;

static int s5k8aay_set_effect(int effect);
static int s5k8aay_set_whitebalance(int wb);
static void s5k8aay_set_ev(int ev);

#ifdef CONFIG_LOAD_FILE

void s5k8aay_regs_table_init(void)
{
	struct file *filp;
	char *dp;
	long lsize;
	loff_t pos;
	int ret;

	/*Get the current address space */
	mm_segment_t fs = get_fs();

	CAM_DEBUG(" %d", __LINE__);

	/*Set the current segment to kernel data segment */
	set_fs(get_ds());

	filp = filp_open("/mnt/sdcard/s5k8aay_regs_v2.h", O_RDONLY, 0);

	if (IS_ERR_OR_NULL(filp)) {
		cam_err(" file open error");
		return ;
	}

	lsize = filp->f_path.dentry->d_inode->i_size;
	dp = vmalloc(lsize);
	if (dp == NULL) {
		cam_err(" Out of Memory");
		filp_close(filp, current->files);
	}

	pos = 0;
	memset(dp, 0, lsize);
	ret = vfs_read(filp, (char __user *)dp, lsize, &pos);
	if (ret != lsize) {
		cam_err(" Failed to read file ret = %d", ret);
		vfree(dp);
		filp_close(filp, current->files);
	}
	/*close the file*/
	filp_close(filp, current->files);

	/*restore the previous address space*/
	set_fs(fs);

	s5k8aay_regs_table = dp;

	s5k8aay_regs_table_size = lsize;

	*((s5k8aay_regs_table + s5k8aay_regs_table_size) - 1) = '\0';

	return;
}
#endif

#ifdef CONFIG_LOAD_FILE

void s5k8aay_regs_table_exit(void)
{
	CAM_DEBUG(" %d", __LINE__);
	if (s5k8aay_regs_table) {
		vfree(s5k8aay_regs_table);
		s5k8aay_regs_table = NULL;
	}
}

#endif

#ifdef CONFIG_LOAD_FILE
static int s5k8aay_write_regs_from_sd(char *name)
{
	char *start, *end, *reg, *size;
	unsigned short addr;
	unsigned int len, value;
	char reg_buf[7], data_buf1[5], data_buf2[7];


	*(reg_buf + 6) = '\0';
	*(data_buf1 + 4) = '\0';
	*(data_buf2 + 6) = '\0';

	CAM_DEBUG(" s5k8aay_regs_table_write start!");
	CAM_DEBUG(" E string = %s", name);

	start = strstr(s5k8aay_regs_table, name);
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
				kstrtoint(reg_buf, 16, &addr);
				kstrtoint(data_buf1, 16, &value);

				if (reg)
					start = (reg + 12);
			} else {/* 2 byte write */
				kstrtoint(reg_buf, 16, &addr);
				kstrtoint(data_buf2, 16, &value);
				if (reg)
					start = (reg + 14);
			}
			size = NULL;

			CAM_DEBUG(" addr 0x%04x, value 0x%04x", addr, value);

			if (addr == 0xFFFF)
				msleep(value);
			else
				s5k8aay_i2c_write_multi(addr, value);

		}
	}
	CAM_DEBUG(" s5k8aay_regs_table_write end!");

	return 0;
}

#endif

static DECLARE_WAIT_QUEUE_HEAD(s5k8aay_wait_queue);

#ifdef UNUSED /* unused */
/**
 * s5k8aay_i2c_read_multi: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 * @r_data: buffer where data is read
 * @r_len: number of bytes to read
 *
 * Returns 0 on success, <0 on error
 */
static int s5k8aay_i2c_read_multi(unsigned short subaddr, unsigned long *data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k8aay_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k8aay_client->adapter) {
		cam_err(" can't search i2c client adapter");
		return -EIO;
	}

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		cam_err(" i2c_transfer  error, %d", err);
		return -EIO;
	}

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		cam_err(" i2c_transfer returned error, %d", err);
		return -EIO;
	}
	/*
	 * Data comes in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */

	*data = *(unsigned long *)(&buf);

	return err;
}
#endif

/**
 * s5k8aay_i2c_read: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @data: data to be read
 *
 * Returns 0 on success, <0 on error
 */
static int s5k8aay_i2c_read(unsigned short subaddr, unsigned short *data)
{
	unsigned char buf[2];
	struct i2c_msg msg = {s5k8aay_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k8aay_client->adapter) {
		cam_err(" can't search i2c client adapter");
		return -EIO;
	}

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		cam_err(" i2c_transfer returned error, %d", err);
		return -EIO;
	}

	msg.flags = I2C_M_RD;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		cam_err(" i2c_transfer returned error, %d", err);
		return -EIO;
	}
	/*
	 * Data comes in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */

	*data = ((buf[0] << 8) | buf[1]);

	return err;
}

/**
 * s5k8aa_i2c_write_multi: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static int s5k8aay_i2c_write_multi(unsigned short addr, unsigned int w_data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k8aay_client->addr, 0, 4, buf};

	int retry_count = 5;
	int err = 0;

	if (!s5k8aay_client->adapter) {
		cam_err(" can't search i2c client adapter");
		return -EIO;
	}

	buf[0] = addr >> 8;
	buf[1] = addr & 0xFF;
	buf[2] = w_data >> 8;
	buf[3] = w_data & 0xFF;
	/*
	 * Data should be written in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */

	while (retry_count--) {
		err  = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
	}

	if (err != 1) {
		cam_err(" i2c_transfer returned error, %d", err);
		return -EIO;
	}

	return 0;
}

static int s5k8_i2c_wrt_list(struct s5k8aay_short_t regs[],
	int size, char *name)
{
#ifdef CONFIG_LOAD_FILE
	s5k8aay_write_regs_from_sd(name);
#else
	int err = 0;
	int i = 0;

	CAM_DEBUG(" %s", name);

	if (!s5k8aay_client->adapter) {
		cam_err(" can't search i2c client adapter");
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		if (regs[i].subaddr == 0xFFFF) {
			msleep(regs[i].value);
			CAM_DEBUG(" delay = 0x%04x, value = 0x%04x",
				regs[i].subaddr, regs[i].value);
		} else {
			err = s5k8aay_i2c_write_multi(regs[i].subaddr,
								regs[i].value);
			if (unlikely(err < 0)) {
				cam_err(" register set failed");
				return -EIO;
			}
		}
	}
#endif

	return 0;
}

#define S5K8_BURST_DATA_LENGTH 2700
unsigned char s5k8aay_buf[S5K8_BURST_DATA_LENGTH];
static int s5k8_i2c_burst_wrt_list(struct s5k8aay_short_t regs[], int size,
	char *name)
{
	int err = 1;

	unsigned short subaddr = 0;
	unsigned short next_subaddr = 0;
	unsigned short value = 0;

	int retry_count = 5;
	int i = 0, idx = 0;

	struct i2c_msg msg = {s5k8aay_client->addr, 0, 0, s5k8aay_buf};

	if (!s5k8aay_client->adapter) {
		cam_err(" can't search i2c client adapter");
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		if (idx > (S5K8_BURST_DATA_LENGTH - 10))
			cam_err(" BURST MODE buffer overflow!!!");

		subaddr = regs[i].subaddr; /* address */
		if (subaddr == 0x0F12)
			next_subaddr = regs[i+1].subaddr; /* address */
		value = (regs[i].value & 0xFFFF); /* value */

		retry_count = 5;

		switch (subaddr) {
		case 0x0F12:
			/* make and fill buffer for burst mode write */
			if (idx == 0) {
				s5k8aay_buf[idx++] = 0x0F;
				s5k8aay_buf[idx++] = 0x12;
			}
			s5k8aay_buf[idx++] = value >> 8;
			s5k8aay_buf[idx++] = value & 0xFF;

			/* write in burstmode */
			if (next_subaddr != 0x0F12) {
				msg.len = idx;
				while (retry_count--) {
					err = i2c_transfer(
					s5k8aay_client->adapter, &msg, 1);
					if (likely(err == 1))
						break;
				}
				idx = 0;
			}
			break;

		case 0xFFFF:
			msleep(value);
			break;

		default:
		    idx = 0;
		    s5k8aay_i2c_write_multi(subaddr, value);
			break;
		}
	}

	if (err != 1) {
		cam_err(" returned error, %d\n", err);
		return -EIO;
	}

	return 0;
}

#ifdef FACTORY_TEST
struct class *sec_class;
struct device *s5k8aay_dev;

static ssize_t cameratype_file_cmd_show(struct device *dev,
				struct device_attribute *attr, char *buf) {
	char sensor_info[30] = "s5k8aay";
	return snprintf(buf, "%s\n", sensor_info);
}

static ssize_t cameratype_file_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
		/*Reserved*/
	return size;
}

static struct device_attribute s5k8aay_camtype_attr = {
	.attr = {
		.name = "camtype",
		.mode = (S_IRUGO | S_IWUGO)},
		.show = cameratype_file_cmd_show,
		.store = cameratype_file_cmd_store
};

static ssize_t cameraflash_file_cmd_show(struct device *dev,
				struct device_attribute *attr, char *buf) {
		/*Reserved*/
	return 0;
}

static ssize_t cameraflash_file_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
	int value;
	sscanf(buf, "%d", &value);

	if (value == 0) {
		CAM_DEBUG(" [Factory flash]OFF");
		s5k8aay_set_flash(MOVIE_FLASH, 0);
	} else {
		CAM_DEBUG(" [Factory flash]ON");
		s5k8aay_set_flash(MOVIE_FLASH, 1);
	}
	return size;
}

static struct device_attribute s5k8aay_cameraflash_attr = {
	.attr = {
		.name = "cameraflash",
		.mode = (S_IRUGO | S_IWUGO)},
		.show = cameraflash_file_cmd_show,
		.store = cameraflash_file_cmd_store
};
#endif

void s5k8aay_get_exif_shutter_speed(void)
{
	unsigned short read_value_lsb = 0;
	unsigned short read_value_msb = 0;

	s5k8aay_i2c_write_multi(0xFCFC, 0xD000);
	s5k8aay_i2c_write_multi(0x002C, 0x7000);
	s5k8aay_i2c_write_multi(0x002E, 0x20DC);

	s5k8aay_i2c_read(0x0F12, &read_value_lsb);
	s5k8aay_i2c_read(0x0F12, &read_value_msb);

	s5k8aay_ctrl->shutter_speed = 400000 / (read_value_lsb
		+ (read_value_msb<<16));
}

void s5k8aay_get_exif_iso_speed_rate(void)
{
	u16 iso_gain_table[] = {10, 20, 30, 40};
	u16 iso_table[] = {50, 100, 200, 400};
	u16 gain = 0, val = 0;
	s32 index = 0;

	s5k8aay_i2c_write_multi(0xFCFC, 0xD000);
	s5k8aay_i2c_write_multi(0x002C, 0x7000);
	s5k8aay_i2c_write_multi(0x002E, 0x20E0);

	s5k8aay_i2c_read(0x0F12, &val);

	gain = (val / 256) * 10;
	for (index = 0; index < 3; index++) {
		if (gain < iso_gain_table[index])
			break;
	}

	s5k8aay_ctrl->iso_speed =  iso_table[index];
}

static int s5k8aay_get_exif(int exif_cmd)
{
	unsigned short val;

	switch (exif_cmd) {
	case EXIF_SHUTTERSPEED:
		val = s5k8aay_ctrl->shutter_speed;
		break;

	case EXIF_ISO:
		val = s5k8aay_ctrl->iso_speed;
		break;

	default:
		break;
	}
	return val;
}

void s5k8aay_set_init_mode(void)
{
	config_csi2 = 0;
	g_bFrontCameraRunning = false;
	s5k8aay_ctrl->cam_mode = PREVIEW_MODE;
	s5k8aay_ctrl->op_mode = CAMERA_MODE_INIT;
	s5k8aay_ctrl->vtcall_mode = 0;
	s5k8aay_ctrl->samsungapp = 0;
}

void s5k8aay_set_preview_size(int32_t index)
{
	CAM_DEBUG(" %d", index);

	s5k8aay_ctrl->settings.preview_size_idx = index;
}

void s5k8aay_set_preview(void)
{
	int stable_delay = 250;

	CAM_DEBUG(" cam_mode = %d, vt_mode = %d",
		s5k8aay_ctrl->cam_mode, s5k8aay_ctrl->vtcall_mode);

	if (s5k8aay_ctrl->cam_mode == MOVIE_MODE) {
		if (s5k8aay_ctrl->settings.preview_size_idx ==
				PREVIEW_SIZE_HD) {
			CAM_DEBUG(" 720P recording common");
			if (s5k8aay_ctrl->op_mode == CAMERA_MODE_INIT ||
				s5k8aay_ctrl->op_mode == CAMERA_MODE_PREVIEW ||
				s5k8aay_ctrl->isHDSize == 0) {
				s5k8aay_ctrl->isHDSize = 1;
				S5K8_BURST_WRT_LIST(s5k8aay_720p_common);
			}
		} else {
			CAM_DEBUG(" VGA recording common");
			if (s5k8aay_ctrl->op_mode == CAMERA_MODE_INIT ||
				s5k8aay_ctrl->op_mode == CAMERA_MODE_PREVIEW ||
				s5k8aay_ctrl->isHDSize == 1) {
				s5k8aay_ctrl->isHDSize = 0;
				S5K8_BURST_WRT_LIST(s5k8aay_recording_common);
			}
		}
		s5k8aay_ctrl->op_mode = CAMERA_MODE_RECORDING;
		stable_delay = 150;
	} else {
		if (s5k8aay_ctrl->op_mode == CAMERA_MODE_INIT ||
			s5k8aay_ctrl->op_mode == CAMERA_MODE_RECORDING) {
			if (s5k8aay_ctrl->vtcall_mode == 1) {
				CAM_DEBUG(" VT common");
				S5K8_BURST_WRT_LIST(s5k8aay_skt_vt_common);
				stable_delay = 350;
			} else if (s5k8aay_ctrl->vtcall_mode == 2) {
				CAM_DEBUG(" WIFI VT common");
				S5K8_BURST_WRT_LIST(s5k8aay_wifi_vt_common);
				stable_delay = 150;
			} else {
				CAM_DEBUG(" Normal common");
				S5K8_BURST_WRT_LIST(s5k8aay_common);
				stable_delay = 250;
			}
		}
		S5K8_WRT_LIST(s5k8aay_preview);  /* add delay 150ms */
		s5k8aay_ctrl->op_mode = CAMERA_MODE_PREVIEW;
	}

	s5k8aay_set_ev(s5k8aay_ctrl->settings.brightness);

	if (!g_bFrontCameraRunning) {
		g_bFrontCameraRunning = true;
		msleep(stable_delay);
	}
}

void s5k8aay_set_capture(void)
{
	CAM_DEBUG(" E");
	S5K8_BURST_WRT_LIST(s5k8aay_capture);
	s5k8aay_get_exif_shutter_speed();
	s5k8aay_get_exif_iso_speed_rate();

	s5k8aay_ctrl->op_mode = CAMERA_MODE_CAPTURE;
}

static int32_t s5k8aay_sensor_setting(int update_type, int rt)
{
	int32_t rc = 0;
	struct msm_camera_csid_params s5k8aay_csid_params;
	struct msm_camera_csiphy_params s5k8aay_csiphy_params;

	CAM_DEBUG(" type = %d", update_type);

	switch (update_type) {
	case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE)
			/* Add some condition statements */
		break;

	case UPDATE_PERIODIC:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			struct msm_camera_csid_vc_cfg s5k8aay_vccfg[] = {
				{0, 0x1E, CSI_DECODE_8BIT},
				{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
			};

			CAM_DEBUG(" UPDATE_PERIODIC");

			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX_0, ISPIF_OFF_IMMEDIATELY));
			msleep(30);

			s5k8aay_csid_params.lane_cnt = 1;
			s5k8aay_csid_params.lane_assign = 0xe4;
			s5k8aay_csid_params.lut_params.num_cid =
				ARRAY_SIZE(s5k8aay_vccfg);
			s5k8aay_csid_params.lut_params.vc_cfg =
				&s5k8aay_vccfg[0];
			s5k8aay_csiphy_params.lane_cnt = 1;
			s5k8aay_csiphy_params.settle_cnt = 0x14;
			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_CSID_CFG, &s5k8aay_csid_params);
			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_CID_CHANGE, NULL);
			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_CSIPHY_CFG, &s5k8aay_csiphy_params);
			mb();

			msleep(20);

			config_csi2 = 1;

			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX_0, ISPIF_ON_FRAME_BOUNDARY));
			msleep(30);
		}
		break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static long s5k8aay_set_sensor_mode(int mode)
{
	CAM_DEBUG(" %d", mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	case SENSOR_VIDEO_MODE:
		s5k8aay_set_preview();
		break;
	case SENSOR_SNAPSHOT_MODE:
	case SENSOR_RAW_SNAPSHOT_MODE:
		s5k8aay_set_capture();
		break;
	default:
		return 0;
	}
	return 0;
}

static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

static int s5k8aay_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;

	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG(" E");

#ifdef CONFIG_LOAD_FILE
	s5k8aay_regs_table_init();
#endif

	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0)
		cam_err(" request gpio failed");

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);

	/*Power on the LDOs */
	data->sensor_platform_info->sensor_power_on(1);

	/*standy VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);

	/*Set Main clock */
	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk,
					1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
					GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;

	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0)
		cam_err(" clk enable failed");

	usleep(15);

	/*reset VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	usleep(100);

	/* sensor validation test */
	rc = S5K8_BURST_WRT_LIST(s5k8aay_pre_common);
	if (rc < 0) {
		pr_info("Error in Front Camera Sensor Validation Test");
		return rc;
	}

	s5k8aay_set_init_mode();

	CAM_DEBUG(" X");

	return rc;
}

static void s5k8aay_check_dataline(int val)
{
	if (val) {
		CAM_DEBUG(" DTP ON");
		s5k8aay_ctrl->dtpTest = 1;

	} else {
		CAM_DEBUG(" DTP OFF");
		s5k8aay_ctrl->dtpTest = 0;
	}
}

static int s5k8aay_set_effect(int effect)
{
	CAM_DEBUG(" %d", effect);

	switch (effect) {
	case CAMERA_EFFECT_OFF:
		S5K8_BURST_WRT_LIST(s5k8aay_effect_none);
		break;

	case CAMERA_EFFECT_MONO:
		S5K8_BURST_WRT_LIST(s5k8aay_effect_mono);
		break;

	case CAMERA_EFFECT_NEGATIVE:
		S5K8_BURST_WRT_LIST(s5k8aay_effect_negative);
		break;

	case CAMERA_EFFECT_SEPIA:
		S5K8_BURST_WRT_LIST(s5k8aay_effect_sepia);
		break;

	case CAMERA_EFFECT_WHITEBOARD:
		S5K8_BURST_WRT_LIST(s5k8aay_effect_sketch);
		break;

	default:
		break;
	}

	s5k8aay_ctrl->settings.effect = effect;

	return 0;
}

static int s5k8aay_set_whitebalance(int wb)
{
	CAM_DEBUG(" %d", wb);

	switch (wb) {
	case CAMERA_WHITE_BALANCE_AUTO:
			S5K8_BURST_WRT_LIST(s5k8aay_wb_auto);
		break;

	case CAMERA_WHITE_BALANCE_INCANDESCENT:
			S5K8_BURST_WRT_LIST(s5k8aay_wb_incandescent);
		break;

	case CAMERA_WHITE_BALANCE_FLUORESCENT:
			S5K8_BURST_WRT_LIST(s5k8aay_wb_fluorescent);
		break;

	case CAMERA_WHITE_BALANCE_DAYLIGHT:
			S5K8_BURST_WRT_LIST(s5k8aay_wb_daylight);
		break;

	case CAMERA_WHITE_BALANCE_CLOUDY_DAYLIGHT:
			S5K8_BURST_WRT_LIST(s5k8aay_wb_cloudy);
		break;

	default:
		break;
	}
	s5k8aay_ctrl->settings.wb = wb;

	return 0;
}

static void s5k8aay_set_ev(int ev)
{
	CAM_DEBUG(" %d", ev);

	switch (ev) {
	case CAMERA_EV_M4:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_M4);
		break;

	case CAMERA_EV_M3:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_M3);
		break;

	case CAMERA_EV_M2:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_M2);
		break;

	case CAMERA_EV_M1:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_M1);
		break;

	case CAMERA_EV_DEFAULT:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_default);
		break;

	case CAMERA_EV_P1:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_P1);
		break;

	case CAMERA_EV_P2:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_P2);
		break;

	case CAMERA_EV_P3:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_P3);
		break;

	case CAMERA_EV_P4:
		S5K8_BURST_WRT_LIST(s5k8aay_brightness_P4);
		break;

	default:
		break;
	}
	s5k8aay_ctrl->settings.brightness = ev;
}

static void s5k8aay_set_frame_rate(int fps)
{
	CAM_DEBUG(" %d", fps);

	switch (fps) {
	case 15:
		S5K8_BURST_WRT_LIST(s5k8aay_vt_15fps);
		break;

	case 12:
		S5K8_BURST_WRT_LIST(s5k8aay_vt_12fps);
		break;

	case 10:
		S5K8_BURST_WRT_LIST(s5k8aay_vt_10fps);
		break;

	case 7:
		S5K8_BURST_WRT_LIST(s5k8aay_vt_7fps);
		break;

	default:
		break;
	}

	s5k8aay_ctrl->settings.fps = fps;
}

void sensor_native_control_front(void __user *arg)
{
	struct ioctl_native_cmd ctrl_info;

	if (copy_from_user((void *)&ctrl_info,
		(const void *)arg, sizeof(ctrl_info)))
		cam_err(" fail copy_from_user!");

	switch (ctrl_info.mode) {
	case EXT_CAM_EV:
		s5k8aay_set_ev(ctrl_info.value_1);
		break;

	case EXT_CAM_EFFECT:
		s5k8aay_set_effect(ctrl_info.value_1);
		break;

	case EXT_CAM_WB:
		s5k8aay_set_whitebalance(ctrl_info.value_1);
		break;

	case EXT_CAM_DTP_TEST:
		s5k8aay_check_dataline(ctrl_info.value_1);
		break;

	case  EXT_CAM_MOVIE_MODE:
		CAM_DEBUG(" MOVIE mode : %d", ctrl_info.value_1);
		s5k8aay_ctrl->cam_mode = ctrl_info.value_1;
		break;

	case EXT_CAM_PREVIEW_SIZE:
		s5k8aay_set_preview_size(ctrl_info.value_1);
		break;

	case EXT_CAM_EXIF:
		ctrl_info.value_1 = s5k8aay_get_exif(ctrl_info.address);
		break;

	case EXT_CAM_VT_MODE:
		CAM_DEBUG(" VT mode : %d", ctrl_info.value_1);
		s5k8aay_ctrl->vtcall_mode = ctrl_info.value_1;
		break;

	case EXT_CAM_SET_FPS:
		s5k8aay_set_frame_rate(ctrl_info.value_1);
		break;

	case EXT_CAM_SAMSUNG_CAMERA:
		CAM_DEBUG(" SAMSUNG camera : %d", ctrl_info.value_1);
		s5k8aay_ctrl->samsungapp = ctrl_info.value_1;
		break;

	default:
		break;
	}

	if (copy_to_user((void *)arg,
		(const void *)&ctrl_info, sizeof(ctrl_info)))
		cam_err(" fail copy_to_user!");
}

long s5k8aay_sensor_subdev_ioctl(struct v4l2_subdev *sd,
			unsigned int cmd, void *arg)
{
	void __user *argp = (void __user *)arg;
	switch (cmd) {
	case VIDIOC_MSM_SENSOR_CFG:
		return s5k8aay_sensor_config(&s5k8aay_s_ctrl, argp);
	default:
		cam_err(" Invalid cmd = %d", cmd);
		return -ENOIOCTLCMD;
	}
}

static int s5k8aay_sensor_config(struct msm_sensor_ctrl_t *s_ctrl,
	void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data, (void *)argp,
						sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	CAM_DEBUG(" cfgtype = %d, mode = %d",
			cfg_data.cfgtype, cfg_data.mode);
	switch (cfg_data.cfgtype) {
	case CFG_SENSOR_INIT:
		if (config_csi2 == 0)
			rc = s5k8aay_sensor_setting(
				UPDATE_PERIODIC, RES_PREVIEW);
		break;
	case CFG_SET_MODE:
		rc = s5k8aay_set_sensor_mode(cfg_data.mode);
		break;
	case CFG_GET_AF_MAX_STEPS:
	default:
		rc = 0;
		break;
	}
	return rc;
}

static int s5k8aay_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG(" E");

#ifdef CONFIG_LOAD_FILE
	s5k8aay_regs_table_exit();
#endif

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	usleep(10); /* 20clk = 0.833us */

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);

	/*CAM_MCLK0*/
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);

	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 0,
			GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);

	data->sensor_platform_info->sensor_power_off(1);

	rc = msm_camera_request_gpio_table(data, 0);
	if (rc < 0)
		cam_err(" request gpio failed");

	CAM_DEBUG(" X");

	return rc;
}

struct v4l2_subdev_info s5k8aay_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static int s5k8aay_enum_fmt(struct v4l2_subdev *sd, unsigned int index,
			   enum v4l2_mbus_pixelcode *code) {
	if ((unsigned int)index >= ARRAY_SIZE(s5k8aay_subdev_info))
		return -EINVAL;

	*code = s5k8aay_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops s5k8aay_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = s5k8aay_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k8aay_subdev_video_ops = {
	.enum_mbus_fmt = s5k8aay_enum_fmt,
};

static struct v4l2_subdev_ops s5k8aay_subdev_ops = {
	.core = &s5k8aay_subdev_core_ops,
	.video  = &s5k8aay_subdev_video_ops,
};

static int s5k8aay_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;

	CAM_DEBUG(" E");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		cam_err("i2c_check_functionality failed");
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr =
				s_ctrl->sensor_i2c_addr;
	} else {
		cam_err("s_ctrl->sensor_i2c_client is NULL");
		rc = -EFAULT;
		goto probe_failure;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		cam_err(" NULL sensor data");
		rc = -EFAULT;
		goto probe_failure;
	}

	s5k8aay_client = client;
	s5k8aay_dev = s_ctrl->sensor_i2c_client->client->dev;

	s5k8aay_ctrl = kzalloc(sizeof(struct s5k8aay_ctrl), GFP_KERNEL);
	if (!s5k8aay_ctrl) {
		cam_err(" s5k8aay_ctrl alloc failed!");
		rc = -ENOMEM;
		goto probe_failure;
	}

	memset(s5k8aay_ctrl, 0, sizeof(s5k8aay_ctrl));

	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);

	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		&s5k8aay_subdev_ops);

	s5k8aay_ctrl->sensor_dev = &s_ctrl->sensor_v4l2_subdev;
	s5k8aay_ctrl->sensordata = client->dev.platform_data;

	rc = msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);
	if (rc < 0) {
		cam_err(" msm_sensor_register failed!");
		kfree(s5k8aay_ctrl);
		goto probe_failure;
	}
	CAM_DEBUG(" success!");
	CAM_DEBUG(" X");
	return 0;

probe_failure:
	CAM_DEBUG(" fail!");
	CAM_DEBUG(" X");
	return rc;
}

static const struct i2c_device_id s5k8aay_i2c_id[] = {
	{"s5k8aay", (kernel_ulong_t)&s5k8aay_s_ctrl},
	{},
};

static struct msm_camera_i2c_client s5k8aay_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct i2c_driver s5k8aay_i2c_driver = {
	.id_table = s5k8aay_i2c_id,
	.probe  = s5k8aay_i2c_probe,
	.driver = {
		.name = "s5k8aay",
	},
};

static int __init s5k8aay_init(void)
{
	return i2c_add_driver(&s5k8aay_i2c_driver);
}

static struct msm_sensor_fn_t s5k8aay_func_tbl = {
	.sensor_config = s5k8aay_sensor_config,
	.sensor_power_up = s5k8aay_sensor_power_up,
	.sensor_power_down = s5k8aay_sensor_power_down,
};


static struct msm_sensor_reg_t s5k8aay_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_sensor_ctrl_t s5k8aay_s_ctrl = {
	.msm_sensor_reg = &s5k8aay_regs,
	.sensor_i2c_client = &s5k8aay_sensor_i2c_client,
	.sensor_i2c_addr = 0x2d,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &s5k8aay_mut,
	.sensor_i2c_driver = &s5k8aay_i2c_driver,
	.sensor_v4l2_subdev_info = s5k8aay_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k8aay_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k8aay_subdev_ops,
	.func_tbl = &s5k8aay_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(s5k8aay_init);
