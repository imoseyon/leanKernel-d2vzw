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
#include <mach/jasper-gpio.h>

#include <asm/mach-types.h>
#include <mach/vreg.h>
#include <linux/io.h>

#include "msm.h"
#include "db8131m.h"

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

static char *db8131m_regs_table;
static int db8131m_regs_table_size;
static int db8131m_write_regs_from_sd(char *name);
static int db8131m_i2c_write_multi(unsigned short addr, unsigned int w_data);

struct test {
	u8 data;
	struct test *nextBuf;
};
static struct test *testBuf;
static s32 large_file;
#endif
static int db8131m_sensor_config(void __user *argp);
DEFINE_MUTEX(db8131m_mut);

#if defined(CONFIG_MACH_GOGH)
#include "db8131m_reg_v2.h"
#elif defined(CONFIG_MACH_JASPER)
#include "db8131m_reg_jasper.h"
#elif defined(CONFIG_MACH_APEXQ)
#include "db8131m_reg_apexq.h"
#elif defined(CONFIG_MACH_AEGIS2)
#include "db8131m_reg_aegis2.h"
#elif defined(CONFIG_MACH_COMANCHE)
#include "db8131m_reg_comanche.h"
#else
#include "db8131m_reg.h"
#endif

#define DB8_WRT_LIST(A)		\
		db8131m_i2c_write_list(A, (sizeof(A) / sizeof(A[0])), #A);

struct db8131m_work {
	struct work_struct work;
};

static struct  i2c_client *db8131m_client;
static struct msm_sensor_ctrl_t db8131m_s_ctrl;
static struct device db8131m_dev;

struct db8131m_ctrl {
	const struct msm_camera_sensor_info *sensordata;
	struct db8131m_userset settings;
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
static struct db8131m_ctrl *db8131m_ctrl;

struct db8131m_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};


#ifdef CONFIG_LOAD_FILE
int db8131m_regs_table_init(void)
{
	struct file *fp = NULL;
	struct test *nextBuf = NULL;

	u8 *nBuf = NULL;
	size_t file_size = 0, max_size = 0, testBuf_size = 0;
	ssize_t nread = 0;
	s32 check = 0, starCheck = 0;
	s32 tmp_large_file = 0;
	s32 i = 0;
	int ret = 0;
	loff_t pos;

	CAM_DEBUG("CONFIG_LOAD_FILE is enable!!\n");

	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	BUG_ON(testBuf);

	fp = filp_open("/mnt/sdcard/db8131m_reg.h", O_RDONLY, 0);
	if (IS_ERR(fp)) {
		cam_err("failed to open /mnt/sdcard/db8131m_reg.h");
		return PTR_ERR(fp);
	}

	file_size = (size_t) fp->f_path.dentry->d_inode->i_size;
	max_size = file_size;

	cam_info("file_size = %d", file_size);

	nBuf = kmalloc(file_size, GFP_ATOMIC);
	if (nBuf == NULL) {
		cam_err("Fail to 1st get memory");
		nBuf = vmalloc(file_size);
		if (nBuf == NULL) {
			cam_err("ERR: nBuf Out of Memory");
			ret = -ENOMEM;
			goto error_out;
		}
		tmp_large_file = 1;
	}

	testBuf_size = sizeof(struct test) * file_size;
	if (tmp_large_file) {
		testBuf = vmalloc(testBuf_size);
		large_file = 1;
	} else {
		testBuf = kmalloc(testBuf_size, GFP_ATOMIC);
		if (testBuf == NULL) {
			cam_err("Fail to get mem(%d bytes)", testBuf_size);
			testBuf = vmalloc(testBuf_size);
			large_file = 1;
		}
	}
	if (testBuf == NULL) {
		cam_err("ERR: Out of Memory");
		ret = -ENOMEM;
		goto error_out;
	}

	pos = 0;
	memset(nBuf, 0, file_size);
	memset(testBuf, 0, file_size * sizeof(struct test));

	nread = vfs_read(fp, (char __user *)nBuf, file_size, &pos);
	if (nread != file_size) {
		cam_err("failed to read file ret = %d", nread);
		ret = -1;
		goto error_out;
	}

	set_fs(fs);

	i = max_size;

	cam_info("i = %d", i);

	while (i) {
		testBuf[max_size - i].data = *nBuf;
		if (i != 1) {
			testBuf[max_size - i].nextBuf =
						&testBuf[max_size - i + 1];
		} else {
			testBuf[max_size - i].nextBuf = NULL;
			break;
		}
		i--;
		nBuf++;
	}

	i = max_size;
	nextBuf = &testBuf[0];

	while (i - 1) {
		if (!check && !starCheck) {
			if (testBuf[max_size - i].data == '/') {
				if (testBuf[max_size-i].nextBuf != NULL) {
					if (testBuf[max_size-i].nextBuf->data
								== '/') {
						check = 1;/* when find '//' */
						i--;
					} else if (testBuf[max_size-i].nextBuf->
								data == '*') {
						starCheck = 1;/*when'/ *' */
						i--;
					}
				} else
					break;
			}
			if (!check && !starCheck) {
				/* ignore '\t' */
				if (testBuf[max_size - i].data != '\t') {
					nextBuf->nextBuf = &testBuf[max_size-i];
					nextBuf = &testBuf[max_size - i];
				}
			}
		} else if (check && !starCheck) {
			if (testBuf[max_size - i].data == '/') {
				if (testBuf[max_size-i].nextBuf != NULL) {
					if (testBuf[max_size-i].nextBuf->
								data == '*') {
						starCheck = 1; /*when '/ *' */
						check = 0;
						i--;
					}
				} else
					break;
			}

			 /* when find '\n' */
			if (testBuf[max_size - i].data == '\n' && check) {
				check = 0;
				nextBuf->nextBuf = &testBuf[max_size - i];
				nextBuf = &testBuf[max_size - i];
			}

		} else if (!check && starCheck) {
			if (testBuf[max_size - i].data
						== '*') {
				if (testBuf[max_size-i].nextBuf != NULL) {
					if (testBuf[max_size-i].nextBuf->
								data == '/') {
						starCheck = 0; /*when'* /' */
						i--;
					}
				} else
					break;
			}
		}

		i--;

		if (i < 2) {
			nextBuf = NULL;
			break;
		}

		if (testBuf[max_size - i].nextBuf == NULL) {
			nextBuf = NULL;
			break;
		}
	}

#if FOR_DEBUG /* for print */
	printk(KERN_DEBUG "i = %d\n", i);
	nextBuf = &testBuf[0];
	while (1) {
		if (nextBuf->nextBuf == NULL)
			break;
		printk(KERN_DEBUG "%c", nextBuf->data);
		nextBuf = nextBuf->nextBuf;
	}
#endif

	tmp_large_file ? vfree(nBuf) : kfree(nBuf);

error_out:
	if (fp)
		filp_close(fp, current->files);
	return ret;
}

static inline int db8131m_write(struct i2c_client *client,
		u16 packet)
{
	u8 buf[2];
	int err = 0, retry_count = 5;

	struct i2c_msg msg;

	if (!client->adapter) {
		cam_err("ERR - can't search i2c client adapter");
		return -EIO;
	}

	buf[0] = (u8) (packet >> 8);
	buf[1] = (u8) (packet & 0xff);

	msg.addr = 0x45;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = buf;

	do {
		err = i2c_transfer(db8131m_client->adapter, &msg, 1);
		if (err == 1)
			return 0;
		retry_count++;
		cam_err("i2c transfer failed, retrying %x err:%d\n",
		       packet, err);
		mdelay(3);

	} while (retry_count <= 5);

	return (err != 1) ? -1 : 0;
}

static int db8131m_write_regs_from_sd(char *name)
{
	struct test *tempData = NULL;

	int ret = -EAGAIN;
	u16 temp, temp_2;
	u16 delay = 0;
	u8 data[7];
	s32 searched = 0;
	size_t size = strlen(name);
	s32 i;

	CAM_DEBUG("E size = %d, string = %s", size, name);
	tempData = &testBuf[0];
	*(data + 6) = '\0';

	while (!searched) {
		searched = 1;
		for (i = 0; i < size; i++) {
			if (tempData->data != name[i]) {
				searched = 0;
				break;
			}
			tempData = tempData->nextBuf;
		}
		tempData = tempData->nextBuf;
	}

	/* structure is get..*/
	while (1) {
		if (tempData->data == '{')
			break;
		else
			tempData = tempData->nextBuf;
	}

	while (1) {
		searched = 0;
		while (1) {
			if (tempData->data == 'x') {
				/* get 6 strings.*/
				data[0] = '0';
				for (i = 1; i < 6; i++) {
					data[i] = tempData->data;
					tempData = tempData->nextBuf;
				}
				kstrtoul(data, 16, &temp);
				/*CAM_DEBUG("%s\n", data);
				CAM_DEBUG("kstrtoul data = 0x%x\n", temp);*/
				break;
			} else if (tempData->data == '}') {
				searched = 1;
				break;
			} else
				tempData = tempData->nextBuf;

			if (tempData->nextBuf == NULL)
				return -1;
		}

		if (searched)
			break;
		if ((temp & 0xFF00) == DB8131M_DELAY) {
			delay = temp & 0xFF;
			cam_info("db8131 delay(%d)", delay); /*step is 10msec */
			msleep(delay);
			continue;
		}
		ret = db8131m_write(db8131m_client, temp);

		/* In error circumstances */
		/* Give second shot */
		if (unlikely(ret)) {
			ret = db8131m_write(db8131m_client, temp);

			/* Give it one more shot */
			if (unlikely(ret))
				ret = db8131m_write(db8131m_client, temp);
			}
		}

	return 0;
}

void db8131m_regs_table_exit(void)
{
	if (testBuf == NULL)
		return;
	else {
		large_file ? vfree(testBuf) : kfree(testBuf);
		large_file = 0;
		testBuf = NULL;
	}
}

#endif

static DECLARE_WAIT_QUEUE_HEAD(db8131m_wait_queue);

static int db8131m_i2c_read(unsigned char subaddr, unsigned char *data)
{
	int ret;
	unsigned char buf[1];
	struct i2c_msg msg = { db8131m_client->addr, 0, 1, buf };

	buf[0] = subaddr;

	ret = i2c_transfer(db8131m_client->adapter, &msg, 1) == 1 ? 0 : -EIO;
	if (ret == -EIO)
		goto error;

	msg.flags = I2C_M_RD;

	ret = i2c_transfer(db8131m_client->adapter, &msg, 1) == 1 ? 0 : -EIO;
	if (ret == -EIO)
		goto error;

	*data = buf[0];

error:
	return ret;
}

static int32_t db8131m_i2c_write_16bit(u16 packet)
{
	int32_t rc = -EFAULT;
	int retry_count = 0;

	unsigned char buf[2];

	struct i2c_msg msg;
	buf[0] = (u8) (packet >> 8);
	buf[1] = (u8) (packet & 0xff);

	msg.addr = db8131m_client->addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = buf;

#if defined(CAM_I2C_DEBUG)
	cam_err("I2C CHIP ID=0x%x, DATA 0x%x 0x%x\n",
			db8131m_client->addr, buf[0], buf[1]);
#endif
	do {
		rc = i2c_transfer(db8131m_client->adapter, &msg, 1);
		if (rc == 1)
			return 0;
		retry_count++;
		cam_err("i2c transfer failed, retrying %x err:%d\n",
			packet, rc);
		mdelay(3);

	} while (retry_count <= 5);

	return 0;
}

static int db8131m_i2c_write_list(const u16 *list, int size, char *name)
{
	int ret = 0;

#ifdef CONFIG_LOAD_FILE
	ret = db8131m_write_regs_from_sd(name);
#else
	int i;
	u8 m_delay = 0;
	u16 temp_packet;

	CAM_DEBUG("%s, size=%d", name, size);
	for (i = 0; i < size; i++) {
		temp_packet = list[i];
		if ((temp_packet & 0xFF00) == DB8131M_DELAY) {
			m_delay = temp_packet & 0xFF;
			cam_info("delay = %d", m_delay);
			msleep(m_delay);
			continue;
		}
		if (db8131m_i2c_write_16bit(temp_packet) < 0) {
			cam_err("fail(0x%x, 0x%x:%d)",
				db8131m_client->addr, temp_packet, i);
			return -EIO;
		}
		/*udelay(10);*/
	}
#endif
	return ret;
}
/**
 * db8131m_i2c_read_multi: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 * @r_data: buffer where data is read
 * @r_len: number of bytes to read
 *
 * Returns 0 on success, <0 on error
 */

static int db8131m_i2c_read_multi(unsigned short subaddr, unsigned long *data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {db8131m_client->addr, 0, 2, buf};

	int err = 0;

	if (!db8131m_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(db8131m_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;
	msg.len = 4;

	err = i2c_transfer(db8131m_client->adapter, &msg, 1);
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
 * db8131m_i2c_write_multi: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static int db8131m_i2c_write_multi(unsigned short addr, unsigned int w_data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {db8131m_client->addr, 0, 4, buf};

	int retry_count = 5;
	int err = 0;

	if (!db8131m_client->adapter)
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
		err  = i2c_transfer(db8131m_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
	}
	return (err == 1) ? 0 : -EIO;
}

#ifdef FACTORY_TEST
struct class *sec_class;
struct device *db8131m_dev;

static ssize_t cameratype_file_cmd_show(struct device *dev,
				struct device_attribute *attr, char *buf) {
	char sensor_info[30] = "db8131m";
	return snprintf(buf, "%s\n", sensor_info);
}

static ssize_t cameratype_file_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
		/*Reserved*/
	return size;
}

static struct device_attribute db8131m_camtype_attr = {
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
		db8131m_set_flash(MOVIE_FLASH, 0);
	} else {
		CAM_DEBUG("[Factory flash]ON");
		db8131m_set_flash(MOVIE_FLASH, 1);
	}
	return size;
}

static struct device_attribute db8131m_cameraflash_attr = {
	.attr = {
		.name = "cameraflash",
		.mode = (S_IRUGO | S_IWUGO)},
		.show = cameraflash_file_cmd_show,
		.store = cameraflash_file_cmd_store
};
#endif

void db8131m_set_preview_size(int32_t index)
{
	CAM_DEBUG("index %d", index);

	db8131m_ctrl->settings.preview_size_idx = index;
}


void db8131m_set_preview(void)
{
	int err = 0;
	CAM_DEBUG("cam_mode = %d", db8131m_ctrl->cam_mode);
	CAM_DEBUG("db8131m_set_preview function called\n");

	if (db8131m_ctrl->cam_mode == MOVIE_MODE) {
		CAM_DEBUG("Camcorder_Mode_ON");
		if (db8131m_ctrl->settings.preview_size_idx ==
				PREVIEW_SIZE_HD) {
			CAM_DEBUG("720P recording");
			DB8_WRT_LIST(db8131m_720p_common);
	} else {
			CAM_DEBUG("VGA recording");
			DB8_WRT_LIST(db8131m_common);
			DB8_WRT_LIST(db8131m_recording_60Hz_common);
		}
	} else {
		CAM_DEBUG("Preview_Mode");
		if (db8131m_ctrl->op_mode == CAMERA_MODE_INIT) {
			DB8_WRT_LIST(db8131m_common);
			CAM_DEBUG("db8131m Common Registers written\n");
		}
		DB8_WRT_LIST(db8131m_preview);

		CAM_DEBUG(" db8131m Preview Registers written\n");

		db8131m_ctrl->op_mode = CAMERA_MODE_PREVIEW;
	}
}

void db8131m_set_capture(void)
{
	CAM_DEBUG("db8131m set capture\n");
	db8131m_ctrl->op_mode = CAMERA_MODE_CAPTURE;
	DB8_WRT_LIST(db8131m_capture);
}

static int32_t db8131m_sensor_setting(int update_type, int rt)
{
	CAM_DEBUG("Start.. Sensor Setting\n");

	int32_t rc = 0;
	int temp = 0;
	struct msm_camera_csid_params db8131m_csid_params;
	struct msm_camera_csiphy_params db8131m_csiphy_params;
	switch (update_type) {
	case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE)
			/* Add some condition statements */
		break;

	case UPDATE_PERIODIC:
		CAM_DEBUG("Update periodic called in db8131m\n");
		if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			CAM_DEBUG("UPDATE_PERIODIC");

			v4l2_subdev_notify(db8131m_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX_0, ISPIF_OFF_IMMEDIATELY));

			/* stop streaming */
			/*S5K8_WRT_LIST(db8131m_stream_stop);*/
			msleep(100);

			struct msm_camera_csid_vc_cfg db8131m_vccfg[] = {
					{0, 0x1E, CSI_DECODE_8BIT},
					{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
					};
			db8131m_csid_params.lane_cnt = 1;
			db8131m_csid_params.lane_assign = 0xe4;
			db8131m_csid_params.lut_params.num_cid =
				ARRAY_SIZE(db8131m_vccfg);
			db8131m_csid_params.lut_params.vc_cfg =
				&db8131m_vccfg[0];
			db8131m_csiphy_params.lane_cnt = 1;
				db8131m_csiphy_params.settle_cnt = 0x07;
			v4l2_subdev_notify(db8131m_ctrl->sensor_dev,
					NOTIFY_CSID_CFG, &db8131m_csid_params);
			v4l2_subdev_notify(db8131m_ctrl->sensor_dev,
					NOTIFY_CID_CHANGE, NULL);
			mb();
			v4l2_subdev_notify(db8131m_ctrl->sensor_dev,
					NOTIFY_CSIPHY_CFG,
					&db8131m_csiphy_params);
			mb();
				/*db8131m_delay_msecs_stdby*/
			msleep(100);
			config_csi2 = 1;

			v4l2_subdev_notify(db8131m_ctrl->sensor_dev,
				NOTIFY_ISPIF_STREAM, (void *)ISPIF_STREAM(
				PIX_0, ISPIF_ON_FRAME_BOUNDARY));

			/*start stream*/
			/*S5K8_WRT_LIST(db8131m_preview); */
		}
		break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int32_t db8131m_video_config(int mode)
{
	int32_t	rc = 0;
	int	rt;

	CAM_DEBUG("[db8131m] %s E", __func__);
	if (db8131m_sensor_setting(UPDATE_PERIODIC, RES_PREVIEW) < 0)
		rc = -1;

	CAM_DEBUG("[db8131m] %s X", __func__);
	return rc;
}

static long db8131m_set_sensor_mode(int mode)
{
	CAM_DEBUG("%d", mode);
	CAM_DEBUG("db8131m set_sensor_mode function E");

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	case SENSOR_VIDEO_MODE:
		CAM_DEBUG("db8131m sensor mode is SENSOR_PREVIEW_MODE\n");
		db8131m_set_preview();
		if (config_csi2 == 0)
			db8131m_video_config(mode);
		break;
	case SENSOR_SNAPSHOT_MODE:
	case SENSOR_RAW_SNAPSHOT_MODE:
		CAM_DEBUG("db8131m sensor mode is SENSOR_SNAPSHOT_MODE\n");
		db8131m_set_capture();
		break;
	default:
		return 0;
	}
	return 0;
}

static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

#if defined(CONFIG_S5K5CCGX) && defined(CONFIG_DB8131M) /* jasper */
static int db8131m_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	CAM_DEBUG("=== Start ===");
	CAM_DEBUG("=== db8131m power up ===");

	int rc = 0;
	int temp = 0;
	int status = 0;
	int count = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

#ifdef CONFIG_LOAD_FILE
	db8131m_regs_table_init();
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
	usleep(1000);

	/*Power on the LDOs */
	data->sensor_platform_info->sensor_power_on(1);
	usleep(1000);
	/*standy VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);
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

	usleep(5000);

	/*reset VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	usleep(100*1000);

	/* sensor validation test */
	CAM_DEBUG("Front Camera Sensor Validation Test");
/*
	rc = S5K8_WRT_LIST(db8131m_pre_common);
*/
	if (rc < 0) {
		pr_info("Error in Front Camera Sensor Validation Test");
		return rc;
	}
	config_csi2 = 0;
/*Added */
	db8131m_ctrl->op_mode = CAMERA_MODE_INIT;

	return rc;
}
#elif defined(CONFIG_ISX012) && defined(CONFIG_DB8131M) /* Gogh *//* AEGIS2 */
static int db8131m_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	CAM_DEBUG("=== Start ===");
	CAM_DEBUG("=== db8131m power up ===");

	int rc = 0;
	int temp = 0;
	int status = 0;
	int count = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

