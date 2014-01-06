/*
 *  wacom_i2c.c - Wacom Digitizer Controller Driver(I2C bus)
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

#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/wacom_i2c.h>
#include <linux/earlysuspend.h>
#include <linux/uaccess.h>
#include <linux/firmware.h>
#include "wacom_i2c_flash.h"

#define WACOM_DEBUG_LOG 0

static void wacom_i2c_release_input(struct wacom_i2c *wac_i2c)
{
	input_report_abs(wac_i2c->input_dev, ABS_X, wac_i2c->last_x);
	input_report_abs(wac_i2c->input_dev, ABS_Y, wac_i2c->last_y);
	input_report_abs(wac_i2c->input_dev, ABS_PRESSURE, 0);
	input_report_key(wac_i2c->input_dev, BTN_STYLUS, false);
	input_report_key(wac_i2c->input_dev, BTN_TOUCH, false);
	input_report_key(wac_i2c->input_dev, BTN_TOOL_RUBBER, false);
	input_report_key(wac_i2c->input_dev, BTN_TOOL_PEN, false);
	input_report_key(wac_i2c->input_dev, KEY_PEN_PDCT, false);
	input_sync(wac_i2c->input_dev);

	wac_i2c->last_x = 0;
	wac_i2c->last_y = 0;
	wac_i2c->pen_prox = false;
	wac_i2c->pen_pressed = false;
	wac_i2c->side_pressed = false;
	wac_i2c->pen_pdct = false;
}

static void wacom_i2c_report_pdct(struct wacom_i2c *wac_i2c, bool detect)
{
	wac_i2c->rdy_pdct = detect;
	input_report_key(wac_i2c->input_dev, KEY_PEN_PDCT, detect);
	input_sync(wac_i2c->input_dev);
}

int wacom_i2c_send(struct wacom_i2c *wac_i2c,
			  const char *buf, int count, bool mode)
{
	struct i2c_client *client = mode ?
		wac_i2c->client : wac_i2c->client_boot;

	return i2c_master_send(client, buf, count);
}

int wacom_i2c_recv(struct wacom_i2c *wac_i2c,
			char *buf, int count, bool mode)
{
	struct i2c_client *client = mode ?
		wac_i2c->client : wac_i2c->client_boot;

	return i2c_master_recv(client, buf, count);
}

static void wacom_open_test(struct wacom_i2c *wac_i2c)
{
	u8 cmd = 0;
	u8 buf[2] = {0,};
	int ret = 0, cnt = MAX_RETRY;

	cmd = COM_SAMPLERATE_STOP;
	ret = wacom_i2c_send(wac_i2c, &cmd, 1, true);
	if (ret <= 0) {
		pr_err("wacom: failed to send stop command\n");
		return ;
	}

	cmd = COM_I2C_GRID_CHECK;
	ret = wacom_i2c_send(wac_i2c, &cmd, 1, true);
	if (ret <= 0) {
		pr_err("wacom: failed to send grid check command\n");
		goto grid_check_error;
	}

	cmd = COM_STATUS;
	do {
		msleep(1000);
		if (wacom_i2c_send(wac_i2c, &cmd, 1, true)) {
			if (2 == wacom_i2c_recv(wac_i2c,
						buf, 2, true)) {
				if (0 != buf[0])
					break;
			}
		}
	} while (cnt--);

	wac_i2c->connection_check = (1 == buf[0]);
	pr_info("wacom: epen_connection : %d, %d(%d)\n",
	       buf[0], buf[1], cnt);

grid_check_error:
	cmd = COM_SAMPLERATE_STOP;
	wacom_i2c_send(wac_i2c, &cmd, 1, true);

	cmd = COM_SAMPLERATE_133;
	wacom_i2c_send(wac_i2c, &cmd, 1, true);

}

static int wacom_checksum(struct wacom_i2c *wac_i2c)
{
	int ret = 0, retry = MAX_RETRY;
	int i = 0;
	u8 buf[5] = {0, };

	buf[0] = COM_CHECKSUM;

	while (retry--) {
		ret = wacom_i2c_send(wac_i2c, &buf[0], 1, true);
		if (ret < 0) {
			pr_err("wacom: i2c fail, retry, %d, return : %d\n",
			       __LINE__, ret);
			continue;
		}

		msleep(WACOM_CSUM_DELAY_MS);
		ret = wacom_i2c_recv(wac_i2c, buf, 5, true);
		if (ret < 0) {
			pr_err("wacom: i2c fail, retry, %d, return : %d\n",
			       __LINE__, ret);
			continue;
		} else if (buf[0] == WACOM_CSUM_HEADER)
			break;
		pr_err("wacom: checksum retry\n");
	}

	if (ret >= 0) {
		pr_info("wacom: received checksum %x, %x, %x, %x, %x\n",
		       buf[0], buf[1], buf[2], buf[3], buf[4]);
	}

	for (i = 0; i < 5; ++i) {
		if (buf[i] != wac_i2c->wac_pdata->fw_checksum[i]) {
			pr_err("wacom: checksum fail %dth %x %x\n", i,
			       buf[i], wac_i2c->wac_pdata->fw_checksum[i]);
			break;
		}
	}

	wac_i2c->checksum_result = (5 == i);

/* TBD: wacom should implement grid check command */
/*	wacom_open_test(wac_i2c); */

	return ret;
}

