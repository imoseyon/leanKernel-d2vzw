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
#include "s5k6aa.h"
#include "s5k6aa_regs.h"

#include "cam_pmic_s5k6aa.h"


//#define CONFIG_LOAD_FILE


#define S5K6AA_WRITE_LIST(A)			s5k6aa_i2c_write_list(A,(sizeof(A) / sizeof(A[0])),#A);

#define CAM_5M_RST		107
#define CAM_5M_ISP_INIT	4
#define CAM_1_3M_RST 	76
#define CAM_1_3M_EN		18
#define CAM_MCLK			5
#define CAM_I2C_SDA		20
#define CAM_I2C_SCL		21


struct s5k6aa_work {
	struct work_struct work;
};

static struct  s5k6aa_work *s5k6aa_sensorw;
static struct  i2c_client *s5k6aa_client;
#if 1
struct s5k6aa_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
	uint32_t sensormode;
	uint32_t fps_divider;/* init to 1 * 0x00000400 */
	uint32_t pict_fps_divider;/* init to 1 * 0x00000400 */
	uint16_t fps;
	int16_t curr_lens_pos;
	uint16_t curr_step_pos;
	uint16_t my_reg_gain;
	uint32_t my_reg_line_count;
	uint16_t total_lines_per_frame;
	enum s5k6aa_resolution_t prev_res;
	enum s5k6aa_resolution_t pict_res;
	enum s5k6aa_resolution_t curr_res;
	enum s5k6aa_test_mode_t set_test;
	unsigned short imgaddr;

	struct v4l2_subdev *sensor_dev;
	struct s5k6aa_format *fmt;
};
#endif

struct s5k6aa_ctrl {
	const struct msm_camera_sensor_info *sensordata;
	struct s5k6aa_userset settings;

	struct v4l2_subdev *sensor_dev;

	int op_mode;
	int dtp_mode;
	int app_mode;	// camera or camcorder
	int vtcall_mode;
	int started;
};

static unsigned int config_csi2;
static struct s5k6aa_ctrl *s5k6aa_ctrl;

struct s5k6aa_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};

static DECLARE_WAIT_QUEUE_HEAD(s5k6aa_wait_queue);

/**
 * s5k6aa_i2c_read_multi: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 * @r_data: buffer where data is read
 * @r_len: number of bytes to read
 *
 * Returns 0 on success, <0 on error
 */

#if 1
static int s5k6aa_i2c_read_multi(unsigned short subaddr, unsigned long *data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k6aa_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k6aa_client->adapter) {
		//dev_err(&s5k6aa_client->dev, "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	buf[0] = subaddr>> 8;
	buf[1] = subaddr & 0xff;

	err = i2c_transfer(s5k6aa_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&s5k6aa_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
		return -EIO;
	}

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(s5k6aa_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&s5k6aa_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
		return -EIO;
	}

	/*
	 * Data comes in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */
	*data = *(unsigned long *)(&buf);

	return err;
}


/**
 * s5k6aa_i2c_read: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @data: data to be read
 *
 * Returns 0 on success, <0 on error
 */
