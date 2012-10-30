/*===========================================================================
*
*                        SiI9024A I2C MASTER.C
*
*
*DESCRIPTION
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
--------        ---              -------------------------------------
2010/10/25  Daniel Lee(Philju)  Initial version of file, SIMG Korea
===========================================================================*/
#include "Common_Def.h"

#include <linux/types.h>

/*===========================================================================

===========================================================================*/


void i2c_write_byte(byte deviceID, byte offset, byte value);
byte i2c_read_byte(byte deviceID, byte offset);

byte read_byte_tpi(byte Offset);
void write_byte_tpi(byte Offset, byte Data);
void write_indexed_register(byte PageNum, byte Offset, byte Data);
void read_modify_write_indexed_register(byte PageNum, byte Offset,
byte mask, byte Data);
void read_modify_write_tpi(byte Offset, byte Mask, byte Data);
void write_byte_cbus(byte Offset, byte Data);
void read_modify_write_cbus(byte Offset, byte Mask, byte Value);
byte read_indexed_register(byte PageNum, byte Offset);
byte read_byte_cbus(byte Offset);