#ifdef CONFIG_LOAD_FILE
	db8131m_regs_table_init();
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
	usleep(1000);

	/*Power on the LDOs */
	data->sensor_platform_info->sensor_power_on(1);
	usleep(1000);
	/*standy VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);
	mdelay(5);

	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;

	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0)
		pr_err("%s: clk enable failed\n", __func__);

	usleep(1000); /* > 20us */

	/*reset VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	mdelay(5); /* > 70000cycle */

	/* sensor validation test */
	CAM_DEBUG("Front Camera Sensor Validation Test");
/*
	rc = S5K8_WRT_LIST(db8131m_pre_common);
*/
	if (rc < 0) {
		pr_info("Error in Front Camera Sensor Validation Test");
		return rc;
	}
	config_csi2 = 0;
/*Added */
	db8131m_ctrl->op_mode = CAMERA_MODE_INIT;

	return rc;
}
#elif defined(CONFIG_S5K4ECGX) && defined(CONFIG_DB8131M) /* Aegis2 */
static int db8131m_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	CAM_DEBUG("=== Start ===");
	CAM_DEBUG("=== db8131m power up ===");

	int rc = 0;
	int temp = 0;
	int status = 0;
	int count = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

#ifdef CONFIG_LOAD_FILE
	db8131m_regs_table_init();
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
	data->sensor_platform_info->sensor_power_on(1);
	usleep(1000);
	/*standy VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);
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

	usleep(5000);

	/*reset VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	usleep(100*1000);

	/* sensor validation test */
	CAM_DEBUG("Front Camera Sensor Validation Test");
