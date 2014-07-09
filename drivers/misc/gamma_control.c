/*
 * Copyright 2013 Francisco Franco
 * 	     2014 Reworked for Samsung OLED, Luis Cruz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#define GAMMACONTROL_VERSION 2

		//     r      g      b
int v255_val[3]	= {    0,     0,     0};
int v1_val[3]	= {    0,     0,     0};
int v171_val[3]	= {    0,     0,     0};
int v87_val[3]	= {    0,     0,     0};
#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
int v59_val[3]	= {    0,     0,     0};
int v35_val[3]	= {    0,     0,     0};
int v15_val[3]	= {    0,     0,     0};
#else
int v43_val[3]	= {    0,     0,     0};
int v19_val[3]	= {    0,     0,     0};
#endif
int tuner[3]	= {   60,    60,    60};

int red_tint[7] = {15, 20, 9, 9, 9, 9, 9};
int grn_tint[7] = {15, 20, 9, 9, 9, 9, 9};
int blu_tint[7] = {15, 20, 9, 9, 9, 9, 9};

extern void panel_load_colors(void);

static ssize_t v255_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v255_val[0], v255_val[1], v255_val[2]);
}

static ssize_t v255_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v255_val[0] || new_g != v255_val[1] || new_b != v255_val[2]) {
		pr_debug("New v255: %d %d %d\n", new_r, new_g, new_b);
		v255_val[0] = new_r;
		v255_val[1] = new_g;
		v255_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

static ssize_t v1_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v1_val[0], v1_val[1], v1_val[2]);
}

static ssize_t v1_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v1_val[0] || new_g != v1_val[1] || new_b != v1_val[2]) {
		pr_debug("New v1: %d %d %d\n", new_r, new_g, new_b);
		v1_val[0] = new_r;
		v1_val[1] = new_g;
		v1_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

static ssize_t v171_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v171_val[0], v171_val[1], v171_val[2]);
}

static ssize_t v171_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v171_val[0] || new_g != v171_val[1] || new_b != v171_val[2]) {
		pr_debug("New v171: %d %d %d\n", new_r, new_g, new_b);
		v171_val[0] = new_r;
		v171_val[1] = new_g;
		v171_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

static ssize_t v87_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v87_val[0], v87_val[1], v87_val[2]);
}

static ssize_t v87_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v87_val[0] || new_g != v87_val[1] || new_b != v87_val[2]) {
		pr_debug("New v87: %d %d %d\n", new_r, new_g, new_b);
		v87_val[0] = new_r;
		v87_val[1] = new_g;
		v87_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
static ssize_t v59_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v59_val[0], v59_val[1], v59_val[2]);
}

static ssize_t v59_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v59_val[0] || new_g != v59_val[1] || new_b != v59_val[2]) {
		pr_debug("New v59: %d %d %d\n", new_r, new_g, new_b);
		v59_val[0] = new_r;
		v59_val[1] = new_g;
		v59_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

static ssize_t v35_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v35_val[0], v35_val[1], v35_val[2]);
}

static ssize_t v35_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v35_val[0] || new_g != v35_val[1] || new_b != v35_val[2]) {
		pr_debug("New v35: %d %d %d\n", new_r, new_g, new_b);
		v35_val[0] = new_r;
		v35_val[1] = new_g;
		v35_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

static ssize_t v15_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v15_val[0], v15_val[1], v15_val[2]);
}

static ssize_t v15_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v15_val[0] || new_g != v15_val[1] || new_b != v15_val[2]) {
		pr_debug("New v15: %d %d %d\n", new_r, new_g, new_b);
		v15_val[0] = new_r;
		v15_val[1] = new_g;
		v15_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

#else /* CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT */
static ssize_t v43_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v43_val[0], v43_val[1], v43_val[2]);
}

static ssize_t v43_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v43_val[0] || new_g != v43_val[1] || new_b != v43_val[2]) {
		pr_debug("New v43: %d %d %d\n", new_r, new_g, new_b);
		v43_val[0] = new_r;
		v43_val[1] = new_g;
		v43_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}

static ssize_t v19_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", v19_val[0], v19_val[1], v19_val[2]);
}

static ssize_t v19_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r != v19_val[0] || new_g != v19_val[1] || new_b != v19_val[2]) {
		pr_debug("New v19: %d %d %d\n", new_r, new_g, new_b);
		v19_val[0] = new_r;
		v19_val[1] = new_g;
		v19_val[2] = new_b;
		panel_load_colors();
	}

	return size;
}
#endif /* CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT */

static ssize_t tuner_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d %d %d\n", tuner[0], tuner[1], tuner[2]);
}

#define calc_r_shift(n) \
	(red_tint[n] * (tuner[0] - 60) / 60)
#define calc_g_shift(n) \
	(grn_tint[n] * (tuner[1] - 60) / 60)
#define calc_b_shift(n) \
	(blu_tint[n] * (tuner[2] - 60) / 60)
