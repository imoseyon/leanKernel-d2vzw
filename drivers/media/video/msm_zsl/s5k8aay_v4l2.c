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



#endif

#define S5K8_WRT_LIST(A) s5k8_i2c_wrt_list(A, (sizeof(A) / sizeof(A[0])), #A);

#define CAM_5M_RST		107
#define CAM_5M_ISP_INIT	4
#define CAM_1_3M_RST	76
#define CAM_1_3M_EN		18
#define CAM_MCLK		5
#define CAM_I2C_SDA		20
#define CAM_I2C_SCL		21
#define CAM_REV ((system_rev <= 1) ? 0 : 1)
#ifdef CAM_REV
	#include "s5k8aay_regs_v2.h"
#else
	#include "s5k8aay_regs.h"
#endif

struct s5k8aay_work {
	struct work_struct work;
};

static struct  s5k8aay_work *s5k8aay_sensorw;
static struct  i2c_client *s5k8aay_client;

struct s5k8aay_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
	uint32_t sensormode;
	uint32_t fps_divider;		/* init to 1 * 0x00000400 */
	uint32_t pict_fps_divider;	/* init to 1 * 0x00000400 */
	uint16_t fps;
	int16_t curr_lens_pos;
	uint16_t curr_step_pos;
	uint16_t my_reg_gain;
	uint32_t my_reg_line_count;
	uint16_t total_lines_per_frame;
	enum s5k8aay_resolution_t prev_res;
	enum s5k8aay_resolution_t pict_res;
	enum s5k8aay_resolution_t curr_res;
	enum s5k8aay_test_mode_t set_test;
	unsigned short imgaddr;

	struct v4l2_subdev *sensor_dev;
	struct s5k8aay_format *fmt;
};

struct s5k8aay_ctrl {
	const struct msm_camera_sensor_info *sensordata;

	struct s5k8aay_userset settings;
	struct v4l2_subdev *sensor_dev;

	int op_mode;
	int dtp_mode;
	int app_mode;
	int vtcall_mode;
	int started;
	int dtpTest;
	int isCapture;
};

static unsigned int config_csi2;
static struct s5k8aay_ctrl *s5k8aay_ctrl;

struct s5k8aay_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};


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

	CAM_DEBUG("%s %d\n", __func__, __LINE__);

	/*Set the current segment to kernel data segment */
	set_fs(get_ds());

	filp = filp_open("/mnt/sdcard/s5k8aay_regs.h", O_RDONLY, 0);

	if (IS_ERR_OR_NULL(filp)) {
		cam_err("file open error\n");
		return ;
	}

	lsize = filp->f_path.dentry->d_inode->i_size;
	CAM_DEBUG("size : %ld\n", lsize);
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

	s5k8aay_regs_table = dp;

	s5k8aay_regs_table_size = lsize;

	*((s5k8aay_regs_table + s5k8aay_regs_table_size) - 1) = '\0';

	CAM_DEBUG("s5k8aay_reg_table_init");

	return;
}
#endif

#ifdef CONFIG_LOAD_FILE

void s5k8aay_regs_table_exit(void)
{
	CAM_DEBUG("%s %d", __func__, __LINE__);
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

	CAM_DEBUG("s5k8aay_regs_table_write start!\n");
	CAM_DEBUG("E string = %s\n", name);

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

			CAM_DEBUG("addr 0x%04x, value 0x%04x\n", addr, value);

			if (addr == 0xFFFF)
				msleep(value);
			else
				s5k8aay_i2c_write_multi(addr, value);

		}
	}
	CAM_DEBUG("s5k8aay_regs_table_write end!");

	return 0;
}

#endif