/*
	rc = S5K8_WRT_LIST(db8131m_pre_common);
*/
	if (rc < 0) {
		pr_info("Error in Front Camera Sensor Validation Test");
		return rc;
	}
	config_csi2 = 0;
/*Added */
	db8131m_ctrl->op_mode = CAMERA_MODE_INIT;

	return rc;
}
#else
static int db8131m_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	printk(KERN_DEBUG "db8131m_sensor_power_up");
}

#endif

static void db8131m_check_dataline(int val)
{
	if (val) {
		CAM_DEBUG("DTP ON");
		db8131m_ctrl->dtpTest = 1;

	} else {
		CAM_DEBUG("DTP OFF");
		db8131m_ctrl->dtpTest = 0;
	}
}

static int db8131m_set_effect(int effect)
{
	CAM_DEBUG("[db8131m] %s : %d", __func__, effect);

	switch (effect) {
	case CAMERA_EFFECT_OFF:
		CAM_DEBUG("[db8131m] Effect off\n");
		/*S5K8_WRT_LIST(db8131m_effect_none);*/
		break;

	case CAMERA_EFFECT_MONO:
		/*S5K8_WRT_LIST(db8131m_effect_mono);*/
		break;

	case CAMERA_EFFECT_NEGATIVE:
		/*S5K8_WRT_LIST(db8131m_effect_negative);*/
		break;

	case CAMERA_EFFECT_SEPIA:
		/*S5K8_WRT_LIST(db8131m_effect_sepia);*/
		break;

	case CAMERA_EFFECT_WHITEBOARD:
		/*S5K8_WRT_LIST(db8131m_effect_sketch);*/
		break;

	default:
		CAM_DEBUG("[db8131m] default effect");
		/*S5K8_WRT_LIST(db8131m_effect_none);*/
		return 0;
	}

	return 0;
}