static int wacom_i2c_query(struct wacom_i2c *wac_i2c)
{
	struct wacom_features *wac_feature = wac_i2c->wac_feature;
	int ret;
	u8 buf;
	u8 data[9] = {0, };
	int i = 0;

	buf = COM_QUERY;

	for (i = 0; i < MAX_RETRY; i++) {
		ret = wacom_i2c_send(wac_i2c, &buf, 1, true);
		if (ret < 0) {
			pr_err("wacom: I2C send failed(%d)\n", ret);
			continue;
		}
		msleep(WACOM_QUERY_DELAY_MS);
		ret = wacom_i2c_recv(wac_i2c, data, COM_QUERY_NUM, true);
		if (ret < 0) {
			pr_err("wacom: I2C recv failed(%d)\n", ret);
			continue;
		}
		pr_info("wacom: %s: %dth ret of wacom query=%d\n",
		       __func__, i, ret);
		if (COM_QUERY_NUM == ret) {
			if (0x0f == data[0]) {
				wac_feature->fw_version =
					((u16) data[7] << 8) + (u16) data[8];
				break;
			} else {
				pr_info("wacom: query data is not valid\n");
			}
		}
	}

	if (wac_i2c->wac_pdata->xy_switch) {
		wac_feature->y_max = ((u16) data[1] << 8) + (u16) data[2];
		wac_feature->x_max = ((u16) data[3] << 8) + (u16) data[4];
	} else {
		wac_feature->x_max = ((u16) data[1] << 8) + (u16) data[2];
		wac_feature->y_max = ((u16) data[3] << 8) + (u16) data[4];
	}
	wac_feature->pressure_max = (u16) data[6] + ((u16) data[5] << 8);

	pr_info("wacom: x_max=0x%X\n", wac_feature->x_max);
	pr_info("wacom: y_max=0x%X\n", wac_feature->y_max);
	pr_info("wacom: pressure_max=0x%X\n",
	       wac_feature->pressure_max);
	pr_info("wacom: fw_version=0x%X (d7:0x%X,d8:0x%X)\n",
	       wac_feature->fw_version, data[7], data[8]);
	pr_info("wacom: %X, %X, %X, %X, %X, %X, %X, %X, %X\n",
	       data[0], data[1], data[2], data[3], data[4], data[5], data[6],
	       data[7], data[8]);

	if ((i == MAX_RETRY) && (ret < 0))
		return ret;

	return 0;
}