static DECLARE_WAIT_QUEUE_HEAD(s5k8aay_wait_queue);

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

	if (!s5k8aay_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
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

	if (!s5k8aay_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;

	err = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
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

	if (!s5k8aay_client->adapter)
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
		err  = i2c_transfer(s5k8aay_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
	}
	return (err == 1) ? 0 : -EIO;
}

static int s5k8_i2c_wrt_list(struct s5k8aay_short_t regs[],
	int size, char *name)
{
#ifdef CONFIG_LOAD_FILE
	s5k8aay_write_regs_from_sd(name);
#else
	int err = 0;
	int i = 0;

	CAM_DEBUG("");

	if (!s5k8aay_client->adapter) {
		cam_err("Can't search i2c client adapter");
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		if (regs[i].subaddr == 0xFFFF) {
			msleep(regs[i].value);
			CAM_DEBUG("delay = 0x%04x, value = 0x%04x\n",
						regs[i].subaddr, regs[i].value);
		} else {
			err = s5k8aay_i2c_write_multi(regs[i].subaddr,
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
		CAM_DEBUG("[Factory flash]OFF");
		s5k8aay_set_flash(MOVIE_FLASH, 0);
	} else {
		CAM_DEBUG("[Factory flash]ON");
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

void s5k8aay_set_preview(void)
{

}

void s5k8aay_set_capture(void)
{
	CAM_DEBUG("");
	S5K8_WRT_LIST(s5k8aay_capture);
}

static int32_t s5k8aay_sensor_setting(int update_type, int rt)
{
	CAM_DEBUG("Start");

	int32_t rc = 0;
	int temp = 0;
	struct msm_camera_csid_params s5k8aay_csid_params;
	struct msm_camera_csiphy_params s5k8aay_csiphy_params;
	switch (update_type) {
	case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE)
			/* Add some condition statements */
		break;

	case UPDATE_PERIODIC:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			CAM_DEBUG("UPDATE_PERIODIC");

			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX0, ISPIF_OFF_IMMEDIATELY));

			/* stop streaming */
			S5K8_WRT_LIST(s5k8aay_stream_stop);
			msleep(100);

			if (config_csi2 == 0) {
				struct msm_camera_csid_vc_cfg
							s5k8aay_vccfg[] = {
					{0, 0x1E, CSI_DECODE_8BIT},
					/* {0, CSI_RAW10, CSI_DECODE_10BIT}, */
					{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
					};
			s5k8aay_csid_params.lane_cnt = 1;
			s5k8aay_csid_params.lane_assign = 0xe4;
			s5k8aay_csid_params.lut_params.num_cid =
				ARRAY_SIZE(s5k8aay_vccfg);
			s5k8aay_csid_params.lut_params.vc_cfg =
				&s5k8aay_vccfg[0];
			s5k8aay_csiphy_params.lane_cnt = 1;
			if (system_rev <= 1)
				s5k8aay_csiphy_params.settle_cnt = 0x07;
			else
				s5k8aay_csiphy_params.settle_cnt = 0x1B;
			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
					NOTIFY_CSID_CFG, &s5k8aay_csid_params);
			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
					NOTIFY_CID_CHANGE, NULL);
			mb();
			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
					NOTIFY_CSIPHY_CFG, &s5k8aay_csiphy_params);
			mb();
				/*s5k8aay_delay_msecs_stdby*/
			msleep(350);
			config_csi2 = 1;
			}
			if (rc < 0)
				return rc;

			v4l2_subdev_notify(s5k8aay_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX0, ISPIF_ON_FRAME_BOUNDARY));

			/*start stream*/
			S5K8_WRT_LIST(s5k8aay_preview);
			msleep(100);
		}
		break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int32_t s5k8aay_video_config(int mode)
{
	CAM_DEBUG("=== Start ===");

	int32_t	rc = 0;
	int	rt;
	/* change sensor resolution	if needed */
	/* if (s5k8aay_ctrl->prev_res == QTR_SIZE) */
		rt = RES_PREVIEW;
	/* else */
		/* rt = RES_CAPTURE; */

	if (s5k8aay_sensor_setting(UPDATE_PERIODIC, rt) < 0)
		return rc;
/*	s5k8aay_ctrl->curr_res = s5k8aay_ctrl->prev_res;
	s5k8aay_ctrl->sensormode = mode; */
	return rc;
}

static long s5k8aay_set_sensor_mode(int mode)
{
	CAM_DEBUG("=== Start ===");

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	case SENSOR_VIDEO_MODE:
		if (s5k8aay_ctrl->isCapture == 1) {
			s5k8aay_ctrl->isCapture = 0;
			S5K8_WRT_LIST(s5k8aay_preview);
		}
		s5k8aay_video_config(mode);
		break;
	case SENSOR_SNAPSHOT_MODE:
		s5k8aay_set_capture();
		break;
	case SENSOR_RAW_SNAPSHOT_MODE:
		s5k8aay_set_capture();
		break;
	default:
		return 0;
	}
	return 0;
}