static int db8131m_set_whitebalance(int wb)
{
	switch (wb) {
	case CAMERA_WHITE_BALANCE_AUTO:
			/*S5K8_WRT_LIST(db8131m_wb_auto);*/
		break;

	case CAMERA_WHITE_BALANCE_INCANDESCENT:
			/*S5K8_WRT_LIST(db8131m_wb_incandescent);*/
		break;

	case CAMERA_WHITE_BALANCE_FLUORESCENT:
			/*S5K8_WRT_LIST(db8131m_wb_fluorescent);*/
		break;

	case CAMERA_WHITE_BALANCE_DAYLIGHT:
			/*S5K8_WRT_LIST(db8131m_wb_daylight);*/
		break;

	case CAMERA_WHITE_BALANCE_CLOUDY_DAYLIGHT:
			/*S5K8_WRT_LIST(db8131m_wb_cloudy);*/
		break;

	default:
		CAM_DEBUG("[db8131m] unexpected WB mode %s/%d",
			__func__, __LINE__);
		return 0;
	}
	return 0;
}

static void db8131m_set_ev(int ev)
{
	CAM_DEBUG("[db8131m] %s : %d", __func__, ev);

	switch (ev) {
	case CAMERA_EV_M4:
		DB8_WRT_LIST(db8131m_bright_m4);
		break;

	case CAMERA_EV_M3:
		DB8_WRT_LIST(db8131m_bright_m3);
		break;

	case CAMERA_EV_M2:
		DB8_WRT_LIST(db8131m_bright_m2);
		break;

	case CAMERA_EV_M1:
		DB8_WRT_LIST(db8131m_bright_m1);
		break;

	case CAMERA_EV_DEFAULT:
		DB8_WRT_LIST(db8131m_bright_default);
		break;

	case CAMERA_EV_P1:
		DB8_WRT_LIST(db8131m_bright_p1);
		break;

	case CAMERA_EV_P2:
		DB8_WRT_LIST(db8131m_bright_p2);
		break;

	case CAMERA_EV_P3:
		DB8_WRT_LIST(db8131m_bright_p3);
		break;

	case CAMERA_EV_P4:
		DB8_WRT_LIST(db8131m_bright_p4);
		break;

	default:
		CAM_DEBUG("[db8131m] unexpected ev mode %s/%d",
			__func__, __LINE__);
		break;
	}
}