static int wacom_i2c_coord(struct wacom_i2c *wac_i2c)
{
	bool prox = false;
	int ret = 0;
	u8 *data;
	int rubber, stylus;
	static u16 x, y, pressure;
	static u16 tmp;
	int rdy = 0;

	data = wac_i2c->wac_feature->data;
	ret = wacom_i2c_recv(wac_i2c, data, COM_COORD_NUM, true);

	if (ret < 0) {
		pr_err("wacom: %s failed to read i2c.L%d\n", __func__,
		       __LINE__);
		return -1;
	}

	#if WACOM_DEBUG_LOG
		pr_info("[E-PEN] %x, %x, %x, %x, %x, %x, %x\n",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
	#endif

	if (data[0] & 0x80) {
		if (!wac_i2c->pen_prox) {
			wac_i2c->pen_prox = true;
			pr_info("wacom: pdct %d(%d)\n",
					wac_i2c->pen_pdct, wac_i2c->pen_prox);

			if (data[0] & 0x40)
				wac_i2c->tool = BTN_TOOL_RUBBER;
			else
				wac_i2c->tool = BTN_TOOL_PEN;
			#if WACOM_DEBUG_LOG
				pr_info("[E-PEN] is in(%d)\n", wac_i2c->tool);
			#endif
		}

		prox = !!(data[0] & 0x10);
		stylus = !!(data[0] & 0x20);
		rubber = !!(data[0] & 0x40);
		rdy = !!(data[0] & 0x80);

		x = ((u16) data[1] << 8) + (u16) data[2];
		y = ((u16) data[3] << 8) + (u16) data[4];
		pressure = ((u16) data[5] << 8) + (u16) data[6];

		if (wac_i2c->wac_pdata->xy_switch) {
			tmp = x;
			x = y;
			y = tmp;
		}

		if (wac_i2c->wac_pdata->x_invert)
			x = wac_i2c->wac_feature->x_max - x;
		if (wac_i2c->wac_pdata->y_invert)
			y = wac_i2c->wac_feature->y_max - y;

		if (x < wac_i2c->wac_feature->x_max &&
				y < wac_i2c->wac_feature->y_max) {
			input_report_abs(wac_i2c->input_dev, ABS_X, x);
			input_report_abs(wac_i2c->input_dev, ABS_Y, y);
			input_report_abs(wac_i2c->input_dev,
					ABS_PRESSURE, pressure);
			input_report_key(wac_i2c->input_dev,
					BTN_STYLUS, stylus);
			input_report_key(wac_i2c->input_dev, BTN_TOUCH, prox);
			input_report_key(wac_i2c->input_dev,
					wac_i2c->tool, true);
			input_sync(wac_i2c->input_dev);

			if (wac_i2c->rdy_pdct)
				wacom_i2c_report_pdct(wac_i2c, false);

			wac_i2c->last_x = x;
			wac_i2c->last_y = y;

		#if WACOM_DEBUG_LOG
			if (prox && !wac_i2c->pen_pressed)
				pr_info("wacom: pressed(%d,%d,%d)(%d)\n",
					x, y, pressure, wac_i2c->tool);
			else if (!prox && wac_i2c->pen_pressed)
				pr_info("wacom: released(%d,%d,%d)(%d)\n",
					x, y, pressure, wac_i2c->tool);

			wac_i2c->pen_pressed = prox;

			if (stylus && !wac_i2c->side_pressed)
				pr_info("wacom: side on\n(%d,%d,%d)(%d)\n",
					x, y, pressure, wac_i2c->tool);
			else if (!stylus && wac_i2c->side_pressed)
				pr_info("wacom: side off\n(%d,%d,%d)(%d)\n",
					x, y, pressure, wac_i2c->tool);
		#else
			if (prox && !wac_i2c->pen_pressed)
				pr_info("wacom: pressed\n");
			else if (!prox && wac_i2c->pen_pressed)
				pr_info("wacom: released\n");

			wac_i2c->pen_pressed = prox;

			if (stylus && !wac_i2c->side_pressed)
				pr_info("wacom: side on\n");
			else if (!stylus && wac_i2c->side_pressed)
				pr_info("wacom: side off\n");
		#endif

			wac_i2c->side_pressed = stylus;
		}

	} else {
			wac_i2c->pen_prox = false;
			pr_info("wacom: pdct %d(%d)\n",
					wac_i2c->pen_pdct, wac_i2c->pen_prox);

			if (wac_i2c->pen_pdct)
				wacom_i2c_report_pdct(wac_i2c, true);

			x = ((u16) data[1] << 8) + (u16) data[2];
			y = ((u16) data[3] << 8) + (u16) data[4];

			if (data[0] & 0x40)
				wac_i2c->tool = BTN_TOOL_RUBBER;
			else
				wac_i2c->tool = BTN_TOOL_PEN;

			input_report_abs(wac_i2c->input_dev, ABS_X, x);
			input_report_abs(wac_i2c->input_dev, ABS_Y, y);
			input_report_abs(wac_i2c->input_dev, ABS_PRESSURE, 0);
			input_report_key(wac_i2c->input_dev, BTN_STYLUS, false);
			input_report_key(wac_i2c->input_dev, BTN_TOUCH, false);
			input_report_key(wac_i2c->input_dev,
						wac_i2c->tool, false);
			input_sync(wac_i2c->input_dev);
	}
	return 0;
}

static void wacom_i2c_enable_irq(struct wacom_i2c *wac_i2c, bool enable)
{
	static bool state = true;

	if (enable) {
		if (!state) {
			enable_irq(wac_i2c->irq);
			enable_irq(wac_i2c->irq_pdct);
			state = true;
		}
	} else {
		if (state) {
			disable_irq(wac_i2c->irq);
			disable_irq(wac_i2c->irq_pdct);
			state = false;
		}
	}
}

static void wacom_i2c_reset_hw(struct wacom_platform_data *wac_pdata)
{
	if (wac_pdata->power)
		wac_pdata->power(false);
	msleep(WACOM_OFF_DELAY_MS);
	if (wac_pdata->power)
		wac_pdata->power(true);
	msleep(WACOM_BOOT_DELAY_MS);
}

static void wacom_i2c_enable(struct wacom_i2c *wac_i2c)
{
	bool en = true;

	if (wac_i2c->battery_saving_mode
		&& wac_i2c->pen_insert)
		en = false;

	if (en) {
		if (wac_i2c->wac_pdata->power)
			wac_i2c->wac_pdata->power(true);
		schedule_delayed_work(&wac_i2c->resume_work,
				HZ * WACOM_BOOT_DELAY_MS / 1000);
	}
}

static void wacom_i2c_disable(struct wacom_i2c *wac_i2c)
{
	cancel_delayed_work_sync(&wac_i2c->resume_work);
	if (wac_i2c->power_enable) {
		if (wac_i2c->pen_pdct || wac_i2c->pen_prox)
			wacom_i2c_release_input(wac_i2c);
		wac_i2c->power_enable = false;
	}

	if (wac_i2c->wac_pdata->power)
		wac_i2c->wac_pdata->power(false);
}

static int wacom_i2c_fw_update_file(struct wacom_i2c *wac_i2c)
{
	struct file *fp;
	mm_segment_t old_fs;
	long fsize, nread;
	int ret = 0;
	int retry = MAX_RETRY;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	fp = filp_open(wac_i2c->wac_pdata->file_fw_path, O_RDONLY, S_IRUSR);
	if (IS_ERR(fp)) {
		pr_err("wacom: failed to open %s.\n",
			wac_i2c->wac_pdata->file_fw_path);
		ret = -ENOENT;
		goto open_err;
	} else {
		fsize = fp->f_path.dentry->d_inode->i_size;
		pr_info("wacom: start, file path %s, size %ld Bytes\n",
		       wac_i2c->wac_pdata->file_fw_path, fsize);

		wac_i2c->fw_bin = kmalloc(fsize, GFP_KERNEL);
		if (IS_ERR(wac_i2c->fw_bin)) {
			pr_err("wacom: %s, kmalloc failed\n", __func__);
			goto malloc_error;
		}

		nread = vfs_read(fp, (char __user *)wac_i2c->fw_bin,
					fsize, &fp->f_pos);
		pr_info("wacom: nread %ld\n", nread);
		if (nread != fsize) {
			pr_err(
				"wacom: failed to read firmware file, nread %ld\n",
				nread);
			ret = -EIO;
			kfree(wac_i2c->fw_bin);
			goto read_err;
		}

		while (retry--) {
			ret = wacom_i2c_flash(wac_i2c);
			wacom_i2c_reset_hw(wac_i2c->wac_pdata);
			if (!ret)
				break;
		}

		if (ret < 0) {
			pr_err("wacom: failed to write firmware(%d)\n", ret);
			kfree(wac_i2c->fw_bin);
			goto fw_write_err;
		}

		kfree(wac_i2c->fw_bin);
		wac_i2c->fw_bin = NULL;

		filp_close(fp, current->files);
	}

	set_fs(old_fs);
	return 0;

fw_write_err:
	wac_i2c->fw_bin = NULL;
read_err:
malloc_error:
	filp_close(fp, current->files);
open_err:
	set_fs(old_fs);
	return ret;
}

static int wacom_i2c_fw_update_binary(struct wacom_i2c *wac_i2c)
{
	int ret = 0;
	int retry = MAX_RETRY;
	const struct firmware *firm_data = NULL;

	ret +=
		request_firmware(&firm_data,
			wac_i2c->wac_pdata->binary_fw_path,
			&wac_i2c->client->dev);
	if (ret) {
		pr_err("wacom: unable to open firmware\n");
		goto fw_request_err;
	}

	wac_i2c->fw_bin = (unsigned char *)firm_data->data;

	while (retry--) {
		ret = wacom_i2c_flash(wac_i2c);
		wacom_i2c_reset_hw(wac_i2c->wac_pdata);
		if (!ret)
			break;
	}

	release_firmware(firm_data);
	wac_i2c->fw_bin = NULL;

fw_request_err:
	return ret;
}

static irqreturn_t wacom_interrupt(int irq, void *dev_id)
{
	struct wacom_i2c *wac_i2c = dev_id;
	int ret;
	ret = wacom_i2c_coord(wac_i2c);
	return IRQ_HANDLED;
}

static irqreturn_t wacom_interrupt_pdct(int irq, void *dev_id)
{
	struct wacom_i2c *wac_i2c = dev_id;

	wac_i2c->pen_pdct = !gpio_get_value(wac_i2c->wac_pdata->gpio_pendct);

	pr_info("wacom: pdct %d(%d)\n",
		wac_i2c->pen_pdct, wac_i2c->pen_prox);

	if (wac_i2c->pen_pdct && !wac_i2c->pen_prox)
		wacom_i2c_report_pdct(wac_i2c, true);
	else if (!wac_i2c->pen_pdct && wac_i2c->rdy_pdct)
		wacom_i2c_report_pdct(wac_i2c, false);

	return IRQ_HANDLED;
}

static void handle_pen_detect(struct wacom_i2c *wac_i2c)
{
	wac_i2c->pen_insert =
		!gpio_get_value(wac_i2c->wac_pdata->gpio_pen_insert);
	pr_info("wacom: pen_detect %d\n", wac_i2c->pen_insert);

	input_report_switch(wac_i2c->input_dev,
		SW_PEN_INSERT, !wac_i2c->pen_insert);
	input_sync(wac_i2c->input_dev);

	if (wac_i2c->pen_insert) {
		if (wac_i2c->battery_saving_mode) {
			wacom_i2c_enable_irq(wac_i2c, false);
			wacom_i2c_disable(wac_i2c);
		}
	} else
		wacom_i2c_enable(wac_i2c);
}

static irqreturn_t wacom_pen_detect(int irq, void *dev_id)
{
	struct wacom_i2c *wac_i2c = dev_id;
	handle_pen_detect(wac_i2c);
	return IRQ_HANDLED;
}

static void wacom_i2c_set_input_values(struct i2c_client *client,
				       struct wacom_i2c *wac_i2c,
				       struct input_dev *input_dev)
{
	input_dev->name = "sec_e-pen";
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;
	input_dev->evbit[0] |= BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);

	if (wac_i2c->wac_pdata->gpio_pen_insert >= 0) {
		input_dev->evbit[0] |= BIT_MASK(EV_SW);
		input_set_capability(input_dev, EV_SW, SW_PEN_INSERT);
	}

	__set_bit(ABS_X, input_dev->absbit);
	__set_bit(ABS_Y, input_dev->absbit);
	__set_bit(ABS_PRESSURE, input_dev->absbit);
	__set_bit(BTN_TOUCH, input_dev->keybit);
	__set_bit(BTN_TOOL_PEN, input_dev->keybit);
	__set_bit(BTN_TOOL_RUBBER, input_dev->keybit);
	__set_bit(BTN_STYLUS, input_dev->keybit);
	__set_bit(KEY_UNKNOWN, input_dev->keybit);
	__set_bit(KEY_PEN_PDCT, input_dev->keybit);
}

