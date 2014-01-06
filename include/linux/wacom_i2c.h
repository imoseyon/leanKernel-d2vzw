/*
 *  wacom_i2c.h
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _LINUX_WACOM_I2C_H
#define _LINUX_WACOM_I2C_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/wakelock.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define NAMEBUF				12
#define MAX_RETRY				3

#define WACNAME				"WAC_I2C_EMR"

/* Wacom Data Size */
#define COM_COORD_NUM			7
#define COM_QUERY_NUM			9

/* Wacom Command */
#define COM_SAMPLERATE_STOP	0x30
#define COM_QUERY				0x2A
#define COM_SURVEYSCAN			0x2B
#define COM_SAMPLERATE_40		0x33
#define COM_SAMPLERATE_80		0x32
#define COM_SAMPLERATE_133		0x31
#define COM_CHECKSUM			0x63
#define COM_I2C_GRID_CHECK		0xC9
#define COM_STATUS				0xD8
#define COM_FLASH				0xFF

/* Additional Switch */
#define SW_PEN_INSERT			0x0e

/* Special keys */
#define KEY_PEN_PDCT			0x230

#define WACOM_BOOT_DELAY_MS	300
#define WACOM_QUERY_DELAY_MS	100
#define WACOM_OFF_DELAY_MS		50
#define WACOM_CSUM_DELAY_MS	600

/* PDCT Signal */
#define PDCT_NOSIGNAL			1
#define PDCT_DETECT_PEN		0

/* Checksum Header */
#define WACOM_CSUM_HEADER		0x1F

/*Parameters for wacom own features*/
struct wacom_features {
	int x_max;
	int y_max;
	int pressure_max;
	u8 data[COM_QUERY_NUM];
	unsigned int fw_version;
	int firm_update_status;
};

/*sec_class sysfs*/
extern struct class *sec_class;

struct wacom_platform_data {
	bool x_invert;
	bool y_invert;
	bool xy_switch;
	bool boot_on;
	int gpio_pendct;
	int gpio_pen_insert;
	int gpio_pen_fwe1;
	u16 boot_addr;
	char *binary_fw_path;
	char *file_fw_path;
	unsigned int fw_version;
	u8 fw_checksum[5];
	void (*power)(bool on);
};

/*Parameters for i2c driver*/
struct wacom_i2c {
	struct i2c_client *client;
	struct i2c_client *client_boot;
	struct input_dev *input_dev;
	struct early_suspend early_suspend;
	struct mutex lock;
	struct device	*dev;
	int irq;
	int irq_pdct;
	bool rdy_pdct;
	bool pen_pdct;
	bool pen_prox;
	bool pen_pressed;
	bool side_pressed;
	int tool;
	u16 last_x;
	u16 last_y;
	bool pen_insert;
	int irq_pen_insert;
	bool checksum_result;
	const char name[NAMEBUF];
	struct wacom_features *wac_feature;
	struct wacom_platform_data *wac_pdata;
	void (*power)(bool on);
	struct delayed_work resume_work;
	bool connection_check;
	bool battery_saving_mode;
	bool power_enable;
	bool epen_reset_result;
	char *fw_bin;
};

extern int wacom_i2c_send(struct wacom_i2c *wac_i2c,
			  const char *buf, int count, bool mode);
extern int wacom_i2c_recv(struct wacom_i2c *wac_i2c,
			char *buf, int count, bool mode);

#endif /* _LINUX_WACOM_I2C_H */