void sensor_native_control_front(void __user *arg)
{
	struct ioctl_native_cmd ctrl_info;

	if (copy_from_user((void *)&ctrl_info,
		(const void *)arg, sizeof(ctrl_info)))
		CAM_DEBUG(
			"[s5k8aa] %s fail copy_from_user!", __func__);

	switch (ctrl_info.mode) {
	case EXT_CAM_EV:
		db8131m_set_ev(ctrl_info.value_1);
		break;

	case EXT_CAM_EFFECT:
		db8131m_set_effect(ctrl_info.value_1);
		break;

	case EXT_CAM_WB:
		db8131m_set_whitebalance(ctrl_info.value_1);
		break;

	case EXT_CAM_DTP_TEST:
		db8131m_check_dataline(ctrl_info.value_1);
		break;

	case  EXT_CAM_MOVIE_MODE:
		CAM_DEBUG("MOVIE mode : %d", ctrl_info.value_1);
		db8131m_ctrl->cam_mode = ctrl_info.value_1;
		break;

	case EXT_CAM_PREVIEW_SIZE:
		db8131m_set_preview_size(ctrl_info.value_1);
		break;

	default:
		CAM_DEBUG("[db8131m] default mode");
		break;
	}

	if (copy_to_user((void *)arg,
		(const void *)&ctrl_info, sizeof(ctrl_info)))
		CAM_DEBUG("[db8131m] %s fail copy_to_user!", __func__);
}