static int wacom_i2c_remove(struct i2c_client *client)
{
	struct wacom_i2c *wac_i2c = i2c_get_clientdata(client);
	free_irq(client->irq, wac_i2c);
	input_unregister_device(wac_i2c->input_dev);
	kfree(wac_i2c);

	return 0;
}

static void wacom_i2c_shutdown(struct i2c_client *client)
{
	struct wacom_i2c *wac_i2c = i2c_get_clientdata(client);

	wacom_i2c_enable_irq(wac_i2c, false);
	wacom_i2c_disable(wac_i2c);
}

static void wacom_i2c_early_suspend(struct early_suspend *h)
{
	struct wacom_i2c *wac_i2c =
	    container_of(h, struct wacom_i2c, early_suspend);

	wacom_i2c_enable_irq(wac_i2c, false);
	wacom_i2c_disable(wac_i2c);
}

static void wacom_i2c_resume_work(struct work_struct *work)
{
	struct wacom_i2c *wac_i2c =
	    container_of(work, struct wacom_i2c, resume_work.work);

	wacom_i2c_enable_irq(wac_i2c, true);
	wac_i2c->power_enable = true;
}

static void wacom_i2c_late_resume(struct early_suspend *h)
{
	struct wacom_i2c *wac_i2c =
	    container_of(h, struct wacom_i2c, early_suspend);

	wacom_i2c_enable(wac_i2c);
}

