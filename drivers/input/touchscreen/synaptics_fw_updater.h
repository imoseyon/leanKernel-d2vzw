/* drivers/input/touchscreen/synaptics_fw_updater.h
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

#ifndef _LINUX_SYNAPTICS_FW_UPDATER_H
#define _LINUX_SYNAPTICS_FW_UPDATER_H

#include <linux/i2c.h>

void set_fw_version(char *, char *);
bool fw_update_file(struct i2c_client *);
bool fw_update_internal(struct i2c_client *);
bool F54_SetRawCapData(struct i2c_client *, s16 *);
bool F54_SetRxToRxData(struct i2c_client *, s16 *);
bool F54_TxToTest(struct i2c_client *, s16 *, int);
void F01_SetTABit(struct i2c_client *, int);

#endif
