/*===========================================================================
*
*                        SiI9024A I2C MASTER.C
*
*
* DESCRIPTION
* This file explains the SiI9024A initialization and call the virtual
* main function.
*
* Copyright (C) (2011, Silicon Image Inc)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*****************************************************************************/

/*===========================================================================
EDIT HISTORY FOR FILE

when              who                         what, where, why
---------------------------------------------------------------------
2010/10/25   Daniel Lee(Philju)   Initial version of file, SIMG Korea
2011/04/06   Rajkumar c m   Added support for qualcomm msm8060
===========================================================================*/

#include <linux/delay.h>
#include <linux/fcntl.h>
#include <linux/freezer.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include "Common_Def.h"
#include "SiI9234_I2C_slave_add.h"

/*===========================================================================

===========================================================================*/

#define LAST_BYTE      1
#define NOT_LAST_BYTE  0


#define TPI_INDEXED_PAGE_REG		0xBC
#define TPI_INDEXED_OFFSET_REG		0xBD
#define TPI_INDEXED_VALUE_REG		0xBE

#define DRIVER_NAME "MHL_sii9234"
#define print_info(f, arg...) printk(KERN_INFO DRIVER_NAME ": " f "\n", ## arg)

/*===========================================================================

===========================================================================*/
void i2c_write_byte(byte deviceID, byte offset, byte value)
{
	int ret = 0;
	struct i2c_client *client_ptr = get_sii9234_client(deviceID);

	if (!client_ptr)
		print_info("[MHL]I2C_WriteByte error %x\n", deviceID);

	if (deviceID == 0x72 || deviceID == 0x7A || deviceID == 0x92
		|| deviceID == 0xC8)
		ret = sii9234_i2c_write(client_ptr, offset, value);

	if (ret < 0)
		print_info("I2C_WriteByte:Device ID=0x%X, Err ret=%d\n",
							deviceID, ret);
}


byte i2c_read_byte(byte deviceID, byte offset)
{
	byte number = 0;
	struct i2c_client *client_ptr = get_sii9234_client(deviceID);

	if (!client_ptr) {
		print_info("[MHL]I2C_ReadByte error %x\n", deviceID);
		return 0;
	}

	if (deviceID == 0x72 || deviceID == 0x7A || deviceID == 0x92
		|| deviceID == 0xC8)
		number = sii9234_i2c_read(client_ptr, offset);

	if (number < 0)
		print_info("I2C_ReadByte:Device ID=0x%X, Err ret=%d\n",
							deviceID, number);

	return number;
}

byte read_byte_tpi(byte Offset)
{
	return i2c_read_byte(SA_TX_Page0_Primary, Offset);
}

void write_byte_tpi(byte Offset, byte Data)
{
	i2c_write_byte(SA_TX_Page0_Primary, Offset, Data);
}

void read_modify_write_tpi(byte Offset, byte Mask, byte Data)
{

	byte Temp;
	Temp = read_byte_tpi(Offset);
	Temp &= ~Mask;
	Temp |= (Data & Mask);
	write_byte_tpi(Offset, Temp);
}

byte read_byte_cbus(byte Offset)
{
	return i2c_read_byte(SA_TX_CBUS_Primary, Offset);
}

void write_byte_cbus(byte Offset, byte Data)
{
	i2c_write_byte(SA_TX_CBUS_Primary, Offset, Data);
}

void read_modify_write_cbus(byte Offset, byte Mask, byte Value)
{
	byte Temp;
	Temp = read_byte_cbus(Offset);
	Temp &= ~Mask;
	Temp |= (Value & Mask);
	write_byte_cbus(Offset, Temp);
}


/*
* FUNCTION:ReadIndexedRegister()
* PURPOSE:Read the value from an indexed register.
*	Write:
*	1. 0xBC => Indexed page num
*	2. 0xBD => Indexed register offset
*	Read:
*	3. 0xBE => Returns the indexed register value
* INPUT PARAMS:PageNum-indexed page number
*	Offset-offset of the register within the indexed page.
* OUTPUT PARAMS:None
* GLOBALS USED:None
* RETURNS:The value read from the indexed register.
*/

byte read_indexed_register(byte PageNum, byte Offset)
{
	write_byte_tpi(TPI_INDEXED_PAGE_REG, PageNum);
	write_byte_tpi(TPI_INDEXED_OFFSET_REG, Offset);
	return read_byte_tpi(TPI_INDEXED_VALUE_REG);
}


/*
* FUNCTION:WriteIndexedRegister()
* PURPOSE:Write a value to an indexed register
* Write:
*	1. 0xBC => Indexed page num
*	2. 0xBD => Indexed register offset
*	3. 0xBE => Set the indexed register value
* INPUT PARAMS:PageNum-indexed page number
*	Offset-offset of the register within the indexed page.
*	Data-the value to be written.
* OUTPUT PARAMS:None
* GLOBALS USED:None
* RETURNS:None
*/

void write_indexed_register(byte PageNum, byte Offset, byte Data)
{
	write_byte_tpi(TPI_INDEXED_PAGE_REG, PageNum);
	write_byte_tpi(TPI_INDEXED_OFFSET_REG, Offset);
	write_byte_tpi(TPI_INDEXED_VALUE_REG, Data);
}


/*
* FUNCTION:ReadModifyWriteIndexedRegister()
* PURPOSE:Set or clear individual bits in a TPI register.
* INPUT PARAMS:PageNum-indexed page number
*	Offset-the offset of the indexed register to be modified.
*	Mask-"1" for each indexed register bit that needs to be modified
*	Data-The desired value for the register bits in their proper positions
* OUTPUT PARAMS:None
* GLOBALS USED:None
* RETURNS:void
*/

void read_modify_write_indexed_register(byte PageNum, byte Offset, byte Mask,
byte Data)
{
	byte Temp;
	Temp = read_indexed_register(PageNum, Offset);
	Temp &= ~Mask;
	Temp |= (Data & Mask);
	write_byte_tpi(TPI_INDEXED_VALUE_REG, Temp);
}

