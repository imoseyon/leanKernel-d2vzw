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
#include <mach/gpio.h>
#include <mach/camera.h>

#include <mach/camera.h>
#include <mach/vreg.h>
#include <linux/io.h>

#include "isx012.h"
#include "isx012_regs.h"
#include "cam_pmic.h"


//#define CONFIG_LOAD_FILE


#define ISX012_WRITE_LIST(A)			isx012_i2c_write_list(A,(sizeof(A) / sizeof(A[0])),#A);



struct isx012_work {
	struct work_struct work;
};

static struct  isx012_work *isx012_sensorw;
static struct  i2c_client *isx012_client;

static unsigned int config_csi2;
static struct isx012_ctrl *isx012_ctrl;

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

#if 0
static int isx012_i2c_read_multi(unsigned short subaddr, unsigned long *data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {isx012_client->addr, 0, 2, buf};

	int err = 0;

	if (!isx012_client->adapter) {
		//dev_err(&isx012_client->dev, "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	buf[0] = subaddr>> 8;
	buf[1] = subaddr & 0xff;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&isx012_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
		return -EIO;
	}

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&isx012_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
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
 * isx012_i2c_read: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @data: data to be read
 *
 * Returns 0 on success, <0 on error
 */
static inline int isx012_i2c_read(unsigned short subaddr, unsigned short *data)
{
	unsigned char buf[2];
	struct i2c_msg msg = {isx012_client->addr, 0, 2, buf};

	int err = 0;

	if (!isx012_client->adapter) {
		//dev_err(&isx012_client->dev, "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	buf[0] = subaddr>> 8;
	buf[1] = subaddr & 0xff;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&isx012_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
		return -EIO;
	}

	msg.flags = I2C_M_RD;

	err = i2c_transfer(isx012_client->adapter, &msg, 1);
	if (unlikely(err < 0)) {
		//dev_err(&isx012_client->dev, "%s: %d register read fail\n", __func__, __LINE__);
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
 * isx012_i2c_write_multi: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static inline int isx012_i2c_write_multi(unsigned short addr, unsigned int w_data, unsigned int w_len)
{
	unsigned char buf[w_len+2];
	struct i2c_msg msg = {isx012_client->addr, 0, w_len+2, buf};

	int retry_count = 5;
	int err = 0;

	if (!isx012_client->adapter) {
		//dev_err(&isx012_client->dev, "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	buf[0] = addr >> 8;
	buf[1] = addr & 0xff;

	/*
	 * Data should be written in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */
	if(w_len == 1) {
		buf[2] = (unsigned char)w_data;
	} else if(w_len == 2)	{
		*((unsigned short *)&buf[2]) = (unsigned short)w_data;
	} else {
		*((unsigned int *)&buf[2]) = w_data;
	}

#ifdef ISX012_DEBUG
	{
		int j;
		printk("isx012 i2c write W: ");
		for(j = 0; j <= w_len+1; j++)
		{
			printk("0x%02x ", buf[j]);
		}
		printk("\n");
	}
#endif

	while(retry_count--) {
		err  = i2c_transfer(isx012_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
//		msleep(POLL_TIME_MS);
	}

	return (err == 1) ? 0 : -EIO;
}

static int isx012_i2c_write_list(isx012_short_t regs[], int size, char *name)
{

#ifdef CONFIG_LOAD_FILE
	isx012_regs_table_write(client, name);
#else
	int err = 0;
	int i = 0;

	if (!isx012_client->adapter) {
		printk(KERN_ERR "%s: %d can't search i2c client adapter\n", __func__, __LINE__);
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		err = isx012_i2c_write_multi(regs[i].subaddr, regs[i].value, regs[i].len);
		if (unlikely(err < 0)) {
			//dev_err(isx012_client, "%s: register set failed\n",  __func__);
			return -EIO;
		}
	}
#endif

	return 0;
}


//////////////////////////////////////////////////////////////
#ifdef FACTORY_TEST
extern struct class *sec_class;
struct device *isx012_dev;

static ssize_t cameratype_file_cmd_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char sensor_info[30] = "isx012";
	return sprintf(buf, "%s\n", sensor_info);
}

static ssize_t cameratype_file_cmd_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t size)
{
		/*Reserved*/
	return size;
}

//static DEVICE_ATTR(cameratype, 0666, cameratype_file_cmd_show, cameratype_file_cmd_store);
static struct device_attribute isx012_camtype_attr = {
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
		isx012_set_flash(MOVIE_FLASH,0);
	} else {
		printk(KERN_INFO "[Factory flash]ON\n");
		isx012_set_flash(MOVIE_FLASH,1);
	}

	return size;
}

//static DEVICE_ATTR(cameraflash, 0666, cameraflash_file_cmd_show, cameraflash_file_cmd_store);
static struct device_attribute isx012_cameraflash_attr = {
	.attr = {
		.name = "cameraflash",
		.mode = (S_IRUGO | S_IWUGO)},
	.show = cameraflash_file_cmd_show,
	.store = cameraflash_file_cmd_store
};
#endif
//////////////////////////////////////////////////////////////

int isx012_mipi_mode(int mode)
{
	int rc = 0;
	struct msm_camera_csi_params isx012_csi_params;

	CAM_DEBUG("%s E\n",__FUNCTION__);

	if (!config_csi2) {
		isx012_csi_params.lane_cnt = 1;
		isx012_csi_params.data_format = CSI_8BIT;
		isx012_csi_params.lane_assign = 0xe4;
		isx012_csi_params.dpcm_scheme = 0;
		isx012_csi_params.settle_cnt = 24;// 0x14; //0x7; //0x14;
		rc = msm_camio_csi_config(&isx012_csi_params);
		if (rc < 0)
			printk(KERN_ERR "config csi controller failed \n");
		config_csi2 = 1;
	}
	CAM_DEBUG("%s X\n",__FUNCTION__);
	return rc;
}


int isx012_start(void)
{
	int rc=0;
//	unsigned short read_value;
//	unsigned short	id = 0;

	CAM_DEBUG("%s E\n",__FUNCTION__);
	CAM_DEBUG("I2C address : 0x%x\n",isx012_client->addr);

	if(isx012_ctrl->started) {
		CAM_DEBUG("%s X : already started\n",__FUNCTION__);
		return rc;
	}

	isx012_mipi_mode(1);
	msleep(300); //=> Please add some delay

	ISX012_WRITE_LIST(isx012_init_reg);
//	isx012_i2c_write_list(isx012_init_reg,sizeof(isx012_init_reg)/sizeof(isx012_init_reg[0]),"isx012_init_reg");

	return 0;

}

void isx012_set_preview(void)
{

}

void isx012_set_capture(void)
{
}

static long isx012_set_sensor_mode(int mode)
{
	CAM_DEBUG("s5k5ccaf_set_sensor_mode : %d\n",mode);
	switch (mode) {
		case SENSOR_PREVIEW_MODE:
		isx012_start();
		isx012_set_preview();
		break;

		case SENSOR_SNAPSHOT_MODE:
		case SENSOR_RAW_SNAPSHOT_MODE:
		isx012_set_capture();
		break;
		default:
			return -EINVAL;
	}
	return 0;
}



static int isx012_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	printk("[S5K5CCGX] sensor_init_probe()\n");

/*	gpio_set_value_cansleep(CAM_3M_RST, LOW);
	gpio_set_value_cansleep(CAM_3M_STBY, LOW);
	gpio_set_value_cansleep(CAM_VGA_RST, LOW);
	gpio_set_value_cansleep(CAM_VGA_STBY, LOW);
	*/

	//cam_ldo_power_on();	// move to msm_camera.c (Jeonhk 20110622)

	msm_camio_clk_rate_set(24000000);

/*
	gpio_set_value_cansleep(CAM_3M_STBY, HIGH);
	udelay(20);
	gpio_set_value_cansleep(CAM_3M_RST, HIGH);
	mdelay(10);
*/

#ifdef CONFIG_LOAD_FILE
	mdelay(100);
	ISX012_WRITE_LIST(isx012_init0);
#endif

	return rc;
}


int isx012_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
//	int i = 0;

	isx012_ctrl = kzalloc(sizeof(struct isx012_ctrl), GFP_KERNEL);
	if (!isx012_ctrl) {
		CDBG("isx012_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}

	if (data)
		isx012_ctrl->sensordata = data;

	config_csi2 = 0;
#ifdef CONFIG_LOAD_FILE
	isx012_regs_table_init();
#endif

	rc = isx012_sensor_init_probe(data);
	if (rc < 0) {
		CDBG("isx012_sensor_init failed!\n");
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

int isx012_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data,
			(void *)argp,
			sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	/* down(&isx012_sem); */

	CDBG("isx012_ioctl, cfgtype = %d, mode = %d\n",
		cfg_data.cfgtype, cfg_data.mode);

		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = isx012_set_sensor_mode(
						cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			//rc = isx012_set_effect(cfg_data.mode, cfg_data.cfg.effect);
			break;

		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = -EINVAL;
			printk("isx012_sensor_config : Invalid cfgtype ! %d\n",cfg_data.cfgtype);
			break;
		}

	/* up(&isx012_sem); */

	return rc;
}

int isx012_sensor_release(void)
{
	int rc = 0;

	printk("[isx012] isx012_sensor_release\n");

	kfree(isx012_ctrl);
	/* up(&isx012_sem); */

#ifdef CONFIG_LOAD_FILE
	isx012_regs_table_exit();
#endif
/*	gpio_set_value_cansleep(CAM_3M_RST, LOW);
	mdelay(1);
	gpio_set_value_cansleep(CAM_3M_STBY, LOW);
	mdelay(1);
*/
//	gpio_set_value(CAM_IO_EN, 0);
	cam_ldo_power_off();
	return rc;
}

static int isx012_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;

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


	CDBG("isx012_probe succeeded!\n");

	return 0;

probe_failure:
	kfree(isx012_sensorw);
	isx012_sensorw = NULL;
	CDBG("isx012_probe failed!\n");
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
	if (rc < 0 || isx012_client == NULL) {
		rc = -ENOTSUPP;
		goto probe_done;
	}

	//rc = isx012_sensor_init_probe(info);
	if (rc < 0)
		goto probe_done;

	s->s_init = isx012_sensor_init;
	s->s_release = isx012_sensor_release;
	s->s_config  = isx012_sensor_config;
	s->s_camera_type = BACK_CAMERA_2D;
	s->s_mount_angle = 90;		// HC - 0/180, GB - 90/270


probe_done:
	CDBG("%s %s:%d\n", __FILE__, __func__, __LINE__);
	return rc;
}

static int __isx012_probe(struct platform_device *pdev)
{
	printk("############# ISX012 probe ##############\n");
#if 0	// sysfs for factory test
	isx012_dev = device_create(sec_class, NULL, 0, NULL, "sec_isx012");

	if (IS_ERR(isx012_dev)) {
		printk("Failed to create device!");
		return -1;
	}

	if (device_create_file(isx012_dev, &isx012_cameraflash_attr) < 0) {
		printk("Failed to create device file!(%s)!\n", isx012_cameraflash_attr.attr.name);
		return -1;
	}

	if (device_create_file(isx012_dev, &isx012_camtype_attr) < 0) {
		printk("Failed to create device file!(%s)!\n", isx012_camtype_attr.attr.name);
		return -1;
	}
#endif
	return msm_camera_drv_start(pdev, isx012_sensor_probe);
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