static int s5k6aa_i2c_read(unsigned short subaddr, unsigned short *data)
{
	unsigned char buf[2];
	struct i2c_msg msg = {s5k6aa_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k6aa_client->adapter) {
		//dev_err(&s5k6aa_client->dev, "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	buf[0] = subaddr>> 8;
	buf[1] = subaddr & 0xff;

	err = i2c_transfer(s5k6aa_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&s5k6aa_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
		return -EIO;
	}

	msg.flags = I2C_M_RD;

	err = i2c_transfer(s5k6aa_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&s5k6aa_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
		return -EIO;
	}

	/*
	 * Data comes in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */
	*data = *(unsigned short *)(&buf);

	return err;
}

#endif
/**
 * s5k6aa_i2c_write_multi: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static int s5k6aa_i2c_write_multi(unsigned short addr, unsigned int w_data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k6aa_client->addr, 0, 4, buf};

	int retry_count = 5;
	int err = 0;

	if (!s5k6aa_client->adapter) {
		//dev_err(&s5k6aa_client->dev, "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	buf[0] = addr >> 8;
	buf[1] = addr & 0xff;
	buf[2] = w_data >> 8;
	buf[3] = w_data & 0xff;
	/*
	 * Data should be written in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */

	while(retry_count--) {
		err  = i2c_transfer(s5k6aa_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
//		msleep(POLL_TIME_MS);
	}

	return (err == 1) ? 0 : -EIO;
}

static int s5k6aa_i2c_write_list(s5k6aa_short_t regs[], int size, char *name)
{
#ifdef CONFIG_LOAD_FILE
	s5k6aa_regs_table_write(client, name);
#else
	int err = 0;
	int i = 0;

	if (!s5k6aa_client->adapter) {
		printk(KERN_ERR "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		if(regs[i].subaddr == 0xFFFE)
		{
		    msleep(regs[i].value);
                    printk("delay 0x%04x, value 0x%04x\n", regs[i].subaddr, regs[i].value);
		}
                else
                 {
        		err = s5k6aa_i2c_write_multi(regs[i].subaddr, regs[i].value);

        		if (unlikely(err < 0)) {
        			printk(KERN_ERR "%s: register set failed\n",  __func__);
        			return -EIO;
        		}
                }
	}
#endif

	return 0;
}


//////////////////////////////////////////////////////////////
#ifdef FACTORY_TEST
extern struct class *sec_class;
struct device *s5k6aa_dev;

static ssize_t cameratype_file_cmd_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char sensor_info[30] = "s5k6aa";
	return sprintf(buf, "%s\n", sensor_info);
}

static ssize_t cameratype_file_cmd_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t size)
{
		/*Reserved*/
	return size;
}

//static DEVICE_ATTR(cameratype, 0666, cameratype_file_cmd_show, cameratype_file_cmd_store);
static struct device_attribute s5k6aa_camtype_attr = {
	.attr = {
		.name = "camtype",
		.mode = (S_IRUGO | S_IWUGO)},
	.show = cameratype_file_cmd_show,
	.store = cameratype_file_cmd_store
};

static ssize_t cameraflash_file_cmd_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
		/*Reserved*/
	return 0;
}

static ssize_t cameraflash_file_cmd_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t size)
{
	int value;

	sscanf(buf, "%d", &value);

	if (value == 0) {
		printk(KERN_INFO "[Factory flash]OFF\n");
		s5k6aa_set_flash(MOVIE_FLASH,0);
	} else {
		printk(KERN_INFO "[Factory flash]ON\n");
		s5k6aa_set_flash(MOVIE_FLASH,1);
	}

	return size;
}

//static DEVICE_ATTR(cameraflash, 0666, cameraflash_file_cmd_show, cameraflash_file_cmd_store);
static struct device_attribute s5k6aa_cameraflash_attr = {
	.attr = {
		.name = "cameraflash",
		.mode = (S_IRUGO | S_IWUGO)},
	.show = cameraflash_file_cmd_show,
	.store = cameraflash_file_cmd_store
};
#endif
//////////////////////////////////////////////////////////////

void s5k6aa_set_preview(void)
{

}

void s5k6aa_set_capture(void)
{
	S5K6AA_WRITE_LIST(mode_capture_1280x960);
}

static int32_t s5k6aa_sensor_setting(int update_type, int rt)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);

	int32_t rc = 0;
	int temp = 0;
	int32_t Cnt = 0; //Kelly's Test
	struct msm_camera_csid_params s5k6aa_csid_params;
	struct msm_camera_csiphy_params s5k6aa_csiphy_params;
	switch (update_type) {
	case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE)
		{
		}
		break;

	case UPDATE_PERIODIC:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			printk("[s5k6aa] UPDATE_PERIODIC\n");

			/* stop streaming */
			gpio_set_value_cansleep(CAM_1_3M_EN, 0);
			temp = gpio_get_value(CAM_1_3M_EN);
			printk("[s5k6aa] CAM_1_3M_EN : %d\n", temp);
			msleep(100);

