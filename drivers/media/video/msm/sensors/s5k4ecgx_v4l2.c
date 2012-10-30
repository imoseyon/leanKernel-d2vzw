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
#include "s5k4ecgx.h"
#include "s5k4ecgx_regs.h"
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

static char *s5k4ecgx_regs_table;
static int s5k4ecgx_regs_table_size;
static int s5k4ecgx_write_regs_from_sd(char *name);
static int s5k4ecgx_i2c_write_multi(unsigned short addr, unsigned int w_data);
#endif
static int s5k4ecgx_sensor_config(void __user *argp);
DEFINE_MUTEX(s5k4ecgx_mut);

#define S5K4ECGX_WRT_LIST(A)	\
	 s5k4ecgx_i2c_wrt_list(A, (sizeof(A) / sizeof(A[0])), #A);

#define CAM_REV ((system_rev <= 1) ? 0 : 1)

struct s5k4ecgx_work {
	struct work_struct work;
};

static struct  i2c_client *s5k4ecgx_client;
static struct msm_sensor_ctrl_t s5k4ecgx_s_ctrl;
static struct device s5k4ecgx_dev;

struct s5k4ecgx_ctrl {
	const struct msm_camera_sensor_info *sensordata;
	struct s5k4ecgx_userset settings;
	struct msm_camera_i2c_client *sensor_i2c_client;
	struct v4l2_subdev *sensor_dev;
	struct v4l2_subdev sensor_v4l2_subdev;
	struct v4l2_subdev_info *sensor_v4l2_subdev_info;
	uint8_t sensor_v4l2_subdev_info_size;
	struct v4l2_subdev_ops *sensor_v4l2_subdev_ops;

	int op_mode;
	int dtp_mode;
	int cam_mode;
	int vtcall_mode;
	int started;
	int dtpTest;
	int isCapture;
};

static unsigned int config_csi2;
static struct s5k4ecgx_ctrl *s5k4ecgx_ctrl;

struct s5k4ecgx_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};


#ifdef CONFIG_LOAD_FILE

void s5k4ecgx_regs_table_init(void)
{
	struct file *filp;
	char *dp;
	long lsize;
	loff_t pos;
	int ret;

	/*Get the current address space */
	mm_segment_t fs = get_fs();

	CAM_DEBUG("%s %d", __func__, __LINE__);

	/*Set the current segment to kernel data segment */
	set_fs(get_ds());

	filp = filp_open("/mnt/sdcard/s5k4ecgx_regs.h", O_RDONLY, 0);

	if (IS_ERR_OR_NULL(filp)) {
		cam_err("file open error\n");
		return ;
	}

	lsize = filp->f_path.dentry->d_inode->i_size;
	CAM_DEBUG("size : %ld", lsize);
	dp = vmalloc(lsize);
	if (dp == NULL) {
		cam_err("Out of Memory");
		filp_close(filp, current->files);
	}

	pos = 0;
	memset(dp, 0, lsize);
	ret = vfs_read(filp, (char __user *)dp, lsize, &pos);
	if (ret != lsize) {
		cam_err("Failed to read file ret = %d\n", ret);
		vfree(dp);
		filp_close(filp, current->files);
	}
	/*close the file*/
	filp_close(filp, current->files);

	/*restore the previous address space*/
	set_fs(fs);

	s5k4ecgx_regs_table = dp;

	s5k4ecgx_regs_table_size = lsize;

	*((s5k4ecgx_regs_table + s5k4ecgx_regs_table_size) - 1) = '\0';

	CAM_DEBUG("s5k4ecgx_reg_table_init");

	return;
}
#endif

#ifdef CONFIG_LOAD_FILE

void s5k4ecgx_regs_table_exit(void)
{
	CAM_DEBUG("%s %d", __func__, __LINE__);
	if (s5k4ecgx_regs_table) {
		vfree(s5k4ecgx_regs_table);
		s5k4ecgx_regs_table = NULL;
	}
}

#endif

