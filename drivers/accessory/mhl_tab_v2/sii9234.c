/*
 * Copyright (C) 2011 Samsung Electronics
 *
 * Authors: Adam Hampson <ahampson@sta.samsung.com>
 *          Erik Gilling <konkers@android.com>
 *
 * Additional contributions by : Shankar Bandal <shankar.b@samsung.com>
 *                               Dharam Kumar <dharam.kr@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/sii9234.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>

#include "sii9234_driver.h"

#ifdef CONFIG_MHL_SWING_LEVEL
#include <linux/ctype.h>

static ssize_t sii9234_swing_test_show(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sii9234_data *sii9234 = dev_get_drvdata(sii9244_mhldev);
	return sprintf(buf, "mhl_show_value : 0x%x\n",
					sii9234->pdata->swing_level);

}
static ssize_t sii9234_swing_test_store(struct device *dev,
				struct device_attribute *attr,
					const char *buf, size_t size)
{
	struct sii9234_data *sii9234 = dev_get_drvdata(sii9244_mhldev);
	char temp[4] = { 0, };
	const char *p = buf;
	int data, clk;
	unsigned int value;

	while (*p != '\0') {
		if (!isspace(*p))
			strncat(temp, p, 1);
		p++;
	}

	if (strlen(temp) != 2)
		return -EINVAL;

	kstrtoul(temp, 10, &value);
	data = value / 10;
	clk = value % 10;
	sii9234->pdata->swing_level = 0xc0;
	sii9234->pdata->swing_level = sii9234->pdata->swing_level
							| (data << 3) | clk;
	sprintf(buf, "mhl_store_value : 0x%x\n", sii9234->pdata->swing_level);
	return size;
}

static CLASS_ATTR(swing, 0664,
		sii9234_swing_test_show, sii9234_swing_test_store);
#endif /*CONFIG_MHL_SWING_LEVEL*/
static int mhl_tx_write_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 value)
{
	int ret;
	ret = i2c_smbus_write_byte_data(sii9234->pdata->mhl_tx_client, offset,
			value);
	if (ret < 0)
		pr_err("[ERROR] sii9234 : %s(0x%02x, 0x%02x)\n", __func__,
				offset, value);
	return ret;
}

static int mhl_tx_read_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 *value)
{
	int ret;

	if (!value)
		return -EINVAL;

	ret = i2c_smbus_write_byte(sii9234->pdata->mhl_tx_client, offset);
	if (ret < 0) {
		pr_err("[ERROR] sii9234 : %s(0x%02x)\n", __func__, offset);
		return ret;
	}

	ret = i2c_smbus_read_byte(sii9234->pdata->mhl_tx_client);
	if (ret < 0) {
		pr_err("[ERROR] sii9234 : %s(0x%02x)\n", __func__, offset);
		return ret;
	}

	*value = ret & 0x000000FF;

	return 0;
}


static int mhl_tx_clear_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 mask)
{
	int ret;
	u8 value;

	ret = mhl_tx_read_reg(sii9234, offset, &value);
	if (ret < 0) {
		pr_err("[ERROR] sii9234 : %s(0x%02x, 0x%02x)\n", __func__,
				offset, mask);
		return ret;
	}

	value &= ~mask;

	ret = mhl_tx_write_reg(sii9234, offset, value);
	if (ret < 0)
		pr_err("[ERROR] sii9234 : %s(0x%02x, 0x%02x)\n", __func__,
				offset, mask);
	return ret;
}

static int tpi_write_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 value)
{
	int ret = 0;
	ret = i2c_smbus_write_byte_data(sii9234->pdata->tpi_client, offset,
			value);
	if (ret < 0)
		pr_err("[ERROR] sii9234 : %s(0x%02x, 0x%02x)\n", __func__,
				offset, value);
	return ret;
}


static int hdmi_rx_write_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 value)
{
	int ret;
	ret = i2c_smbus_write_byte_data(sii9234->pdata->hdmi_rx_client, offset,
			value);
	if (ret < 0)
		pr_err("[ERROR] sii9234 : %s(0x%02x, 0x%02x)\n", __func__,
				offset, value);
	return ret;
}

static int cbus_write_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 value)
{
	return i2c_smbus_write_byte_data(sii9234->pdata->cbus_client, offset,
			value);
}