#ifdef CONFIG_HAS_EARLYSUSPEND
#define wacom_i2c_suspend	NULL
#define wacom_i2c_resume	NULL
#else
static int wacom_i2c_suspend(struct i2c_client *client, pm_message_t mesg)
{
	wacom_i2c_early_suspend();
	return 0;
}

static int wacom_i2c_resume(struct i2c_client *client)
{
	wacom_i2c_late_resume();
	return 0;
}
#endif

static ssize_t epen_firm_update_status_show(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);

	pr_debug("wacom: %s:(%d)\n", __func__,
		wac_i2c->wac_feature->firm_update_status);

	if (wac_i2c->wac_feature->firm_update_status == 2)
		return snprintf(buf, sizeof(buf)+2, "PASS\n");
	else if (wac_i2c->wac_feature->firm_update_status == 1)
		return snprintf(buf, sizeof(buf)+2, "DOWNLOADING\n");
	else if (wac_i2c->wac_feature->firm_update_status == -1)
		return snprintf(buf, sizeof(buf)+2, "FAIL\n");
	else
		return 0;
}

static ssize_t epen_firm_version_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);
	pr_debug("wacom: %s: 0x%x|0x%X\n", __func__,
		wac_i2c->wac_feature->fw_version,
		wac_i2c->wac_pdata->fw_version);

	return sprintf(buf, "%04X\t%04X\n",
		wac_i2c->wac_feature->fw_version,
		wac_i2c->wac_pdata->fw_version);
}

static ssize_t epen_firmware_update_store(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);
	int ret = 0;

	mutex_lock(&wac_i2c->lock);

	wacom_i2c_enable_irq(wac_i2c, false);

	wac_i2c->wac_feature->firm_update_status = 1;

	pr_debug("wacom: %s\n", __func__);

	switch (*buf) {
	case 'I':
		pr_info("wacom: start F/W flashing (Internal storage).\n");
		pr_info("wacom: force boot mode\n");
		gpio_set_value(wac_i2c->wac_pdata->gpio_pen_fwe1, true);
		printk(KERN_ERR
			"[wacom] @after read fw version, pen_fwe1 = %d\n",
			gpio_get_value(wac_i2c->wac_pdata->gpio_pen_fwe1));
		wacom_i2c_reset_hw(wac_i2c->wac_pdata);
	
		ret = wacom_i2c_fw_update_file(wac_i2c);
		break;
	case 'K':
		pr_info("wacom: start F/W flashing (Kernel binary).\n");
		pr_info("wacom: force boot mode\n");
		gpio_set_value(wac_i2c->wac_pdata->gpio_pen_fwe1, true);
		printk(KERN_ERR
			"[wacom] @after read fw version, pen_fwe1 = %d\n",
			gpio_get_value(wac_i2c->wac_pdata->gpio_pen_fwe1));
		wacom_i2c_reset_hw(wac_i2c->wac_pdata);
	
		ret = wacom_i2c_fw_update_binary(wac_i2c);
		break;

	case 'R':
		if (wac_i2c->wac_feature->fw_version <
			wac_i2c->wac_pdata->fw_version)
			ret = wacom_i2c_fw_update_binary(wac_i2c);
		break;
	default:
		pr_err("wacom: '%c' wrong parameter.\n", *buf);
		goto param_err;
		break;
	}

	if (ret < 0) {
		pr_err("wacom: failed to flash F/W.\n");
		goto failure;
	}

	pr_info("wacom: finish F/W flashing.\n");

	wacom_i2c_query(wac_i2c);

	wac_i2c->wac_feature->firm_update_status = 2;
	wacom_i2c_enable_irq(wac_i2c, true);
	mutex_unlock(&wac_i2c->lock);

	return count;

 param_err:

 failure:
	wac_i2c->wac_feature->firm_update_status = -1;
	wacom_i2c_enable_irq(wac_i2c, true);
	mutex_unlock(&wac_i2c->lock);
	return -1;
}