#ifdef CONFIG_LOAD_FILE
static int s5k4ecgx_write_regs_from_sd(char *name)
{
	char *start, *end, *reg, *size;
	unsigned short addr;
	unsigned int len, value;
	char reg_buf[7], data_buf1[5], data_buf2[7];


	*(reg_buf + 6) = '\0';
	*(data_buf1 + 4) = '\0';
	*(data_buf2 + 6) = '\0';

	CAM_DEBUG("s5k4ecgx_regs_table_write start!");
	CAM_DEBUG("E string = %s", name);

	start = strstr(s5k4ecgx_regs_table, name);
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

			CAM_DEBUG("addr 0x%04x, value 0x%04x", addr, value);

			if (addr == 0xFFFF)
				msleep(value);
			else
				s5k4ecgx_i2c_write_multi(addr, value);

		}
	}
	CAM_DEBUG("s5k4ecgx_regs_table_write end!");

	return 0;
}

#endif

static DECLARE_WAIT_QUEUE_HEAD(s5k4ecgx_wait_queue);

/**
 * s5k4ecgx_i2c_read_multi: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 * @r_data: buffer where data is read
 * @r_len: number of bytes to read
 *
 * Returns 0 on success, <0 on error
 */

static int s5k4ecgx_i2c_read_multi(unsigned short subaddr, unsigned long *data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k4ecgx_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k4ecgx_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
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
 * s5k4ecgx_i2c_read: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @data: data to be read
 *
 * Returns 0 on success, <0 on error
 */
static int s5k4ecgx_i2c_read(unsigned short subaddr, unsigned short *data)
{
	unsigned char buf[2];
	struct i2c_msg msg = {s5k4ecgx_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k4ecgx_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;

	err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
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
 * s5k4ecgx_i2c_write_multi: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static int s5k4ecgx_i2c_write_multi(unsigned short addr, unsigned int w_data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k4ecgx_client->addr, 0, 4, buf};

	int retry_count = 5;
	int err = 0;

	if (!s5k4ecgx_client->adapter)
		return -EIO;

	buf[0] = addr >> 8;
	buf[1] = addr & 0xFF;
	buf[2] = w_data >> 8;
	buf[3] = w_data & 0xFF;
	/*
	 * Data should be written in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */

	while (retry_count--) {
		err  = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
	}
	return (err == 1) ? 0 : -EIO;
}

static int s5k4ecgx_i2c_wrt_list(struct s5k4ecgx_short_t regs[],
	int size, char *name)
{
#ifdef CONFIG_LOAD_FILE
	s5k4ecgx_write_regs_from_sd(name);
#else
	int err = 0;
	int i = 0;

	CAM_DEBUG("%s", name);

	if (!s5k4ecgx_client->adapter) {
		cam_err("Can't search i2c client adapter");
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		if (regs[i].subaddr == 0xFFFF) {
			msleep(regs[i].value);
			CAM_DEBUG("delay = 0x%04x, value = 0x%04x",
						regs[i].subaddr, regs[i].value);
		} else {
			err = s5k4ecgx_i2c_write_multi(regs[i].subaddr,
								regs[i].value);
			if (unlikely(err < 0)) {
				cam_err("register set failed");
				return -EIO;
				}
			}
		}
#endif

	return 0;
}


#ifdef FACTORY_TEST
struct class *sec_class;
struct device *s5k4ecgx_dev;

static ssize_t cameratype_file_cmd_show(struct device *dev,
				struct device_attribute *attr, char *buf) {
	char sensor_info[30] = "s5k4ecgx";
	return snprintf(buf, "%s\n", sensor_info);
}

static ssize_t cameratype_file_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
		/*Reserved*/
	return size;
}

static struct device_attribute s5k4ecgx_camtype_attr = {
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
		CAM_DEBUG("[Factory flash]OFF");
		s5k4ecgx_set_flash(MOVIE_FLASH, 0);
	} else {
		CAM_DEBUG("[Factory flash]ON");
		s5k4ecgx_set_flash(MOVIE_FLASH, 1);
	}
	return size;
}