static int cbus_read_reg(struct sii9234_data *sii9234, unsigned int offset,
		u8 *value)
{
	int ret;

	if (!value)
		return -EINVAL;

	ret = i2c_smbus_write_byte(sii9234->pdata->cbus_client, offset);
	if (ret < 0) {
		pr_err("[ERROR] sii9234 : %s(0x%02x)\n", __func__, offset);
		return ret;
	}

	ret = i2c_smbus_read_byte(sii9234->pdata->cbus_client);
	if (ret < 0) {
		pr_err("[ERROR] sii9234 : %s(0x%02x)\n", __func__, offset);
		return ret;
	}

	*value = ret & 0x000000FF;

	return 0;
}




static void sii9234_power_down(struct sii9234_data *sii9234)
{
	if (sii9234->pdata->vbus_present)
		sii9234->pdata->vbus_present(false);

	tpi_write_reg(sii9234, TPI_DPD_REG, 0);
	/*turn on&off hpd festure for only QCT HDMI*/
}



static int sii9234_cbus_init_for_9290(struct sii9234_data *sii9234)
{
	int ret = 0;

	ret = cbus_write_reg(sii9234, 0x1F, 0x02);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x07, 0x30 | 0x06);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x40, 0x03);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x42, 0x06);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x36, 0x0C);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x3D, 0xFD);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x1C, 0x00);
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x44, 0x00);

	return ret;
}


static int sii9234_30pin_reg_init_for_9290(struct sii9234_data *sii9234)
{
	int ret = 0;
	u8 value;
	pr_info("[: %s]++\n", __func__);
	ret = tpi_write_reg(sii9234, 0x3D, 0x3F);
	if (ret < 0)
		return ret;

	ret = hdmi_rx_write_reg(sii9234, 0x11, 0x01);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x12, 0x15);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0x08, 0x35);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x00, 0x00);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x13, 0x60);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x14, 0xF0);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x4B, 0x06);
	if (ret < 0)
		return ret;

	/* Analog PLL Control */
	ret = hdmi_rx_write_reg(sii9234, 0x17, 0x07);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x1A, 0x20);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x22, 0xE0);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x23, 0xC0);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x24, 0xA0);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x25, 0x80);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x26, 0x60);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x27, 0x40);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x28, 0x20);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x29, 0x00);
	if (ret < 0)
		return ret;

	ret = hdmi_rx_write_reg(sii9234, 0x4D, 0x02);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x4C, 0xA0);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0x80, 0x34);
	if (ret < 0)
		return ret;

	ret = hdmi_rx_write_reg(sii9234, 0x31, 0x0B);
	if (ret < 0)
		return ret;
	ret = hdmi_rx_write_reg(sii9234, 0x45, 0x06);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0xA0, 0xD0);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0xA1, 0xFC);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0xA3 /*MHL_TX_MHLTX_CTL4_REG*/,
					sii9234->pdata->swing_level);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0xA6, 0x00);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0x2B, 0x01);
	if (ret < 0)
		return ret;

	/* CBUS & Discovery */
	ret = mhl_tx_read_reg(sii9234, 0x90/*MHL_TX_DISC_CTRL1_REG*/, &value);
	if (ret < 0)
		return ret;
	value &= ~(1<<2);
	value |= (1<<3);
	ret = mhl_tx_write_reg(sii9234, 0x90 /*MHL_TX_DISC_CTRL1_REG*/, value);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0x91, 0xE5);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0x94, 0x66);
	if (ret < 0)
		return ret;

	ret = cbus_read_reg(sii9234, 0x31, &value);
	if (ret < 0)
		return ret;
	value |= 0x0C;
	if (ret < 0)
		return ret;
	ret = cbus_write_reg(sii9234, 0x31, value);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0xA5, 0x80);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0x95, 0x31);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0x96, 0x22);
	if (ret < 0)
		return ret;

	ret = mhl_tx_read_reg(sii9234, 0x95/*MHL_TX_DISC_CTRL6_REG*/, &value);
	if (ret < 0)
		return ret;
	value |= (1<<6);
	ret = mhl_tx_write_reg(sii9234,  0x95/*MHL_TX_DISC_CTRL6_REG*/, value);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0x92, 0x46);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0x93, 0xDC);
	if (ret < 0)
		return ret;
	/*0x79=MHL_TX_INT_CTRL_REG*/
	ret = mhl_tx_clear_reg(sii9234, 0x79, (1<<2) | (1<<1));
	if (ret < 0)
		return ret;

	mdelay(25);
	/*0x95=MHL_TX_DISC_CTRL6_REG*/
	ret = mhl_tx_clear_reg(sii9234,  0x95, (1<<6)/*USB_ID_OVR*/);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0x90, 0x27);
	if (ret < 0)
		return ret;

	ret = sii9234_cbus_init_for_9290(sii9234);
	if (ret < 0)
		return ret;

	ret = mhl_tx_write_reg(sii9234, 0x05, 0x4);
	if (ret < 0)
		return ret;
	ret = mhl_tx_write_reg(sii9234, 0x0D, 0x1C);
	pr_info("[MHD: %s]--\n", __func__);
	return ret;
}