static ssize_t epen_sampling_rate_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);
	int value;
	char mode;

	if (sscanf(buf, "%d", &value) == 1) {
		switch (value) {
		case 0:
			mode = COM_SAMPLERATE_STOP;
			break;
		case 40:
			mode = COM_SAMPLERATE_40;
			break;
		case 80:
			mode = COM_SAMPLERATE_80;
			break;
		case 133:
			mode = COM_SAMPLERATE_133;
			break;
		default:
			pr_err("wacom: Invalid sampling rate value\n");
			count = -1;
			goto fail;
		}

		wacom_i2c_enable_irq(wac_i2c, false);
		if (1 == wacom_i2c_send(wac_i2c, &mode, 1, true)) {
			pr_info("wacom: sampling rate %d\n", value);
			msleep(WACOM_QUERY_DELAY_MS);
		} else {
			pr_err("wacom: I2C write error\n");
			count = -1;
		}
		wacom_i2c_enable_irq(wac_i2c, true);

	} else {
		pr_err("wacom: can't get sampling rate data\n");
		count = -1;
	}
 fail:
	return count;
}

static ssize_t epen_reset_store(struct device *dev,
				struct device_attribute *attr, const char *buf,
				size_t count)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);
	int ret;
	int val;

	sscanf(buf, "%d", &val);

	if (val == 1) {
		wacom_i2c_enable_irq(wac_i2c, false);

		wacom_i2c_reset_hw(wac_i2c->wac_pdata);

		ret = wacom_i2c_query(wac_i2c);

		wacom_i2c_enable_irq(wac_i2c, true);

		if (ret < 0)
			wac_i2c->epen_reset_result = false;
		else
			wac_i2c->epen_reset_result = true;

		pr_info("wacom: %s, result %d\n", __func__,
		       wac_i2c->epen_reset_result);
	}

	return count;
}

static ssize_t epen_reset_result_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);

	if (wac_i2c->epen_reset_result) {
		pr_info("wacom: %s, PASS\n", __func__);
		return snprintf(buf, sizeof(buf), "PASS\n");
	} else {
		pr_info("wacom: %s, FAIL\n", __func__);
		return snprintf(buf, sizeof(buf), "FAIL\n");
	}
}

static ssize_t epen_checksum_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);
	int val;

	sscanf(buf, "%d", &val);

	if (val == 1) {
		wacom_i2c_enable_irq(wac_i2c, false);

		wacom_checksum(wac_i2c);

		wacom_i2c_enable_irq(wac_i2c, true);

		pr_info("wacom: %s, result %d\n", __func__,
		       wac_i2c->checksum_result);
	}

	return count;
}

static ssize_t epen_checksum_result_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);

	if (wac_i2c->checksum_result) {
		pr_info("wacom: checksum, PASS\n");
		return snprintf(buf, sizeof(buf), "PASS\n");
	} else {
		pr_info("wacom: checksum, FAIL\n");
		return snprintf(buf, sizeof(buf), "FAIL\n");
	}
}

static ssize_t epen_connection_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);

	pr_info("wacom: connection_check : %d\n",
		wac_i2c->connection_check);
	return snprintf(buf, sizeof(buf), "%s\n",
		wac_i2c->connection_check ?
		"OK" : "NG");
}

static ssize_t epen_saving_mode_store(struct device *dev,
				struct device_attribute *attr, const char *buf,
				size_t count)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);
	int val;

	if (sscanf(buf, "%u", &val) == 1)
		wac_i2c->battery_saving_mode = !!val;

	if (wac_i2c->battery_saving_mode) {
		if (wac_i2c->pen_insert) {
			wacom_i2c_enable_irq(wac_i2c, false);
			wacom_i2c_disable(wac_i2c);
		}
	} else
		wacom_i2c_enable(wac_i2c);
	return count;
}

static ssize_t epen_reset_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	struct wacom_i2c *wac_i2c = dev_get_drvdata(dev);

	wac_i2c->power(0);
	msleep(30);
	wac_i2c->power(1);
	msleep(50);

	pr_info("wacom: reset\n");

	return snprintf(buf, sizeof(buf), "NG");
}




static DEVICE_ATTR(epen_firm_update,
		   S_IWUSR | S_IWGRP, NULL, epen_firmware_update_store);
static DEVICE_ATTR(epen_firm_update_status,
		   S_IRUGO, epen_firm_update_status_show, NULL);
static DEVICE_ATTR(epen_firm_version, S_IRUGO, epen_firm_version_show, NULL);

static DEVICE_ATTR(epen_sampling_rate,
		   S_IWUSR | S_IWGRP, NULL, epen_sampling_rate_store);