#if 0//For Test
			//Start polling CSIPHY interrupt status register here
			do
			{
				printk("[s5k6aa] Kelly Test %d\n", Cnt);

				msm_io_read_interrupt();
			}while(Cnt++<5);
#endif
			if (config_csi2 == 0) {
				struct msm_camera_csid_vc_cfg s5k6aa_vccfg[] = {
					{0, 0x1E, CSI_DECODE_8BIT},
//					{0, CSI_RAW10, CSI_DECODE_10BIT},
					{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
				};
				s5k6aa_csid_params.lane_cnt = 1;
				s5k6aa_csid_params.lane_assign = 0xe4;
				s5k6aa_csid_params.lut_params.num_cid =
					ARRAY_SIZE(s5k6aa_vccfg);
				s5k6aa_csid_params.lut_params.vc_cfg =
					&s5k6aa_vccfg[0];
				s5k6aa_csiphy_params.lane_cnt = 1;
				s5k6aa_csiphy_params.settle_cnt = 0x07;//0x1B;
				rc = msm_camio_csid_config(&s5k6aa_csid_params);
				v4l2_subdev_notify(s5k6aa_ctrl->sensor_dev,
						NOTIFY_CID_CHANGE, NULL);
				mb();
				rc = msm_camio_csiphy_config
					(&s5k6aa_csiphy_params);
				mb();
				/*s5k6aa_delay_msecs_stdby*/
				msleep(350);
				config_csi2 = 1;
			}
			if (rc < 0)
				return rc;
			/*start stream*/
			gpio_set_value_cansleep(CAM_1_3M_EN, 1);
			temp = gpio_get_value(CAM_1_3M_EN);
			printk("[s5k6aa] CAM_1_3M_EN : %d\n", temp);
			msleep(100);
#if 0//For Test
			//Start polling CSIPHY interrupt status register here
			do
			{
				printk("[s5k6aa] Kelly Test %d\n", Cnt);

				msm_io_read_interrupt();
			}while(Cnt++<10);
#endif
		}
		break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int32_t s5k6aa_video_config(int mode)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);

	int32_t	rc = 0;
	int	rt;
	/* change sensor resolution	if needed */
//	if (s5k6aa_ctrl->prev_res == QTR_SIZE)
		rt = RES_PREVIEW;
//	else
//		rt = RES_CAPTURE;

	if (s5k6aa_sensor_setting(UPDATE_PERIODIC, rt) < 0)
		return rc;
//	s5k6aa_ctrl->curr_res = s5k6aa_ctrl->prev_res;
//	s5k6aa_ctrl->sensormode = mode;
	return rc;
}

static long s5k6aa_set_sensor_mode(int mode)
{
	printk("[s5k6aa] %s : %d /%d\n", __func__, mode, __LINE__);

	switch (mode) {
		case SENSOR_PREVIEW_MODE:
		s5k6aa_video_config(mode);
		break;

		case SENSOR_SNAPSHOT_MODE:
		case SENSOR_RAW_SNAPSHOT_MODE:
		s5k6aa_set_capture();
		break;
		default:
			return 0;//-EINVAL;
	}
	return 0;
}


static int s5k6aa_sensor_pre_init(const struct msm_camera_sensor_info *data)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);

	int rc = 0;

#ifndef CONFIG_LOAD_FILE
	rc = S5K6AA_WRITE_LIST(s5k6aa_init_reg);
	if(rc < 0)
		printk("Error in s5k6aa_sensor_pre_init !");
#endif
	mdelay(10);

	return rc;
}