static struct device_attribute s5k4ecgx_cameraflash_attr = {
	.attr = {
		.name = "cameraflash",
		.mode = (S_IRUGO | S_IWUGO)},
		.show = cameraflash_file_cmd_show,
		.store = cameraflash_file_cmd_store
};
#endif


void s5k4ecgx_set_preview_size(int32_t index)
{
	CAM_DEBUG("index %d", index);

	s5k4ecgx_ctrl->settings.preview_size_idx = index;
}


void s5k4ecgx_set_preview(void)
{
	CAM_DEBUG("cam_mode = %d", s5k4ecgx_ctrl->cam_mode);

	if (s5k4ecgx_ctrl->cam_mode == MOVIE_MODE) {
		CAM_DEBUG("Camcorder_Mode_ON");
		if (s5k4ecgx_ctrl->settings.preview_size_idx ==
				PREVIEW_SIZE_HD) {
			CAM_DEBUG("720P recording");
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_720p_common);*/
	} else {
			CAM_DEBUG("VGA recording");
			/*S5K4ECGX_WRT_LIST(s5k4ecgx_recording_common);*/
		}
	} else {
		CAM_DEBUG("Preview_Mode");
		if (s5k4ecgx_ctrl->op_mode == CAMERA_MODE_INIT)
			S5K4ECGX_WRT_LIST(s5k4ecgx_common);

		S5K4ECGX_WRT_LIST(s5k4ecgx_preview);
		s5k4ecgx_ctrl->op_mode = CAMERA_MODE_PREVIEW;
	}
}

void s5k4ecgx_set_capture(void)
{
	CAM_DEBUG("");
	s5k4ecgx_ctrl->op_mode = CAMERA_MODE_CAPTURE;
	/*(s5k4ecgx_capture);*/
}

static int32_t s5k4ecgx_sensor_setting(int update_type, int rt)
{
	CAM_DEBUG("Start");

	int32_t rc = 0;
	int temp = 0;
	struct msm_camera_csid_params s5k4ecgx_csid_params;
	struct msm_camera_csiphy_params s5k4ecgx_csiphy_params;
	switch (update_type) {
	case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE)
			/* Add some condition statements */
		break;

	case UPDATE_PERIODIC:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			CAM_DEBUG("UPDATE_PERIODIC");

			v4l2_subdev_notify(s5k4ecgx_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX_0, ISPIF_OFF_IMMEDIATELY));

			/* stop streaming */
			S5K4ECGX_WRT_LIST(s5k4ecgx_stream_stop);
			msleep(100);

/*			if (config_csi2 == 0) { */
				struct msm_camera_csid_vc_cfg
							s5k4ecgx_vccfg[] = {
					{0, 0x1E, CSI_DECODE_8BIT},
					/* {0, CSI_RAW10, CSI_DECODE_10BIT}, */
					{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
					};
			s5k4ecgx_csid_params.lane_cnt = 2;
			s5k4ecgx_csid_params.lane_assign = 0xe4;
			s5k4ecgx_csid_params.lut_params.num_cid =
				ARRAY_SIZE(s5k4ecgx_vccfg);
			s5k4ecgx_csid_params.lut_params.vc_cfg =
				&s5k4ecgx_vccfg[0];
			s5k4ecgx_csiphy_params.lane_cnt = 2;
				s5k4ecgx_csiphy_params.settle_cnt = 0x07;
			v4l2_subdev_notify(s5k4ecgx_ctrl->sensor_dev,
					NOTIFY_CSID_CFG, &s5k4ecgx_csid_params);
			v4l2_subdev_notify(s5k4ecgx_ctrl->sensor_dev,
					NOTIFY_CID_CHANGE, NULL);
			mb();
			v4l2_subdev_notify(s5k4ecgx_ctrl->sensor_dev,
					NOTIFY_CSIPHY_CFG,
					&s5k4ecgx_csiphy_params);
			mb();
				/*s5k4ecgx_delay_msecs_stdby*/
			msleep(350);
			config_csi2 = 1;
/*			}*/
			if (rc < 0)
				return rc;

			v4l2_subdev_notify(s5k4ecgx_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX_0, ISPIF_ON_FRAME_BOUNDARY));

			/*start stream*/
			S5K4ECGX_WRT_LIST(s5k4ecgx_preview);
			S5K4ECGX_WRT_LIST(s5k4ecgx_stream_on);
			msleep(100);
		}
		break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int32_t s5k4ecgx_video_config(int mode)
{
	int32_t	rc = 0;
	int	rt;

	if (s5k4ecgx_sensor_setting(UPDATE_PERIODIC, RES_PREVIEW) < 0)
		rc = -1;

	return rc;
}