static int sii9234_30pin_init_for_9290(struct sii9234_data *sii9234)
{
	u8 value;
	int ret = 0;
	pr_info("[MHD: %s]++\n", __func__);
	/* init registers */
	ret = sii9234_30pin_reg_init_for_9290(sii9234);
	if (ret < 0)
		goto unhandled;

	/* start tpi */
	ret = mhl_tx_write_reg(sii9234, 0xC7, 0x00);
	if (ret < 0)
		goto unhandled;

	/* enable interrupts */
	ret = mhl_tx_write_reg(sii9234, 0xBC, 0x01);
	if (ret < 0)
		goto unhandled;
	ret = mhl_tx_write_reg(sii9234, 0xBD, 0x78);
	if (ret < 0)
		goto unhandled;
	ret = mhl_tx_write_reg(sii9234, 0xBE, 0x01);
	if (ret < 0)
		goto unhandled;

	/* mhd rx connected */
	ret = mhl_tx_write_reg(sii9234, 0xBC, 0x01);
	if (ret < 0)
		goto unhandled;
	ret = mhl_tx_write_reg(sii9234, 0xBD, 0xA0);
	if (ret < 0)
		goto unhandled;
	ret = mhl_tx_write_reg(sii9234, 0xBE, 0x10);
	if (ret < 0)
		goto unhandled;
	ret = cbus_write_reg(sii9234, 0x07, 0x30 | 0x0E);
	if (ret < 0)
		goto unhandled;
	ret = cbus_write_reg(sii9234, 0x47, 0x03);
	if (ret < 0)
		goto unhandled;
	ret = cbus_write_reg(sii9234, 0x21, 0x01);
	if (ret < 0)
		goto unhandled;

	/* enable mhd tx */
	ret = mhl_tx_clear_reg(sii9234, 0x1A, 1<<4);
	if (ret < 0)
		goto unhandled;

	/* set mhd power active mode */
	ret = mhl_tx_clear_reg(sii9234, 0x1E, 1<<1 | 1<<0);
	if (ret < 0)
		goto unhandled;

	ret = mhl_tx_write_reg(sii9234, 0xBC, 0x01);
	if (ret < 0)
		goto unhandled;
	ret = mhl_tx_write_reg(sii9234, 0xBD, 0xA0);
	if (ret < 0)
		goto unhandled;

	ret = mhl_tx_read_reg(sii9234, 0xBE, &value);
	if (ret < 0)
		goto unhandled;
	if ((value & (1<<7 | 1<<6)) != 0x00) {
		/* Assert Mobile HD FIFO Reset */
		ret = mhl_tx_write_reg(sii9234, 0xBC, 0x01);
		if (ret < 0)
			goto unhandled;
		ret = mhl_tx_write_reg(sii9234, 0xBD, 0x05);
		if (ret < 0)
			goto unhandled;
		ret = mhl_tx_write_reg(sii9234, 0xBE, (1<<4 | 0x04));
		if (ret < 0)
			goto unhandled;
		mdelay(1);
		/* Deassert Mobile HD FIFO Reset */
		ret = mhl_tx_write_reg(sii9234, 0xBC, 0x01);
		if (ret < 0)
			goto unhandled;
		ret = mhl_tx_write_reg(sii9234, 0xBD, 0x05);
		if (ret < 0)
			goto unhandled;
		ret = mhl_tx_write_reg(sii9234, 0xBE, 0x04);
		if (ret < 0)
			goto unhandled;
	}

	/* This is tricky but there's no way to handle other accessories
	 * but sending UNHANDLED.
	 * return MHL_CON_HANDLED;
	 */
	 pr_info("[MHD: %s]--\n", __func__);
	return 0;
unhandled:
	return -1;
}


