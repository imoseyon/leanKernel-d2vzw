/*
 *  Copyright (C) 2010, Samsung Electronics Co. Ltd. All Rights Reserved.
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
 */
#include <mach/gpio.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/ctype.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <linux/regulator/consumer.h>
#include <mach/board-t-project-camera.h>
#include <mach/t-project-gpio.h>

static ssize_t back_camera_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_type[] = "SONY_ISX012\n";

	return snprintf(buf, sizeof(cam_type), "%s", cam_type);
}

static ssize_t front_camera_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_type[] = "SLSI_S5K8AA\n";

	return snprintf(buf, sizeof(cam_type), "%s", cam_type);
}

static DEVICE_ATTR(camtype_back, S_IRUGO, back_camera_type_show, NULL);
static DEVICE_ATTR(camtype_front, S_IRUGO, front_camera_type_show, NULL);

static ssize_t cameraflash_file_cmd_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t size)
{
	int value;
	int i = 0;
	int err = 1;

	if (strlen(buf) > 2)
		return -err;

	if (isdigit(*buf))
		kstrtoint(buf, NULL, &value);
	else
		return -err;

	if (value == 0) {
		pr_err("[Factory flash]OFF\n");
		gpio_set_value_cansleep(GPIO_VFE_CAMIF_TIMER1, 0);
		gpio_set_value_cansleep(GPIO_VFE_CAMIF_TIMER2, 0);
	} else {
		pr_err("[Factory flash]ON\n");
		gpio_set_value_cansleep(GPIO_VFE_CAMIF_TIMER1, 0);

		for (i = 5; i > 1; i--) {
			gpio_set_value_cansleep(
				GPIO_VFE_CAMIF_TIMER2, 1);
			udelay(1);
			gpio_set_value_cansleep(
				GPIO_VFE_CAMIF_TIMER2, 0);
			udelay(1);
		}
		gpio_set_value_cansleep(GPIO_VFE_CAMIF_TIMER2, 1);
		usleep(2*1000);
	}

	return size;
}

static DEVICE_ATTR(camflash, S_IRUGO | S_IWUSR | S_IWGRP,
		NULL, cameraflash_file_cmd_store);

void __init msm8960_kenos_cam_init(void)
{
	struct device *cam_dev_back;
	struct device *cam_dev_front;
	struct class *camera_class;

	pr_err("msm8960_kenos_cam_init\n");

	camera_class = class_create(THIS_MODULE, "camera");

	if (IS_ERR(camera_class)) {
		pr_err("Failed to create class(camera)!\n");
		return;
	}
	cam_dev_back = device_create(camera_class, NULL,
		0, NULL, "rear");

	if (IS_ERR(cam_dev_back)) {
		pr_err("Failed to create cam_dev_back device!\n");
		goto OUT5;
	}

	if (device_create_file(cam_dev_back, &dev_attr_camflash) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_camflash.attr.name);
		goto OUT4;
	}

	if (device_create_file(cam_dev_back, &dev_attr_camtype_back) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_camtype_back.attr.name);
		goto OUT3;
	}

	cam_dev_front = device_create(camera_class, NULL,
		1, NULL, "front");

	if (IS_ERR(cam_dev_front)) {
		pr_err("Failed to create cam_dev_front device!");
		goto OUT2;
	}

	if (device_create_file(cam_dev_front, &dev_attr_camtype_front) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_camtype_front.attr.name);
		goto OUT1;
	}

	return;

OUT1:
	device_destroy(camera_class, 1);
OUT2:
	device_remove_file(cam_dev_back, &dev_attr_camtype_back);
OUT3:
	device_remove_file(cam_dev_back, &dev_attr_camflash);
OUT4:
	device_destroy(camera_class, 0);
OUT5:
	return;
}