static long s5k4ecgx_set_sensor_mode(int mode)
{
	CAM_DEBUG("%d", mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	case SENSOR_VIDEO_MODE:
		s5k4ecgx_set_preview();
		if (config_csi2 == 0)
			s5k4ecgx_video_config(mode);
		break;
	case SENSOR_SNAPSHOT_MODE:
	case SENSOR_RAW_SNAPSHOT_MODE:
		s5k4ecgx_set_capture();
		break;
	default:
		return 0;
	}
	return 0;
}

static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

static int s5k4ecgx_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	CAM_DEBUG("=== Start ===");

	int rc = 0;
	int temp = 0;
	int status = 0;
	int count = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

#ifdef CONFIG_LOAD_FILE
	s5k4ecgx_regs_table_init();
#endif

	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0)
		pr_err("%s: request gpio failed\n", __func__);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_5M_RST : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_5M_ISP_INIT : %d", temp);

	/*Power on the LDOs */
	data->sensor_platform_info->sensor_power_on(0);
	usleep(1000);

	/*Set Main clock */
	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 1,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);

	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;

	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0)
		pr_err("%s: clk enable failed\n", __func__);

	usleep(10);

	/*standby Main cam */
	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("[s5k4ecgx] CAM_5M_ISP_INIT : %d", temp);
	usleep(15);

	/*reset Main cam */
	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("[s5k4ecgx] CAM_5M_RST : %d", temp);
	usleep(50 * 1000);

	/* sensor validation test */
	CAM_DEBUG(" Camera Sensor Validation Test");
	rc = S5K4ECGX_WRT_LIST(s5k4ecgx_pre_common);
	if (rc < 0) {
		pr_info("Error in  Camera Sensor Validation Test");
		return rc;
	}

	config_csi2 = 0;
	s5k4ecgx_ctrl->op_mode = CAMERA_MODE_INIT;

	return rc;
}


static void s5k4ecgx_check_dataline(int val)
{
	if (val) {
		CAM_DEBUG("DTP ON");
		s5k4ecgx_ctrl->dtpTest = 1;

	} else {
		CAM_DEBUG("DTP OFF");
		s5k4ecgx_ctrl->dtpTest = 0;
	}
}

static int s5k4ecgx_set_effect(int effect)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d", __func__, effect);

	switch (effect) {
	case CAMERA_EFFECT_OFF:
		/*(s5k4ecgx_effect_none);*/
		break;

	case CAMERA_EFFECT_MONO:
		/*(s5k4ecgx_effect_mono);*/
		break;

	case CAMERA_EFFECT_NEGATIVE:
		/*(s5k4ecgx_effect_negative);*/
		break;

	case CAMERA_EFFECT_SEPIA:
		/*(s5k4ecgx_effect_sepia);*/
		break;

	case CAMERA_EFFECT_WHITEBOARD:
		/*(s5k4ecgx_effect_sketch);*/
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] default effect");
		/*(s5k4ecgx_effect_none);*/
		return 0;
	}

	return 0;
}