static int s5k6aa_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);

	int rc = 0;
	int temp = 0;
	int status = 0;
	int count = 0;
	gpio_tlmm_config(GPIO_CFG(CAM_1_3M_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(CAM_1_3M_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
//	gpio_tlmm_config(GPIO_CFG(CAM_I2C_SDA, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
//	gpio_tlmm_config(GPIO_CFG(CAM_I2C_SCL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	gpio_set_value_cansleep(CAM_1_3M_RST, 0);
	temp = gpio_get_value(CAM_1_3M_RST);
	printk("[s5k6aa] CAM_1_3M_RST : %d\n", temp);

	gpio_set_value_cansleep(CAM_1_3M_EN, 0);
	temp = gpio_get_value(CAM_1_3M_EN);
	printk("[s5k6aa] CAM_1_3M_EN : %d\n", temp);

	cam_ldo_power_on_s5k6aa();	// move to msm_camera.c (Jeonhk 20110622)
	mdelay(1);
	gpio_set_value_cansleep(CAM_1_3M_EN, 1);
	temp = gpio_get_value(CAM_1_3M_EN);
	printk("[s5k6aa] CAM_1_3M_EN : %d\n", temp);
	mdelay(1);
	gpio_tlmm_config(GPIO_CFG(CAM_MCLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	msm_camio_clk_rate_set(24000000);
	mdelay(10);
	gpio_set_value_cansleep(CAM_1_3M_RST, 1);
	temp = gpio_get_value(CAM_1_3M_RST);
	printk("[s5k6aa] CAM_1_3M_RST : %d\n", temp);

	mdelay(100);

	S5K6AA_WRITE_LIST(s5k6aa_init_reg);
	S5K6AA_WRITE_LIST(mode_preview_640x480);

	return rc;
}


int s5k6aa_sensor_init(const struct msm_camera_sensor_info *data)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);

	int rc = 0;
//	int i = 0;

//	s5k6aa_ctrl = kzalloc(sizeof(struct s5k6aa_ctrl), GFP_KERNEL);
	if (!s5k6aa_ctrl) {
		printk("s5k6aa_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		s5k6aa_ctrl->sensordata = data;

	config_csi2 = 0;
#ifdef CONFIG_LOAD_FILE
	s5k6aa_regs_table_init();
#endif

	rc = s5k6aa_sensor_init_probe(data);
	if (rc < 0) {
		printk("s5k6aa_sensor_init failed!\n");
		goto init_fail;
	}

init_done:
	return rc;

init_fail:
	kfree(s5k6aa_ctrl);
	return rc;
}

static int s5k6aa_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&s5k6aa_wait_queue);
	return 0;
}

int s5k6aa_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&s5k6aa_sem); */

	printk("s5k6aa_ioctl, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = s5k6aa_set_sensor_mode(
						cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			//rc = s5k6aa_set_effect(cfg_data.mode, cfg_data.cfg.effect);
			break;

		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = 0;//-EINVAL;
			printk("s5k6aa_sensor_config : Invalid cfgtype ! %d\n",cfg_data.cfgtype);
			break;
		}

	/* up(&s5k6aa_sem); */

	return rc;
}

int s5k6aa_sensor_release(void)
{
	int rc = 0;

	printk("[s5k6aa] s5k6aa_sensor_release\n");

	kfree(s5k6aa_ctrl);
	/* up(&s5k6aa_sem); */

#ifdef CONFIG_LOAD_FILE
	s5k6aa_regs_table_exit();
#endif
/*	gpio_set_value_cansleep(CAM_3M_RST, LOW);
	mdelay(1);
	gpio_set_value_cansleep(CAM_3M_STBY, LOW);
	mdelay(1);
*/
//	gpio_set_value(CAM_IO_EN, 0);
	cam_ldo_power_off_s5k6aa();
	return rc;
}

static int s5k6aa_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	printk("111111111111111\n");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
	printk("222222222222222\n");
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	s5k6aa_sensorw =
		kzalloc(sizeof(struct s5k6aa_work), GFP_KERNEL);

	if (!s5k6aa_sensorw) {
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, s5k6aa_sensorw);
	s5k6aa_init_client(client);
	s5k6aa_client = client;


	printk("s5k6aa_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(s5k6aa_sensorw);
	s5k6aa_sensorw = NULL;
	printk("s5k6aa_probe failed!\n");
	return rc;
}

static const struct i2c_device_id s5k6aa_i2c_id[] = {
	{ "s5k6aa", 0},
	{ },
};

static struct i2c_driver s5k6aa_i2c_driver = {
	.id_table = s5k6aa_i2c_id,
	.probe  = s5k6aa_i2c_probe,
	.remove = __exit_p(s5k6aa_i2c_remove),
	.driver = {
		.name = "s5k6aa",
	},
};