static DEVICE_ATTR(epen_reset, S_IWUSR | S_IWGRP, NULL, epen_reset_store);
static DEVICE_ATTR(epen_reset_result, S_IRUGO, epen_reset_result_show, NULL);

static DEVICE_ATTR(epen_checksum, S_IWUSR | S_IWGRP, NULL, epen_checksum_store);
static DEVICE_ATTR(epen_checksum_result, S_IRUGO,
		   epen_checksum_result_show, NULL);

static DEVICE_ATTR(epen_connection,
		   S_IRUGO, epen_connection_show, NULL);
static DEVICE_ATTR(epen_saving_mode,
		   S_IWUSR | S_IWGRP, NULL, epen_saving_mode_store);

static DEVICE_ATTR(reset,
		   S_IRUGO, epen_reset_show, NULL);

static struct attribute *epen_attributes[] = {
	&dev_attr_epen_firm_update.attr,
	&dev_attr_epen_firm_update_status.attr,
	&dev_attr_epen_firm_version.attr,
	&dev_attr_epen_sampling_rate.attr,
	&dev_attr_epen_reset.attr,
	&dev_attr_epen_reset_result.attr,
	&dev_attr_epen_checksum.attr,
	&dev_attr_epen_checksum_result.attr,
	&dev_attr_epen_connection.attr,
	&dev_attr_epen_saving_mode.attr,
	&dev_attr_reset.attr,
	NULL,
};

static struct attribute_group epen_attr_group = {
	.attrs = epen_attributes,
};

static int wacom_i2c_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct wacom_platform_data *pdata = client->dev.platform_data;
	struct wacom_i2c *wac_i2c;
	struct input_dev *input;
	int ret = 0;

	printk(KERN_ERR "%s: wacom probe start!!!!\n", __func__);

	if (pdata == NULL) {
		pr_err("%s: no pdata\n", __func__);
		ret = -ENODEV;
		goto err_i2c_fail;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("wacom: no i2c functionality found\n");
		ret = -ENODEV;
		goto err_i2c_fail;
	}

	wac_i2c = kzalloc(sizeof(struct wacom_i2c), GFP_KERNEL);
	if (!wac_i2c) {
		pr_err("wacom: failed to allocate wac_i2c.\n");
		ret = -ENOMEM;
		goto err_alloc_mem;
	}

	wac_i2c->wac_feature =
		kzalloc(sizeof(struct wacom_features), GFP_KERNEL);
	if (!wac_i2c->wac_feature) {
		pr_err("wacom: failed to allocate wac_feature.\n");
		ret = -ENOMEM;
		goto err_alloc_mem2;
	}

	wac_i2c->wac_pdata = pdata;
	wac_i2c->client = client;
	wac_i2c->irq = client->irq;
	wac_i2c->irq_pdct = gpio_to_irq(pdata->gpio_pendct);

	wac_i2c->client_boot = i2c_new_dummy(client->adapter,
		pdata->boot_addr);
	if (!wac_i2c->client_boot) {
		pr_err("wacom: fail to register sub client[0x%x]\n",
			 pdata->boot_addr);
		goto err_alloc_i2c_client;
	}

	input = input_allocate_device();
	if (!input) {
		pr_err("wacom: failed to allocate input device.\n");
		ret = -ENOMEM;
		goto err_input_allocate_device;
	} else
		wacom_i2c_set_input_values(client, wac_i2c, input);
	wac_i2c->input_dev = input;

	if (pdata->power)
		pdata->power(true);
	if (!pdata->boot_on)
		msleep(WACOM_BOOT_DELAY_MS);

	msleep(WACOM_BOOT_DELAY_MS);

	wac_i2c->power_enable = true;

	printk(KERN_ERR "[wacom] @probe start, gpio_value : pen_fwe1 = %d\n",
			gpio_get_value(pdata->gpio_pen_fwe1));

	ret = wacom_i2c_query(wac_i2c);
	if (ret < 0)
		wac_i2c->epen_reset_result = false;
	else
		wac_i2c->epen_reset_result = true;

	pr_info("wacom: IC fw ver : 0x%x, bin fw ver : 0x%x\n",
		wac_i2c->wac_feature->fw_version,
		pdata->fw_version);

	if (wac_i2c->wac_feature->fw_version < pdata->fw_version) {
		pr_info("wacom: force boot mode\n");
		gpio_set_value(pdata->gpio_pen_fwe1, true);
		printk(KERN_ERR
			"[wacom] @after read fw version, pen_fwe1 = %d\n",
			gpio_get_value(pdata->gpio_pen_fwe1));
		wacom_i2c_reset_hw(pdata);

		ret = wacom_i2c_fw_update_binary(wac_i2c);

		if (ret < 0) {
			printk(KERN_ERR "%s: [wacom] firmware update failed!!\n",
				__func__);
			goto err_fw_update;
		}

		if (pdata->gpio_pen_fwe1 >= 0 &&
				gpio_get_value(pdata->gpio_pen_fwe1)) {
			gpio_set_value(pdata->gpio_pen_fwe1, false);
			printk(KERN_ERR
				"[wacom] @after update firmware, pen_fwe1 = %d\n",
				gpio_get_value(pdata->gpio_pen_fwe1));

			wacom_i2c_reset_hw(pdata);
		}

		ret = wacom_i2c_query(wac_i2c);
		if (ret < 0)
			wac_i2c->epen_reset_result = false;
		else
			wac_i2c->epen_reset_result = true;
	}

	input_set_abs_params(wac_i2c->input_dev, ABS_X,
		0, wac_i2c->wac_feature->x_max, 4, 0);
	input_set_abs_params(wac_i2c->input_dev, ABS_Y,
		0, wac_i2c->wac_feature->y_max, 4, 0);
	input_set_abs_params(wac_i2c->input_dev, ABS_PRESSURE,
		0, wac_i2c->wac_feature->pressure_max, 0, 0);
	input_set_drvdata(input, wac_i2c);

	ret = input_register_device(input);
	if (ret) {
		pr_err("wacom: failed to register input device.\n");
		goto err_register_device;
	}

	i2c_set_clientdata(client, wac_i2c);
	i2c_set_clientdata(wac_i2c->client_boot, wac_i2c);

	mutex_init(&wac_i2c->lock);
	INIT_DELAYED_WORK(&wac_i2c->resume_work, wacom_i2c_resume_work);