static int s5k4ecgx_set_whitebalance(int wb)
{
	switch (wb) {
	case CAMERA_WHITE_BALANCE_AUTO:
			/*(s5k4ecgx_wb_auto);*/
		break;

	case CAMERA_WHITE_BALANCE_INCANDESCENT:
			/*(s5k4ecgx_wb_incandescent);*/
		break;

	case CAMERA_WHITE_BALANCE_FLUORESCENT:
			/*(s5k4ecgx_wb_fluorescent);*/
		break;

	case CAMERA_WHITE_BALANCE_DAYLIGHT:
			/*(s5k4ecgx_wb_daylight);*/
		break;

	case CAMERA_WHITE_BALANCE_CLOUDY_DAYLIGHT:
			/*(s5k4ecgx_wb_cloudy);*/
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected WB mode %s/%d",
			__func__, __LINE__);
		return 0;
	}
	return 0;
}

static void s5k4ecgx_set_ev(int ev)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d", __func__, ev);

	switch (ev) {
	case CAMERA_EV_M4:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_M4);*/
		break;

	case CAMERA_EV_M3:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_M3);*/
		break;

	case CAMERA_EV_M2:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_M2);*/
		break;

	case CAMERA_EV_M1:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_M1);*/
		break;

	case CAMERA_EV_DEFAULT:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_default);*/
		break;

	case CAMERA_EV_P1:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_P1);*/
		break;

	case CAMERA_EV_P2:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_P2);*/
		break;

	case CAMERA_EV_P3:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_P3);*/
		break;

	case CAMERA_EV_P4:
		/*S5K4ECGX_WRT_LIST(s5k4ecgx_brightness_P4);*/
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected ev mode %s/%d",
			__func__, __LINE__);
		break;
	}
}

void sensor_native_control(void __user *arg)
{
	struct ioctl_native_cmd ctrl_info;

	if (copy_from_user((void *)&ctrl_info,
		(const void *)arg, sizeof(ctrl_info)))
		CAM_DEBUG(
			"[s5k4ecgx] %s fail copy_from_user!", __func__);

	switch (ctrl_info.mode) {
	case EXT_CAM_EV:
		s5k4ecgx_set_ev(ctrl_info.value_1);
		break;

	case EXT_CAM_EFFECT:
		s5k4ecgx_set_effect(ctrl_info.value_1);
		break;

	case EXT_CAM_WB:
		s5k4ecgx_set_whitebalance(ctrl_info.value_1);
		break;

	case EXT_CAM_DTP_TEST:
		s5k4ecgx_check_dataline(ctrl_info.value_1);
		break;

	case  EXT_CAM_MOVIE_MODE:
		CAM_DEBUG("MOVIE mode : %d", ctrl_info.value_1);
		s5k4ecgx_ctrl->cam_mode = ctrl_info.value_1;
		break;

	case EXT_CAM_PREVIEW_SIZE:
		s5k4ecgx_set_preview_size(ctrl_info.value_1);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] default mode");
		break;
	}

	if (copy_to_user((void *)arg,
		(const void *)&ctrl_info, sizeof(ctrl_info)))
		CAM_DEBUG("[s5k4ecgx] %s fail copy_to_user!", __func__);
}

long s5k4ecgx_sensor_subdev_ioctl(struct v4l2_subdev *sd,
			unsigned int cmd, void *arg)
{
	void __user *argp = (void __user *)arg;
	CAM_DEBUG("s5k4ecgx_sensor_subdev_ioctl\n");
	switch (cmd) {
	case VIDIOC_MSM_SENSOR_CFG:
		return s5k4ecgx_sensor_config(argp);
	default:
		return -ENOIOCTLCMD;
	}
}

int s5k4ecgx_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data, (void *)argp,
						sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	CAM_DEBUG(" cfgtype = %d, mode = %d",
			cfg_data.cfgtype, cfg_data.mode);
		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = s5k4ecgx_set_sensor_mode(cfg_data.mode);
			break;

		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = 0;
			cam_err(" Invalid cfgtype = %d", cfg_data.cfgtype);
			break;
		}
	return rc;
}

static int s5k4ecgx_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG("=== Start ===");