long db8131m_sensor_subdev_ioctl(struct v4l2_subdev *sd,
			unsigned int cmd, void *arg)
{
	void __user *argp = (void __user *)arg;
	CAM_DEBUG("db8131m_sensor_subdev_ioctl\n");
	switch (cmd) {
	case VIDIOC_MSM_SENSOR_CFG:
		return db8131m_sensor_config(argp);
	default:
		return -ENOIOCTLCMD;
	}
}

int db8131m_sensor_config(void __user *argp)
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
			rc = db8131m_set_sensor_mode(cfg_data.mode);
			break;

		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = 0;
			cam_err(" Invalid cfgtype = %d", cfg_data.cfgtype);
			break;
		}
	return rc;
}

#if defined(CONFIG_S5K5CCGX) && defined(CONFIG_DB8131M) /* jasper */
static int db8131m_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG("=== Start ===");

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_5M_RST : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_5M_ISP_INIT : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	usleep(10); /* 20clk = 0.833us */

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);

	/*CAM_MCLK0*/
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);

	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);

	data->sensor_platform_info->sensor_power_off(1);

	msm_camera_request_gpio_table(data, 0);

#ifdef CONFIG_LOAD_FILE
	db8131m_regs_table_exit();
#endif
	return rc;
}
#elif defined(CONFIG_ISX012) && defined(CONFIG_DB8131M) /* Gogh *//* AEGIS2 */
static int db8131m_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG("=== Start ===");

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_5M_RST : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_5M_ISP_INIT : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);
	usleep(1000);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	usleep(1000);

	/*CAM_MCLK0*/
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);

	usleep(1000);

	data->sensor_platform_info->sensor_power_off(1);

	msm_camera_request_gpio_table(data, 0);