static int s5k8aay_sensor_pre_init(const struct msm_camera_sensor_info *data)
{
	CAM_DEBUG("== Start ==");

	int rc = 0;

#ifndef CONFIG_LOAD_FILE
	rc = S5K8_WRT_LIST(s5k8aay_common);
	if (rc < 0)
		CAM_DEBUG("Fail!");
#endif
	usleep(10*1000);

	return rc;
}


static int s5k8aay_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	CAM_DEBUG("=== Start ===");

	int rc = 0;
	int temp = 0;
	int status = 0;
	int count = 0;
	gpio_tlmm_config(GPIO_CFG(CAM_1_3M_RST, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(CAM_1_3M_EN, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);

	gpio_set_value_cansleep(CAM_1_3M_RST, 0);
	temp = gpio_get_value(CAM_1_3M_RST);
	CAM_DEBUG("CAM_1_3M_RST : %d", temp);

	gpio_set_value_cansleep(CAM_1_3M_EN, 0);
	temp = gpio_get_value(CAM_1_3M_EN);
	CAM_DEBUG("CAM_1_3M_EN : %d", temp);

	data->sensor_platform_info->sensor_power_on(1);
	usleep(1000);

	gpio_set_value_cansleep(CAM_1_3M_EN, 1);
	temp = gpio_get_value(CAM_1_3M_EN);
	CAM_DEBUG("CAM_1_3M_EN : %d", temp);
	usleep(1000);

	gpio_tlmm_config(GPIO_CFG(CAM_MCLK, 1, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	msm_camio_clk_rate_set(24000000);
	usleep(10*1000);

	gpio_set_value_cansleep(CAM_1_3M_RST, 1);
	temp = gpio_get_value(CAM_1_3M_RST);
	CAM_DEBUG("CAM_1_3M_RST : %d", temp);

	usleep(100*1000);

	S5K8_WRT_LIST(s5k8aay_common);
	S5K8_WRT_LIST(s5k8aay_preview);

	return rc;
}

int s5k8aay_sensor_init(const struct msm_camera_sensor_info *data)
{
	CAM_DEBUG("=== Start ===");

	int rc = 0;
/*	s5k8aay_ctrl = kzalloc(sizeof(struct s5k8aay_ctrl), GFP_KERNEL); */
	if (!s5k8aay_ctrl) {
		cam_err("Failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		s5k8aay_ctrl->sensordata = data;

	config_csi2 = 0;
#ifdef CONFIG_LOAD_FILE
	s5k8aay_regs_table_init();
#endif

	rc = s5k8aay_sensor_init_probe(data);
	if (rc < 0) {
		cam_err("Failed!");
		goto init_fail;
	}

init_done:
	return rc;

init_fail:
	kfree(s5k8aay_ctrl);
	return rc;
}

static int s5k8aay_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&s5k8aay_wait_queue);
	return 0;
}

static void s5k8aay_check_dataline(int val)
{
	if (val) {
		printk(KERN_DEBUG "DTP ON\n");
		s5k8aay_ctrl->dtpTest = 1;

	} else {
		printk(KERN_DEBUG "DTP OFF\n");
		s5k8aay_ctrl->dtpTest = 0;
	}
}

static int s5k8aay_set_effect(int effect)
{
	printk(KERN_DEBUG "[s5k8aay] %s : %d\n", __func__, effect);

	switch (effect) {
	case CAMERA_EFFECT_OFF:
		S5K8_WRT_LIST(s5k8aay_effect_none);
		break;

	case CAMERA_EFFECT_MONO:
		S5K8_WRT_LIST(s5k8aay_effect_mono);
		break;

	case CAMERA_EFFECT_NEGATIVE:
		S5K8_WRT_LIST(s5k8aay_effect_negative);
		break;

	case CAMERA_EFFECT_SEPIA:
		S5K8_WRT_LIST(s5k8aay_effect_sepia);
		break;

	case CAMERA_EFFECT_WHITEBOARD:
		S5K8_WRT_LIST(s5k8aay_effect_sketch);
		break;

	default:
		printk(KERN_DEBUG "[s5k8aay] default effect\n");
		S5K8_WRT_LIST(s5k8aay_effect_none);
		return 0;
	}

	return 0;
}

static int s5k8aay_set_whitebalance(int wb)
{
	switch (wb) {
	case CAMERA_WHITE_BALANCE_AUTO:
			S5K8_WRT_LIST(s5k8aay_wb_auto);
		break;

	case CAMERA_WHITE_BALANCE_INCANDESCENT:
			S5K8_WRT_LIST(s5k8aay_wb_incandescent);
		break;

	case CAMERA_WHITE_BALANCE_FLUORESCENT:
			S5K8_WRT_LIST(s5k8aay_wb_fluorescent);
		break;

	case CAMERA_WHITE_BALANCE_DAYLIGHT:
			S5K8_WRT_LIST(s5k8aay_wb_daylight);
		break;

	case CAMERA_WHITE_BALANCE_CLOUDY_DAYLIGHT:
			S5K8_WRT_LIST(s5k8aay_wb_cloudy);
		break;

	default:
		printk(KERN_DEBUG "[s5k8aay] unexpected WB mode %s/%d\n",
			__func__, __LINE__);
		return 0;
	}
	return 0;
}

static void s5k8aay_set_ev(int ev)
{
	printk(KERN_DEBUG "[s5k8aay] %s : %d\n", __func__, ev);

	switch (ev) {
	case CAMERA_EV_M2:
		S5K8_WRT_LIST(s5k8aay_brightness_M4);
		break;

	case CAMERA_EV_M1:
		S5K8_WRT_LIST(s5k8aay_brightness_M2);
		break;

	case CAMERA_EV_DEFAULT:
		S5K8_WRT_LIST(s5k8aay_brightness_default);
		break;

	case CAMERA_EV_P1:
		S5K8_WRT_LIST(s5k8aay_brightness_P2);
		break;

	case CAMERA_EV_P2:
		S5K8_WRT_LIST(s5k8aay_brightness_P4);
		break;

	default:
		printk(KERN_DEBUG "[s5k8aay] unexpected ev mode %s/%d\n",
			__func__, __LINE__);
		break;
	}
}
void sensor_native_control_front(void __user *arg)
{
	struct ioctl_native_cmd ctrl_info;

	printk(KERN_DEBUG "[s5k8aa] %s/%d\n", __func__, __LINE__);

	if (copy_from_user((void *)&ctrl_info,
		(const void *)arg, sizeof(ctrl_info)))
		printk(KERN_DEBUG
			"[s5k8aa] %s fail copy_from_user!\n", __func__);

	printk(KERN_DEBUG "[s5k8aa] %d %d %d %d %d\n",
		ctrl_info.mode, ctrl_info.address, ctrl_info.value_1,
		ctrl_info.value_2, ctrl_info.value_3);

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

	default:
		printk(KERN_DEBUG "[s5k8aa] default mode\n");
		break;
	}

	if (copy_to_user((void *)arg,
		(const void *)&ctrl_info, sizeof(ctrl_info)))
		printk(KERN_DEBUG "[s5k8aa] %s fail copy_to_user!\n", __func__);
}

int s5k8aay_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data, (void *)argp,
						sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	CAM_DEBUG(" cfgtype = %d, mode = %d\n",
			cfg_data.cfgtype, cfg_data.mode);
		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = s5k8aay_set_sensor_mode(cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
/* rc = s5k8aay_set_effect(cfg_data.mode, cfg_data.cfg.effect); */
			break;

		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = 0;
			cam_err(" Invalid cfgtype = %d", cfg_data.cfgtype);
			break;
		}
	return rc;
}

int s5k8aay_sensor_release(void)
{
	int rc = 0;

	CAM_DEBUG("=== Start ===");

#ifdef CONFIG_LOAD_FILE
	s5k8aay_regs_table_exit();
#endif
	gpio_set_value_cansleep(CAM_1_3M_RST, 0);
	usleep(1000);
	gpio_set_value_cansleep(CAM_1_3M_EN, 0);
	usleep(1000);
	s5k8aay_ctrl->sensordata->sensor_platform_info->sensor_power_off(1);
	return rc;
}

static int s5k8aay_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	CAM_DEBUG("=== Start ===");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		cam_err("=== i2c Probe error ===");
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	s5k8aay_sensorw =
		kzalloc(sizeof(struct s5k8aay_work), GFP_KERNEL);

	if (!s5k8aay_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, s5k8aay_sensorw);
	s5k8aay_init_client(client);
	s5k8aay_client = client;

	CAM_DEBUG("s5k8aay_probe succeeded!");

	return 0;

probe_failure:
	kfree(s5k8aay_sensorw);
	s5k8aay_sensorw = NULL;
	cam_err("s5k8aay_probe failed!");
	return rc;
}