u8 mhl_onoff_ex(bool onoff)
{
	struct sii9234_data *sii9234 = dev_get_drvdata(sii9244_mhldev);

	if (!sii9234 || !sii9234->pdata) {
		pr_info("mhl_onoff_ex: getting resource is failed\n");
		return 2;
	}

	if (sii9234->pdata->power_state == onoff) {
		pr_info("mhl_onoff_ex: mhl already %s\n", onoff ? "on" : "off");
		return 2;
	}

	sii9234->pdata->power_state = onoff; /*save power state*/

	if (onoff) {
		if (sii9234->pdata->hw_onoff)
			sii9234->pdata->hw_onoff(1);

		if (sii9234->pdata->hw_reset)
			sii9234->pdata->hw_reset();

		mutex_lock(&sii9234->lock);
		sii9234_30pin_init_for_9290(sii9234);
		/*turn on hdm corei*/
		mhl_hpd_handler(true);
		mutex_unlock(&sii9234->lock);

	} else {

		mutex_lock(&sii9234->lock);
		/*turn off hdmi core*/
		mhl_hpd_handler(false);
		sii9234_power_down(sii9234);
		mutex_unlock(&sii9234->lock);

		if (sii9234->pdata->hw_onoff)
			sii9234->pdata->hw_onoff(0);
	}

	return sii9234->rgnd;
}
EXPORT_SYMBOL(mhl_onoff_ex);


#ifdef MHL_SS_FACTORY
#define SII_ID 0x92
static ssize_t sysfs_check_mhl_command(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int size;
	u8 sii_id1 = 0;
	u8 sii_id2 = 0;
	struct sii9234_data *sii9234 = dev_get_drvdata(sii9244_mhldev);

	if (sii9234->pdata->hw_onoff)
		sii9234->pdata->hw_onoff(1);

	if (sii9234->pdata->hw_reset)
		sii9234->pdata->hw_reset();

	mhl_tx_read_reg(sii9234, MHL_TX_IDH_REG, &sii_id1);
	mhl_tx_read_reg(sii9234, MHL_TX_IDL_REG, &sii_id2);
	pr_info("sel_show sii_id: %X%X\n", sii_id1, sii_id2);

	if (sii9234->pdata->hw_onoff)
		sii9234->pdata->hw_onoff(0);

	size = snprintf(buf, 10, "%d\n", sii_id1 == SII_ID ? 1 : 0);

	return size;

}

static CLASS_ATTR(test_result, 0664 , sysfs_check_mhl_command, NULL);
#endif /*MHL_SS_FACTORY*/

#ifdef CONFIG_PM
static int sii9234_mhl_tx_suspend(struct device *dev)
{
	struct sii9234_data *sii9234 = dev_get_drvdata(dev);

	/*set config_gpio for mhl*/
	if (sii9234->pdata->gpio_cfg)
		sii9234->pdata->gpio_cfg();

	return 0;
}

static int sii9234_mhl_tx_resume(struct device *dev)
{
	struct sii9234_data *sii9234 = dev_get_drvdata(dev);

	/*set config_gpio for mhl*/
	if (sii9234->pdata->gpio_cfg)
		sii9234->pdata->gpio_cfg();

	return 0;
}

static const struct dev_pm_ops sii9234_pm_ops = {
	.suspend        = sii9234_mhl_tx_suspend,
	.resume         = sii9234_mhl_tx_resume,
};
#endif

static int __devinit sii9234_mhl_tx_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct sii9234_data *sii9234;
	int ret = 0;
#ifdef MHL_SS_FACTORY
	struct class *sec_mhl;
#endif
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -EIO;

	sii9234 = kzalloc(sizeof(struct sii9234_data), GFP_KERNEL);
	if (!sii9234) {
		dev_err(&client->dev, "failed to allocate driver data\n");
		return -ENOMEM;
	}

	sii9234->pdata = client->dev.platform_data;
	if (!sii9234->pdata) {
		ret = -EINVAL;
		goto err_exit1;
	}
	sii9234->pdata->mhl_tx_client = client;


	mutex_init(&sii9234->lock);


	i2c_set_clientdata(client, sii9234);
	sii9244_mhldev = &client->dev;
	if (sii9234->pdata->swing_level == 0)
		sii9234->pdata->swing_level = 0xEB;


#ifdef MHL_SS_FACTORY
	pr_info("create mhl sysfile\n");

	sec_mhl = class_create(THIS_MODULE, "mhl");
	if (IS_ERR(sec_mhl)) {
		pr_err("Failed to create class(sec_mhl)!\n");
		goto err_class;
	}

	ret = class_create_file(sec_mhl, &class_attr_test_result);
	if (ret) {
		pr_err("[ERROR] Failed to create device file in sysfs entries!\n");
		goto err_exit2a;
	}
#endif
#ifdef CONFIG_MHL_SWING_LEVEL
	pr_info("create mhl sysfile\n");

	ret = class_create_file(sec_mhl, &class_attr_swing);
	if (ret) {
		pr_err("[ERROR] failed to create swing sysfs file\n");
		goto err_exit2a;
	}
#endif
	return 0;

err_class:
#ifdef CONFIG_MHL_SWING_LEVEL
	class_remove_file(sec_mhl, &class_attr_swing);