#ifdef CONFIG_LOAD_FILE
	db8131m_regs_table_exit();
#endif
	return rc;
}
#elif defined(CONFIG_S5K4ECGX) && defined(CONFIG_DB8131M) /* Aegis2 */
static int db8131m_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG("=== Start ===");

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_5M_RST : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_5M_ISP_INIT : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);

	/*CAM_MCLK0*/
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, &s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);

	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);

	data->sensor_platform_info->sensor_power_off(1);

	msm_camera_request_gpio_table(data, 0);

#ifdef CONFIG_LOAD_FILE
	db8131m_regs_table_exit();
#endif
	return rc;
}
#else
static int db8131m_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	printk(KERN_DEBUG "db8131m_sensor_power_down");
}
#endif

static struct db8131m_format db8131m_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static int db8131m_enum_fmt(struct v4l2_subdev *sd, unsigned int index,
			   enum v4l2_mbus_pixelcode *code) {
	CAM_DEBUG("Index is %d", index);
	if ((unsigned int)index >= ARRAY_SIZE(db8131m_subdev_info))
		return -EINVAL;

	*code = db8131m_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops db8131m_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = db8131m_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops db8131m_subdev_video_ops = {
	.enum_mbus_fmt = db8131m_enum_fmt,
};

static struct v4l2_subdev_ops db8131m_subdev_ops = {
	.core = &db8131m_subdev_core_ops,
	.video  = &db8131m_subdev_video_ops,
};

static int db8131m_i2c_probe(struct i2c_client *client,
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
		goto probe_failure;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s: NULL sensor data\n", __func__);
		return -EFAULT;
	}
	CAM_DEBUG("Sensor name : %s\n", s_ctrl->sensordata->sensor_name);
	db8131m_client = client;
	db8131m_dev = s_ctrl->sensor_i2c_client->client->dev;

	db8131m_ctrl = kzalloc(sizeof(struct db8131m_ctrl), GFP_KERNEL);
	if (!db8131m_ctrl) {
		CAM_DEBUG("db8131m_ctrl alloc failed!\n");
		rc = -ENOMEM;
		goto probe_failure;
	}

	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);

	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		&db8131m_subdev_ops);

	db8131m_ctrl->sensor_dev = &s_ctrl->sensor_v4l2_subdev;
	db8131m_ctrl->sensordata = client->dev.platform_data;

	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);