static const struct i2c_device_id s5k8aay_i2c_id[] = {
	{ "s5k8aay", 0},
	{ },
};

static struct i2c_driver s5k8aay_i2c_driver = {
	.id_table = s5k8aay_i2c_id,
	.probe  = s5k8aay_i2c_probe,
	.remove = __exit_p(s5k8aay_i2c_remove),
	.driver = {
		.name = "s5k8aay",
	},
};

static int s5k8aay_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s) {
	CAM_DEBUG("== Start ==");
	int rc = i2c_add_driver(&s5k8aay_i2c_driver);

	if (rc < 0 || s5k8aay_client == NULL) {
		CAM_DEBUG("rc = %d, s5k8aay_client = %d", rc,
			s5k8aay_client);

		rc = -ENOTSUPP;
		goto probe_done;
	}

	msm_camio_clk_rate_set(24000000);

	s->s_init = s5k8aay_sensor_init;
	s->s_release = s5k8aay_sensor_release;
	s->s_config  = s5k8aay_sensor_config;
	s->s_camera_type = FRONT_CAMERA_2D;
	s->s_mount_angle = 270;		/* HC - 0/180, GB - 90/270 */


probe_done:
	CAM_DEBUG("=== DONE ===");
	return rc;
}

static struct s5k8aay_format s5k8aay_subdev_info[] = {
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
	CAM_DEBUG("Index is %d", index);
	if ((unsigned int)index >= ARRAY_SIZE(s5k8aay_subdev_info))
		return -EINVAL;