#endif

err_exit2a:
#if defined(MHL_SS_FACTORY) || defined(CONFIG_MHL_SWING_LEVEL)
	class_destroy(sec_mhl);
#endif

err_exit1:
	kfree(sii9234);
	return ret;
}

static int __devinit sii9234_tpi_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct sii9234_platform_data *pdata = client->dev.platform_data;
	if (!pdata)
		return -EINVAL;
	pdata->tpi_client = client;
	return 0;
}

static int __devinit sii9234_hdmi_rx_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct sii9234_platform_data *pdata = client->dev.platform_data;
	if (!pdata)
		return -EINVAL;

	pdata->hdmi_rx_client = client;
	return 0;
}

static int __devinit sii9234_cbus_i2c_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct sii9234_platform_data *pdata = client->dev.platform_data;
	if (!pdata)
		return -EINVAL;

	pdata->cbus_client = client;
	return 0;
}

static int __devexit sii9234_mhl_tx_remove(struct i2c_client *client)
{
	return 0;
}

static int __devexit sii9234_tpi_remove(struct i2c_client *client)
{
	return 0;
}

static int __devexit sii9234_hdmi_rx_remove(struct i2c_client *client)
{
	return 0;
}

static int __devexit sii9234_cbus_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id sii9234_mhl_tx_id[] = {
	{"sii9234_mhl_tx", 0},
	{}
};

static const struct i2c_device_id sii9234_tpi_id[] = {
	{"sii9234_tpi", 0},
	{}
};

static const struct i2c_device_id sii9234_hdmi_rx_id[] = {
	{"sii9234_hdmi_rx", 0},
	{}
};

static const struct i2c_device_id sii9234_cbus_id[] = {
	{"sii9234_cbus", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, sii9234_mhl_tx_id);
MODULE_DEVICE_TABLE(i2c, sii9234_tpi_id);
MODULE_DEVICE_TABLE(i2c, sii9234_hdmi_rx_id);
MODULE_DEVICE_TABLE(i2c, sii9234_cbus_id);

static struct i2c_driver sii9234_mhl_tx_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "sii9234_mhl_tx",
#ifdef CONFIG_PM
		.pm	= &sii9234_pm_ops,
#endif
	},
	.id_table = sii9234_mhl_tx_id,
	.probe = sii9234_mhl_tx_i2c_probe,
	.remove = __devexit_p(sii9234_mhl_tx_remove),
	.command = NULL,
};

static struct i2c_driver sii9234_tpi_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "sii9234_tpi",
	},
	.id_table = sii9234_tpi_id,
	.probe = sii9234_tpi_i2c_probe,
	.remove = __devexit_p(sii9234_tpi_remove),
};

static struct i2c_driver sii9234_hdmi_rx_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "sii9234_hdmi_rx",
	},
	.id_table	= sii9234_hdmi_rx_id,
	.probe	= sii9234_hdmi_rx_i2c_probe,
	.remove	= __devexit_p(sii9234_hdmi_rx_remove),
};

static struct i2c_driver sii9234_cbus_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "sii9234_cbus",
	},
	.id_table = sii9234_cbus_id,
	.probe = sii9234_cbus_i2c_probe,
	.remove = __devexit_p(sii9234_cbus_remove),
};

static int __init sii9234_init(void)
{
	int ret;

	ret = i2c_add_driver(&sii9234_mhl_tx_i2c_driver);
	if (ret < 0)
		return ret;

	ret = i2c_add_driver(&sii9234_tpi_i2c_driver);
	if (ret < 0)
		goto err_exit1;

	ret = i2c_add_driver(&sii9234_hdmi_rx_i2c_driver);
	if (ret < 0)
		goto err_exit2;

	ret = i2c_add_driver(&sii9234_cbus_i2c_driver);
	if (ret < 0)
		goto err_exit3;

	return 0;

err_exit3:
	i2c_del_driver(&sii9234_hdmi_rx_i2c_driver);
err_exit2:
	i2c_del_driver(&sii9234_tpi_i2c_driver);
err_exit1:
	i2c_del_driver(&sii9234_mhl_tx_i2c_driver);
	pr_err("i2c_add_driver fail\n");
	return ret;
}

static void __exit sii9234_exit(void)
{
	i2c_del_driver(&sii9234_cbus_i2c_driver);
	i2c_del_driver(&sii9234_hdmi_rx_i2c_driver);
	i2c_del_driver(&sii9234_tpi_i2c_driver);
	i2c_del_driver(&sii9234_mhl_tx_i2c_driver);
}

module_init(sii9234_init);
module_exit(sii9234_exit);