#ifdef CONFIG_HAS_EARLYSUSPEND
	wac_i2c->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	wac_i2c->early_suspend.suspend = wacom_i2c_early_suspend;
	wac_i2c->early_suspend.resume = wacom_i2c_late_resume;
	register_early_suspend(&wac_i2c->early_suspend);
#endif

	wac_i2c->dev = device_create(sec_class, NULL, 0, NULL, "sec_epen");
	if (IS_ERR(wac_i2c->dev)) {
		pr_err("Failed to create device(wac_i2c->dev)!\n");
		goto err_sysfs_create_group;
	} else {
		dev_set_drvdata(wac_i2c->dev, wac_i2c);
		ret = sysfs_create_group(&wac_i2c->dev->kobj, &epen_attr_group);
		if (ret) {
			pr_err("wacom: failed to create sysfs group\n");
			goto err_sysfs_create_group;
		}
	}

	ret =
	    request_threaded_irq(wac_i2c->irq, NULL, wacom_interrupt,
				 IRQF_DISABLED | IRQF_TRIGGER_RISING |
				 IRQF_ONESHOT, wac_i2c->name, wac_i2c);
	if (ret < 0) {
		pr_err("wacom: failed to request irq(%d) - %d\n",
		       wac_i2c->irq, ret);
		goto err_request_irq;
	}

	ret =
		request_threaded_irq(wac_i2c->irq_pdct, NULL,
				wacom_interrupt_pdct,
				IRQF_DISABLED | IRQF_TRIGGER_RISING |
				IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
				wac_i2c->name, wac_i2c);
	if (ret < 0) {
		pr_err("wacom: failed to request irq(%d) - %d\n",
			wac_i2c->irq_pdct, ret);
		goto err_request_irq;
	}

	if (pdata->gpio_pen_insert >= 0) {
		wac_i2c->irq_pen_insert = gpio_to_irq(pdata->gpio_pen_insert);
		ret =
			request_threaded_irq(
				wac_i2c->irq_pen_insert, NULL,
				wacom_pen_detect,
				IRQF_DISABLED | IRQF_TRIGGER_RISING |
				IRQF_TRIGGER_FALLING | IRQF_ONESHOT |
				IRQF_NO_SUSPEND, wac_i2c->name, wac_i2c);
		if (ret < 0) {
			pr_err("wacom: failed to request irq(%d) - %d\n",
			wac_i2c->irq_pen_insert, ret);
			goto err_request_irq;
		}
		handle_pen_detect(wac_i2c);
	}

	return 0;

 err_request_irq:
 err_sysfs_create_group:
	input_unregister_device(input);
 err_register_device:
	input_free_device(input);
 err_fw_update:
 err_input_allocate_device:
	i2c_unregister_device(wac_i2c->client_boot);
 err_alloc_i2c_client:
	kfree(wac_i2c->wac_feature);
 err_alloc_mem2:
	kfree(wac_i2c);
 err_alloc_mem:
 err_i2c_fail:
	return ret;
}

static const struct i2c_device_id wacom_i2c_id[] = {
	{WACNAME, 0},
	{},
};

static struct i2c_driver wacom_i2c_driver = {
	.driver = {
		   .name = WACNAME,
		   },
	.probe = wacom_i2c_probe,
	.remove = wacom_i2c_remove,
	.suspend = wacom_i2c_suspend,
	.resume = wacom_i2c_resume,
	.shutdown = wacom_i2c_shutdown,
	.id_table = wacom_i2c_id,
};

static int __init wacom_i2c_init(void)
{
	int ret;

	ret = i2c_add_driver(&wacom_i2c_driver);
	if (ret)
		pr_err("wacom: fail to i2c_add_driver\n");
	return ret;
}

static void __exit wacom_i2c_exit(void)
{
	i2c_del_driver(&wacom_i2c_driver);
}

late_initcall(wacom_i2c_init);
module_exit(wacom_i2c_exit);

MODULE_AUTHOR("Samsung");
MODULE_DESCRIPTION("Driver for Wacom Digitizer Controller Driver");

MODULE_LICENSE("GPL");