/* To test i2c
	rc = db8131m_i2c_write_list(db8131m_common,
		sizeof(db8131m_common)/sizeof(db8131m_common[0]),
							"db8131m_common");
	printk("I2c write ret = %d\n", rc);
*/
	cam_err("db8131m_probe succeeded!");
	return 0;

probe_failure:
	cam_err("db8131m_probe failed!");
	return rc;
}

static const struct i2c_device_id db8131m_i2c_id[] = {
	{"db8131m", (kernel_ulong_t)&db8131m_s_ctrl},
	{},
};

static struct msm_camera_i2c_client db8131m_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct i2c_driver db8131m_i2c_driver = {
	.id_table = db8131m_i2c_id,
	.probe  = db8131m_i2c_probe,
	.driver = {
		.name = "db8131m",
	},
};

static int __init db8131m_init(void)
{
	CAM_DEBUG("db8131m init called\n");
	return i2c_add_driver(&db8131m_i2c_driver);
}

static struct msm_sensor_fn_t db8131m_func_tbl = {
	.sensor_config = db8131m_sensor_config,
	.sensor_power_up = db8131m_sensor_power_up,
	.sensor_power_down = db8131m_sensor_power_down,
};


static struct msm_sensor_reg_t db8131m_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_sensor_ctrl_t db8131m_s_ctrl = {
	.msm_sensor_reg = &db8131m_regs,
	.sensor_i2c_client = &db8131m_sensor_i2c_client,
	.sensor_i2c_addr = 0x45,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &db8131m_mut,
	.sensor_i2c_driver = &db8131m_i2c_driver,
	.sensor_v4l2_subdev_info = db8131m_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(db8131m_subdev_info),
	.sensor_v4l2_subdev_ops = &db8131m_subdev_ops,
	.func_tbl = &db8131m_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};


module_init(db8131m_init);