#ifdef CONFIG_LOAD_FILE
	s5k4ecgx_regs_table_exit();
#endif

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_5M_RST : %d", temp);
	usleep(50 * 1000);

	/*CAM_MCLK0*/
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);

	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_5M_ISP_INIT : %d", temp);

	data->sensor_platform_info->sensor_power_off(0);

	msm_camera_request_gpio_table(data, 0);

	return rc;
}

static struct s5k4ecgx_format s5k4ecgx_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static int s5k4ecgx_enum_fmt(struct v4l2_subdev *sd, unsigned int index,
			   enum v4l2_mbus_pixelcode *code) {
	CAM_DEBUG("Index is %d", index);
	if ((unsigned int)index >= ARRAY_SIZE(s5k4ecgx_subdev_info))
		return -EINVAL;

	*code = s5k4ecgx_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops s5k4ecgx_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = s5k4ecgx_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k4ecgx_subdev_video_ops = {
	.enum_mbus_fmt = s5k4ecgx_enum_fmt,
};

static struct v4l2_subdev_ops s5k4ecgx_subdev_ops = {
	.core = &s5k4ecgx_subdev_core_ops,
	.video  = &s5k4ecgx_subdev_video_ops,
};

static int s5k4ecgx_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;

	cam_err("%s_i2c_probe called", client->name);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		cam_err("i2c_check_functionality failed\n");
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
		cam_err("s_ctrl->sensor_i2c_client is NULL\n");
		rc = -EFAULT;
		return rc;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s: NULL sensor data\n", __func__);
		return -EFAULT;
	}

	s5k4ecgx_client = client;
	s5k4ecgx_dev = s_ctrl->sensor_i2c_client->client->dev;

	s5k4ecgx_ctrl = kzalloc(sizeof(struct s5k4ecgx_ctrl), GFP_KERNEL);
	if (!s5k4ecgx_ctrl) {
		CAM_DEBUG("s5k4ecgx_ctrl alloc failed!\n");
		return -ENOMEM;
	}

	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);

	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		&s5k4ecgx_subdev_ops);

	s5k4ecgx_ctrl->sensor_dev = &s_ctrl->sensor_v4l2_subdev;
	s5k4ecgx_ctrl->sensordata = client->dev.platform_data;

	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);

	cam_err("s5k4ecgx_probe succeeded!");
	return 0;

probe_failure:
	cam_err("s5k4ecgx_probe failed!");
	return rc;
}

static const struct i2c_device_id s5k4ecgx_i2c_id[] = {
	{"s5k4ecgx", (kernel_ulong_t)&s5k4ecgx_s_ctrl},
	{},
};

static struct msm_camera_i2c_client s5k4ecgx_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct i2c_driver s5k4ecgx_i2c_driver = {
	.id_table = s5k4ecgx_i2c_id,
	.probe  = s5k4ecgx_i2c_probe,
	.driver = {
		.name = "s5k4ecgx",
	},
};

static int __init s5k4ecgx_init(void)
{
	return i2c_add_driver(&s5k4ecgx_i2c_driver);
}

static struct msm_sensor_fn_t s5k4ecgx_func_tbl = {
	.sensor_config = s5k4ecgx_sensor_config,
	.sensor_power_up = s5k4ecgx_sensor_power_up,
	.sensor_power_down = s5k4ecgx_sensor_power_down,
};


static struct msm_sensor_reg_t s5k4ecgx_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_sensor_ctrl_t s5k4ecgx_s_ctrl = {
	.msm_sensor_reg = &s5k4ecgx_regs,
	.sensor_i2c_client = &s5k4ecgx_sensor_i2c_client,
	.sensor_i2c_addr = 0xAC>>1,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &s5k4ecgx_mut,
	.sensor_i2c_driver = &s5k4ecgx_i2c_driver,
	.sensor_v4l2_subdev_info = s5k4ecgx_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k4ecgx_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k4ecgx_subdev_ops,
	.func_tbl = &s5k4ecgx_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};


module_init(s5k4ecgx_init);