static int s5k6aa_sensor_probe(const struct msm_camera_sensor_info *info,
				struct msm_sensor_ctrl *s)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);
	int rc = i2c_add_driver(&s5k6aa_i2c_driver);
	if (rc < 0 || s5k6aa_client == NULL) {
	printk("[s5k6aa] %d/%d\n", rc, s5k6aa_client);

		rc = -ENOTSUPP;
		goto probe_done;
	}

	msm_camio_clk_rate_set(24000000);

//	rc = s5k6aa_sensor_init_probe(info);
//	if (rc < 0)
//		goto probe_done;

	s->s_init = s5k6aa_sensor_init;
	s->s_release = s5k6aa_sensor_release;
	s->s_config  = s5k6aa_sensor_config;
	s->s_camera_type = FRONT_CAMERA_2D;
	s->s_mount_angle = 90;		// HC - 0/180, GB - 90/270


probe_done:
	printk("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}


static struct s5k6aa_format s5k6aa_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static int s5k6aa_enum_fmt(struct v4l2_subdev *sd, unsigned int index,
			   enum v4l2_mbus_pixelcode *code)
{
	printk(KERN_DEBUG "Index is %d\n", index);
	if ((unsigned int)index >= ARRAY_SIZE(s5k6aa_subdev_info))
		return -EINVAL;

	*code = s5k6aa_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops s5k6aa_subdev_core_ops;
static struct v4l2_subdev_video_ops s5k6aa_subdev_video_ops = {
	.enum_mbus_fmt = s5k6aa_enum_fmt,
};

static struct v4l2_subdev_ops s5k6aa_subdev_ops = {
	.core = &s5k6aa_subdev_core_ops,
	.video  = &s5k6aa_subdev_video_ops,
};

static int s5k6aa_sensor_probe_cb(const struct msm_camera_sensor_info *info,
	struct v4l2_subdev *sdev, struct msm_sensor_ctrl *s)
{
	printk("[s5k6aa] %s/%d\n", __func__, __LINE__);
	int rc = 0;
	rc = s5k6aa_sensor_probe(info, s);
	if (rc < 0)
		return rc;

	s5k6aa_ctrl = kzalloc(sizeof(struct s5k6aa_ctrl_t), GFP_KERNEL);
	if (!s5k6aa_ctrl) {
		printk("s5k6aa_sensor_probe failed!\n");
		return -ENOMEM;
	}

	/* probe is successful, init a v4l2 subdevice */
	printk(KERN_DEBUG "going into v4l2_i2c_subdev_init\n");
	if (sdev) {
		v4l2_i2c_subdev_init(sdev, s5k6aa_client,
						&s5k6aa_subdev_ops);
		s5k6aa_ctrl->sensor_dev = sdev;
	}
        else printk(KERN_DEBUG "[s5k6aa] sdev is null in probe_cb\n");

	return rc;
}

static int __s5k6aa_probe(struct platform_device *pdev)
{
	printk("############# S5K6AA probe ##############\n");
#if 0	// sysfs for factory test
	s5k6aa_dev = device_create(sec_class, NULL, 0, NULL, "sec_s5k6aa");

	if (IS_ERR(s5k6aa_dev)) {
		printk("Failed to create device!");
		return -1;
	}

	if (device_create_file(s5k6aa_dev, &s5k6aa_cameraflash_attr) < 0) {
		printk("Failed to create device file!(%s)!\n", s5k6aa_cameraflash_attr.attr.name);
		return -1;
	}

	if (device_create_file(s5k6aa_dev, &s5k6aa_camtype_attr) < 0) {
		printk("Failed to create device file!(%s)!\n", s5k6aa_camtype_attr.attr.name);
		return -1;
	}
#endif
	return msm_sensor_register(pdev, s5k6aa_sensor_probe_cb);
}

static struct platform_driver msm_camera_driver = {
	.probe = __s5k6aa_probe,
	.driver = {
		.name = "msm_camera_s5k6aa",
		.owner = THIS_MODULE,
	},
};

static int __init s5k6aa_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(s5k6aa_init);

