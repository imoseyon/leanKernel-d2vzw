/******************************************************************************
*
*                        SiI9234_DRIVER.H
*
* DESCRIPTION
* This file explains the SiI9234 initialization and call the virtual
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
--------        ---                        --------------------------------
2010/11/06    Daniel Lee(Philju)      Initial version of file, SIMG Korea
==========================================================================*/

void sii9234_interrupt_event(void);
bool sii9234_initialize(struct kobject *rcp_kobj);

void    write_initialize_register_values(void);
void    mhltx_drvprocess_connection(void);
void    mhltx_drvprocess_disconnection(void);
void    cbus_reset(void);
void    switch_to_D0(void);
void    switch_to_D3(void);
void    sii_mhltx_notify_connection(bool mhlConnected);
void    app_vbus_control(bool powerOn);
void    app_rcp_demo(byte event, byte eventParameter);
void    sii_mhltx_notify_dshpd_change(byte dsHpdStatus);
bool    sii_mhltx_read_devcap(byte offset);
bool    sii_mhltx_rcpk_send(byte);
void    sii_mhltx_got_mhlstatus(byte status_0, byte status_1);
void    sii_mhltx_got_mhlintr(byte intr_0, byte intr_1);
void    sii_mhltx_got_mhlmscmsg(byte subCommand, byte cmdData);
void    sii_mhltx_msc_command_done(byte data1);
void    sii_mhltx_get_events(byte *event, byte *eventParameter);
void    sii_mhltx_initialize(void);
bool    sii_mhltx_rcpe_send(byte rcpeErrorCode);


void    delay_ms(word msec);
void    sii9234_hw_reset(void);
bool    sii9234_start_tpi(void);
bool    sii9234_initialize(struct kobject *rcp_kobj);
void    sii9234_interrupt_event(void);
void    process_rgnd(void);
extern void mhl_hpd_handler(bool);
/*
#ifdef CONFIG_USB_SWITCH_FSA9485
extern void FSA9480_MhlSwitchSel(bool);
extern void FSA9480_MhlTvOff(void);
extern void EnableFSA9480Interrupts(void);
extern void DisableFSA9480Interrupts(void);
#endif
*/
extern bool sii9234_start_tpi(void);
#define MHL_READ_RCP_DATA 0x1

/****************************************************************************
byte    rcpSupportTable[MHL_MAX_RCP_KEY_CODE] = {
	/ 0x00 = Select8i/
	/ 0x01 = Up/
	/ 0x02 = Down/
	/ 0x03 = Left/
	/ 0x04 = Right/
	/ 05-08 Reserved/
	/ 0x09 = Root Menu/
	/ 0A-0C Reserved/
	/ 0x0D = Select/
	/ 0E-1F Reserved/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
	/0x2A = Dot/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA | MHL_DEV_LD_TUNER),/
	/ 2D-2F Reserved/
	/(MHL_DEV_LD_TUNER),             // 0x30 = Channel Up
	/(MHL_DEV_LD_TUNER),             // 0x31 = Channel Dn
	/(MHL_DEV_LD_TUNER),             // 0x32 = Previous Channel
	/(MHL_DEV_LD_AUDIO),             // 0x33 = Sound Select
	/ 0x34 = Input Select/
	/ 0x35 = Show Information/
	/ 0x36 = Help/
	/ 0x37 = Page Up/
	/ 0x38 = Page Down/
	/ 0x39-0x3F Reserved/
	/ 0x40 = Undefined/

	/(MHL_DEV_LD_SPEAKER),   // 0x41 = Volume Up
	/(MHL_DEV_LD_SPEAKER),   // 0x42 = Volume Down
	/(MHL_DEV_LD_SPEAKER),   // 0x43 = Mute
	// 0x44 = Play
	//0x45 = Stop
	//0x46 = Pause
	/    (MHL_DEV_LD_RECORD),    // 0x47 = Record
	// 0x48 = Rewind
	// 0x49 = Fast Forward
	/    (MHL_DEV_LD_MEDIA),             // 0x4A = Eject
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),// 0x4B = Forward
/(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),//0x4C = Backward
	/ 4D-4F Reserved/
	/ 0x50 = Angle/
	/ 0x51 = Subpicture/
	/ 52-5F Reserved/
	// 0x60 = Play Function
	// 0x61 = Pause the Play Function
/(MHL_DEV_LD_RECORD),    // 0x62 = Record Function
/(MHL_DEV_LD_RECORD),    // 0x63 = Pause the Record Function
	//0x64 = Stop Function

/(MHL_DEV_LD_SPEAKER),   // 0x65 = Mute Function
/(MHL_DEV_LD_SPEAKER),   // 0x66 = Restore Mute Function
	/ Undefined or reserved/
	/ Undefined or reserved/
};
******************************************************************************/