static ssize_t tuner_store(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
	int new_r, new_g, new_b;

	sscanf(buf, "%d %d %d", &new_r, &new_g, &new_b);

	if (new_r > 120 || new_r < 0 || new_g > 120 || new_g < 0 || new_b > 120 || new_b < 0) {
		new_r = new_g = new_b = 60;
		pr_err("Master tuner out of bounds, reset!\n");
	}

	if (new_r != tuner[0]) {
		tuner[0] = new_r;

		v255_val[0] = calc_r_shift(0);
		v1_val[0] = calc_r_shift(1);
		v171_val[0] = calc_r_shift(2);
		v87_val[0] = calc_r_shift(3);
#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
		v59_val[0] = calc_r_shift(4);
		v35_val[0] = calc_r_shift(5);
		v15_val[0] = calc_r_shift(6);
#else
		v43_val[0] = calc_r_shift(4);
		v19_val[0] = calc_r_shift(5);
#endif
		if (new_g == tuner[1] && new_b == tuner[2])
			goto load_colors;
		if (new_g == tuner[1])
			goto blue;
	}

	if (new_g != tuner[1]) {
		tuner[1] = new_g;

		v255_val[1] = calc_g_shift(0);
		v1_val[1] = calc_g_shift(1);
		v171_val[1] = calc_g_shift(2);
		v87_val[1] = calc_g_shift(3);
#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
		v59_val[1] = calc_g_shift(4);
		v35_val[1] = calc_g_shift(5);
		v15_val[1] = calc_g_shift(6);
#else
		v43_val[1] = calc_g_shift(4);
		v19_val[1] = calc_g_shift(5);
#endif
		if (new_b == tuner[2])
			goto load_colors;
	}

blue:
	if (new_b != tuner[2]) {
		tuner[2] = new_b;

		v255_val[2] = calc_b_shift(0);
		v1_val[2] = calc_b_shift(1);
		v171_val[2] = calc_b_shift(2);
		v87_val[2] = calc_b_shift(3);
#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
		v59_val[2] = calc_b_shift(4);
		v35_val[2] = calc_b_shift(5);
		v15_val[2] = calc_b_shift(6);
#else
		v43_val[2] = calc_b_shift(4);
		v19_val[2] = calc_b_shift(5);
#endif
load_colors:
		pr_debug("New master tuner: %d %d %d\n", new_r, new_g, new_b);
		panel_load_colors();
	}

	return size;
}

static ssize_t gammacontrol_version(struct device * dev, struct device_attribute * attr, char * buf)
{
	return sprintf(buf, "%u\n", GAMMACONTROL_VERSION);
}

static DEVICE_ATTR(v255rgb, 0644, v255_show, v255_store);
static DEVICE_ATTR(v1rgb, 0644, v1_show, v1_store);
static DEVICE_ATTR(v171rgb, 0644, v171_show, v171_store);
static DEVICE_ATTR(v87rgb, 0644, v87_show, v87_store);
#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
static DEVICE_ATTR(v59rgb, 0644, v59_show, v59_store);
static DEVICE_ATTR(v35rgb, 0644, v35_show, v35_store);
static DEVICE_ATTR(v15rgb, 0644, v15_show, v15_store);
#else
static DEVICE_ATTR(v43rgb, 0644, v43_show, v43_store);
static DEVICE_ATTR(v19rgb, 0644, v19_show, v19_store);
#endif
static DEVICE_ATTR(version, 0644, gammacontrol_version, NULL);
static DEVICE_ATTR(tuner, 0644, tuner_show, tuner_store);

static struct attribute *gammacontrol_attributes[] =
{
	&dev_attr_v255rgb.attr,
	&dev_attr_v1rgb.attr,
	&dev_attr_v171rgb.attr,
	&dev_attr_v87rgb.attr,
#if !defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT)
	&dev_attr_v59rgb.attr,
	&dev_attr_v35rgb.attr,
	&dev_attr_v15rgb.attr,
#else
	&dev_attr_v43rgb.attr,
	&dev_attr_v19rgb.attr,
#endif
	&dev_attr_tuner.attr,
	&dev_attr_version.attr,
	NULL
};

static struct attribute_group gammacontrol_group =
{
	.attrs  = gammacontrol_attributes,
};

static struct miscdevice gammacontrol_device =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "gammacontrol",
};

static int __init gammacontrol_init(void)
{
	int ret;

	pr_info("%s misc_register(%s)\n", __FUNCTION__, gammacontrol_device.name);

	ret = misc_register(&gammacontrol_device);

	if (ret) {
		pr_err("%s misc_register(%s) fail\n", __FUNCTION__, gammacontrol_device.name);
		return 1;
	}

	if (sysfs_create_group(&gammacontrol_device.this_device->kobj, &gammacontrol_group) < 0) {
		pr_err("%s sysfs_create_group fail\n", __FUNCTION__);
		pr_err("Failed to create sysfs group for device (%s)!\n", gammacontrol_device.name);
	}

	return 0;
}

device_initcall(gammacontrol_init);
