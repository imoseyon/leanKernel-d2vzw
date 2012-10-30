/*
 * include/linux/synaptics_s7301.h
 *
 * Copyright (C) 2012 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LINUX_SYNAPTICS_TS_H
#define _LINUX_SYNAPTICS_TS_H

#define SYNAPTICS_TS_NAME "synaptics-ts"


/**
 * struct synaptics_platform_data - represent specific touch device
 * @model_name : name of device name
 * @panel_name : name of sensor panel name
 * @rx_channel_no : receive channel number of touch sensor
 * @tx_channel_no : transfer channel number of touch sensor
 * @x_pixel_size : maximum of x axis pixel size
 * @y_pixel_size : maximum of y axis pixel size
 * @ta_state : represent of charger connect state
 * @link : pointer that represent touch screen driver struct
 * @gpio_set : physical gpios used by touch screen IC
 * @set_ta_mode : callback function when TA, USB connected or disconnected
 * @set_power : control touch screen IC power gpio pin
 */
struct synaptics_platform_data {
	char *model_name;
	char *panel_name;
	int rx_channel_no;
	int tx_channel_no;
	int x_pixel_size;
	int y_pixel_size;
	int ta_state;
	void *link;
	struct gpio *gpio_set;
	void (*set_ta_mode)(int *);
	void (*set_power)(bool);
};
extern struct class *sec_class;

#endif