	*code = s5k8aay_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops s5k8aay_subdev_core_ops;
static struct v4l2_subdev_video_ops s5k8aay_subdev_video_ops = {
	.enum_mbus_fmt = s5k8aay_enum_fmt,
};

static struct v4l2_subdev_ops s5k8aay_subdev_ops = {
	.core = &s5k8aay_subdev_core_ops,
	.video  = &s5k8aay_subdev_video_ops,
};

static int s5k8aay_sensor_probe_cb(const struct msm_camera_sensor_info *info,
	struct v4l2_subdev *sdev, struct msm_sensor_ctrl *s) {

	int rc = 0;
	CAM_DEBUG("=== Start ===");
	rc = s5k8aay_sensor_probe(info, s);

	if (rc < 0)
		return rc;

	s5k8aay_ctrl = kzalloc(sizeof(struct s5k8aay_ctrl_t), GFP_KERNEL);
	if (!s5k8aay_ctrl) {
		cam_err("s5k8aay_sensor_probe failed!");
		return -ENOMEM;
		}

	/* probe is successful, init a v4l2 subdevice */
	CAM_DEBUG("going into v4l2_i2c_subdev_init");
	if (sdev) {
		v4l2_i2c_subdev_init(sdev, s5k8aay_client, &s5k8aay_subdev_ops);
		s5k8aay_ctrl->sensor_dev = sdev;
		}
	else
		cam_err("sdev is null in probe_cb");

	return rc;
}

static int __s5k8aay_probe(struct platform_device *pdev)
{
	CAM_DEBUG("=== s5k8aay probe ===");

	return msm_sensor_register(pdev, s5k8aay_sensor_probe_cb);
}

static struct platform_driver msm_camera_driver = {
	.probe = __s5k8aay_probe,
	.driver = {
		.name = "msm_camera_s5k8aay",
		.owner = THIS_MODULE,
	},
};

static int __init s5k8aay_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(s5k8aay_init);
