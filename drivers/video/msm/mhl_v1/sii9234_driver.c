/*************************************************************************
*
*                        sii9234 Driver Processor
*
* DESCRIPTION
*  This file explains the sii9234 initialization and call
						 the virtual main function.
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
--------        ------                ----------------------------------
2010/10/25    Daniel Lee(Philju)      Initial version of file, SIMG Korea
2011/04/06    Rajkumar c m            added support for qualcomm msm8060
2011/08/29    min oh		      added support for qualcomm msm8960
===========================================================================*/


/*===========================================================================
		INCLUDE FILES FOR MODULE
===========================================================================*/

#include <linux/delay.h>
#include <linux/fcntl.h>
#include <linux/freezer.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include "Common_Def.h"
#include "SiI9234_Reg.h"
#include "SiI9234_I2C_master.h"
#include "SiI9234_I2C_slave_add.h"
#include "si_cbusDefs.h"
#include "si_cbus_regs.h"
#include "si_cbus.h"
#include "si_apiCbus.h"
#include "sii9234_driver.h"

/*===========================================================================
Definition
===========================================================================*/
#define TX_HW_RESET_PERIOD      200


#define SiI_DEVICE_ID           0xB0
#define SiI_DEVICE_ID_9234		0xB4


#define DDC_XLTN_TIMEOUT_MAX_VAL		0x30


#define INDEXED_PAGE_0		0x01
#define INDEXED_PAGE_1		0x02
#define INDEXED_PAGE_2		0x03

#define ASR_VALUE 0x04

#define	TX_POWER_STATE_D0_NO_MHL		TX_POWER_STATE_D2
#define	TX_POWER_STATE_D0_MHL			TX_POWER_STATE_D0
#define	TX_POWER_STATE_FIRST_INIT		0xFF
#define MHL_INIT_POWER_OFF        0x00
#define MHL_POWER_ON              0x01
#define MHL_1K_IMPEDANCE_VERIFIED 0x02
#define MHL_RSEN_VERIFIED         0x04
#define MHL_TV_OFF_CABLE_CONNECT 0x08

#define TX_DEBUG_PRINT(x) printk x

#define	I2C_READ_MODIFY_WRITE(saddr, offset, mask)			\
		i2c_write_byte(saddr, offset, i2c_read_byte(saddr, offset)\
								 | (mask));

#define	SET_BIT(saddr, offset, bitnumber)				\
		I2C_READ_MODIFY_WRITE(saddr, offset, (1<<bitnumber))
#define	CLR_BIT(saddr, offset, bitnumber)		i2c_write_byte(saddr, \
		offset, i2c_read_byte(saddr, offset) & ~(1<<bitnumber))
/*
90[0] = Enable / Disable MHL Discovery on MHL link
*/
#define	DISABLE_DISCOVERY	CLR_BIT(SA_TX_Page0_Primary, 0x90, 0);
#define	ENABLE_DISCOVERY	SET_BIT(SA_TX_Page0_Primary, 0x90, 0);
/*
	Look for interrupts on INTR_4 (Register 0x74)
		7 = PVT_HTBT(reserved)
		6 = RGND RDY		(interested)
		5 = VBUS low(interested)
		4 = CBUS LKOUT		(reserved)
		3 = USB EST		(reserved)
		2 = MHL EST		(reserved)
		1 = RPWR5V CHANGE		(reserved)
		0 = SCDT CHANGE		(reserved)*/
#define	INTR_4_DESIRED_MASK		(BIT_2 | BIT_3 | BIT_4 | BIT_6)
#define	UNMASK_INTR_4_INTERRUPTS	i2c_write_byte(SA_TX_Page0_Primary,\
					 0x78, 0x00)
#define	MASK_INTR_4_INTERRUPTS	i2c_write_byte(SA_TX_Page0_Primary,	\
					 0x78, INTR_4_DESIRED_MASK)
/*	Look for interrupts on INTR_1 (Register 0x71)
		7 = RSVD		(reserved)
		6 = MDI_HPD		(interested)
		5 = RSEN CHANGED(interested)
		4 = RSVD		(reserved)
		3 = RSVD		(reserved)
		2 = RSVD		(reserved)
		1 = RSVD		(reserved)
		0 = RSVD		(reserved)
*/
#define	INTR_1_DESIRED_MASK				(BIT_5|BIT_6)
#define	UNMASK_INTR_1_INTERRUPTS					\
				i2c_write_byte(SA_TX_Page0_Primary, 0x75, 0x00)
#define	MASK_INTR_1_INTERRUPTS	i2c_write_byte(SA_TX_Page0_Primary, 0x75, \
							INTR_1_DESIRED_MASK)
/*	Look for interrupts on CBUS:CBUS_INTR_STATUS [0xC8:0x08]
		7 = RSVD			(reserved)
		6 = MSC_RESP_ABORT	(interested)
		5 = MSC_REQ_ABORT	(interested)
		4 = MSC_REQ_DONE	(interested)
		3 = MSC_MSG_RCVD	(interested)
		2 = DDC_ABORT		(interested)
		1 = RSVD			(reserved)
		0 = rsvd			(reserved)
*/
#define	INTR_CBUS1_DESIRED_MASK		(BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6)
#define	UNMASK_CBUS1_INTERRUPTS		i2c_write_byte(SA_TX_CBUS_Primary,\
								 0x09, 0x00)
#define	MASK_CBUS1_INTERRUPTS		i2c_write_byte(SA_TX_CBUS_Primary,\
					 0x09, INTR_CBUS1_DESIRED_MASK)

#define	INTR_CBUS2_DESIRED_MASK			(BIT_2 | BIT_3)
#define	UNMASK_CBUS2_INTERRUPTS		i2c_write_byte(SA_TX_CBUS_Primary, \
								0x1F, 0x00)
#define	MASK_CBUS2_INTERRUPTS		 i2c_write_byte(SA_TX_CBUS_Primary,\
						 0x1F, INTR_CBUS2_DESIRED_MASK)

#define		MHL_TX_EVENT_NONE			0x00/* No event worth
								 reporting.*/
#define		MHL_TX_EVENT_DISCONNECTION	0x01/* MHL connection has
								 been lost */
#define		MHL_TX_EVENT_CONNECTION		0x02/* MHL connection has
							 been established */
#define		MHL_TX_EVENT_RCP_READY		0x03/* MHL connection is
							 ready for RCP */
#define		MHL_TX_EVENT_RCP_RECEIVED	0x04/* Received an RCP.
						 Key Code in "eventParameter" */
#define		MHL_TX_EVENT_RCPK_RECEIVED	0x05/* Received an RCPK
								 message */
#define		MHL_TX_EVENT_RCPE_RECEIVED	0x06/* Received an RCPE
								 message .*/

/* To use hrtimer*/
#define	MS_TO_NS(x)	(x * 1000000)

#define MHL_NOT_USED_FEATURE		1

DECLARE_WAIT_QUEUE_HEAD(wake_wq);

static struct hrtimer hr_wake_timer;

static bool wakeup_time_expired;

static bool hrtimer_initialized;
static bool first_timer;
static struct kobject *uevent_rcp;

enum hrtimer_restart hrtimer_wakeup_callback(struct hrtimer *timer)
{
	wake_up(&wake_wq);
	wakeup_time_expired = true;
	/*hrtimer_cancel(&hr_wake_timer);*/
	return HRTIMER_NORESTART;
}


void start_hrtimer_ms(unsigned long delay_in_ms)
{
	ktime_t ktime;
	ktime = ktime_set(0, MS_TO_NS(delay_in_ms));

	wakeup_time_expired = false;
	/*hrtimer_init(&hr_wake_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);*/
	if (first_timer)
		first_timer = false;
	else
		hrtimer_cancel(&hr_wake_timer);

	/*hr_wake_timer.function = &hrtimer_wakeup_callback;*/
	hrtimer_start(&hr_wake_timer, ktime, HRTIMER_MODE_REL);
}

/*  To remember the current power state. */
static byte fwPowerState = TX_POWER_STATE_FIRST_INIT;

#ifndef MHL_NOT_USED_FEATURE
/*
 When MHL Fifo underrun or overrun happens, we set this flag
 to avoid calling a function in recursive manner. The monitoring loop
 would look at this flag and call appropriate function and clear this flag.
*/
static	bool	gotFifoUnderRunOverRun = FALSE;

/*
 This flag is set to TRUE as soon as a INT1 RSEN CHANGE interrupt arrives and
 a deglitch timer is started.

 We will not get any further interrupt so the RSEN LOW status needs to be polled
 until this timer expires.
*/
static	bool	deglitchingRsenNow = FALSE;
#endif
/*
 To serialize the RCP commands posted to the CBUS engine, this flag
 is maintained by the function SiiMhlTxDrvSendCbusCommand()
*/
static	bool		mscCmdInProgress;/* FALSE when it is okay
					to send a new command*/
/*
Preserve Downstream HPD status
*/
static	byte	dsHpdStatus;

byte mhl_cable_status = MHL_INIT_POWER_OFF;

#define	MHL_MAX_RCP_KEY_CODE	(0x7F + 1)	/* inclusive*/
/* refer to sii9234_driver.h explained values */
static byte rcpSupportTable[MHL_MAX_RCP_KEY_CODE] = {
	(MHL_DEV_LD_GUI), /* 0x00 = Select8i*/
	(MHL_DEV_LD_GUI),
	(MHL_DEV_LD_GUI),
	(MHL_DEV_LD_GUI),
	(MHL_DEV_LD_GUI),
	0, 0, 0, 0, /* 05-08 Reserved*/
	(MHL_DEV_LD_GUI),
	0, 0, 0,
	(MHL_DEV_LD_GUI),
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,/*0E-1F Reserved*/
	0,	/* Numeric keys 0x20-0x29*/
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,	/*0x2A = Dot*/
	0,	/* Enter key = 0x2B*/
	0,	/* Clear key = 0x2C*/
	0, 0, 0,        /* 2D-2F Reserved*/
	0, /*(MHL_DEV_LD_TUNER),             // 0x30 = Channel Up*/
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0, 0, 0, 0, 0, 0, 0, /* 0x39-0x3F Reserved*/
	0, /* 0x40 = Undefined*/

	0,
	0,
	0,
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	/* 0x44 = Play*/
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),
	0,
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	/* 0x48 = Rewind*/
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),	/* 0x49 = Fast Forward*/
	0,/*	(MHL_DEV_LD_MEDIA),		// 0x4A = Eject*/
	0,/*	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),*/
	0,/*	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_MEDIA),*/
	0, 0, 0,				/* 4D-4F Reserved*/
	0,						/* 0x50 = Angle*/
	0,						/* 0x51 = Subpicture*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 52-5F Reserved*/
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO),
	0,/*	(MHL_DEV_LD_RECORD),	// 0x62 = Record Function*/
	0,/*	(MHL_DEV_LD_RECORD),	// 0x63 = Pause the Record Function*/
	(MHL_DEV_LD_VIDEO | MHL_DEV_LD_AUDIO | MHL_DEV_LD_RECORD),

	0,/*	(MHL_DEV_LD_SPEAKER),	// 0x65 = Mute Function*/
	0,/*	(MHL_DEV_LD_SPEAKER),	// 0x66 = Restore Mute Function*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* Undefined or reserved*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	/* Undefined or reserved*/
};
static bool vbus_mhl_est_fail = FALSE;
static bool mhl_vbus = FALSE;
/*===========================================================================
		FUNCTION DEFINITIONS
===========================================================================*/
static	void	int4_isr(void);
static	void	int1_rsen_isr(void);
static	void	mhl_cbus_isr(void);
#ifndef MHL_NOT_USED_FEATURE
static void deglitch_rsen_low(void);
#endif
static	void	init_cbus_regs(void);
static	void	force_usb_id_switch_open(void);
static	void	release_usb_id_switch_open(void);
static	void	apply_ddc_abort_safety(void);
/*
Store global config info here. This is shared by the driver.
structure to hold operating information of MhlTx component
*/
static	mhlTx_config_t	mhlTxConfig;
/*
Functions used internally.
*/
static	bool	sii_mhltx_rapk_send(void);
static	void	mhltx_drive_states(void);
static	void	mhltx_reset_states(void);
static	bool	mhltx_send_msc_msg(byte command, byte cmdData);

/*
*  FUNCTION DelayMS

*  DESCRIPTION

*  DEPENDENCIES
*  None

*  RETURN VALUE
*  None

*  SIDE EFFECTS
*  None
*/
void delay_ms(word msec)
{
	msleep(msec);
}

/*
*  FUNCTION sii9234_HW_Reset

*  DESCRIPTION
*  SiI9024A HW reset

*  DEPENDENCIES
*  None

*  RETURN VALUE
*  None

*  SIDE EFFECTS
*  None
*/
void sii9234_hw_reset(void)
{
	pr_info(">>sii9234_hw_reset()\n");
	/*nothing to do!!*/
}

/*
*  FUNCTION sii9234_startTPI

*  DESCRIPTION
*  Change the TPI mode (SW->H/W TPI mode)

*  DEPENDENCIES
*  None

*  RETURN VALUE
*  None

*  SIDE EFFECTS
*  None
*/
bool sii9234_start_tpi(void)
{
	byte devID = 0x00;
	pr_info(">>StartTPI()\n");
	/*Write "0" to 72:C7 to start HW TPI mode*/
	write_byte_tpi(TPI_ENABLE, 0x00);
	delay_ms(100);

	devID = read_byte_tpi(TPI_DEVICE_ID);
	if (devID == SiI_DEVICE_ID || devID == SiI_DEVICE_ID_9234) {
		pr_info("######## Silicon Device Id: %x\n", devID);
		return TRUE;
	}

	pr_info("Unsupported TX\n");
	return FALSE;
}

/*
*  FUNCTION sii9234_initialize

*  DESCRIPTION
*  sii9234 initialization function.

*  DEPENDENCIES
*  None

*  RETURN VALUE
*  None

*  SIDE EFFECTS
*  None
*/
bool sii9234_initialize(struct kobject *rcp_kobj)
{
	uevent_rcp = rcp_kobj;
	/*bool status;*/
	pr_info("# sii9234 initialization start~\n");

	sii_mhltx_initialize();
	return TRUE;
}
/*
* SiiMhlTxChipInitialize

* Chip specific initialization.
* This function is for SiI 9244 Initialization: HW Reset, Interrupt enable.
*/
bool sii_mhltx_chip_initialize(void)
{
	TX_DEBUG_PRINT(("Drv:sii_mhltx_chip_initialize: 0x%X\n",
			(int)i2c_read_byte(SA_TX_Page0_Primary, 0x03)));

	/* setup device registers. Ensure RGND interrupt would happen.*/
	write_initialize_register_values();
	/* Setup interrupt masks for all those we are interested.*/
	MASK_INTR_4_INTERRUPTS;
	MASK_INTR_1_INTERRUPTS;
	switch_to_D3();
	return TRUE;
}
/*
* SiiMhlTxDeviceIsr

* This function must be called from a master interrupt handler or any polling
* loop in the host software if during initialization call the parameter
* interruptDriven was set to TRUE. SiiMhlTxGetEvents will not look at these
* events assuming firmware is operating in interrupt driven mode. MhlTx
* component performs a check of all its internal status registers to see if a
* hardware event such as connection or disconnection has happened or an RCP
* message has been received from the connected device. Due to the
* interruptDriven being TRUE, MhlTx code will ensure concurrency by asking the
* host software and hardware to disable interrupts and restore when completed.
* Device interrupts are cleared by the MhlTx component before returning back
* to the caller. Any handling of programmable interrupt controller logic if
* present in the host will have to be done by the caller after this function
* returns back.

* This function has no parameters and returns nothing.

* This is the master interrupt handler for 9244. It calls sub handlers
* of interest. Still couple of status would be required to be picked up
* in the monitoring routine (Sii9244TimerIsr)

* To react in least amount of time hook up this ISR to processor's
* interrupt mechanism.

* Just in case environment does not provide this, set a flag so we
* call this from our monitor (Sii9244TimerIsr) in periodic fashion.

* Device Interrupts we would look at
*		RGND		= to wake up from D3
*		MHL_EST		= connection establishment
*		CBUS_LOCKOUT	= Service USB switch
*		RSEN_LOW	= Disconnection deglitcher
*		CBUS\		= responder to peer messages
*				Especially for DCAP etc time based events
*/
void	sii_mhltx_device_isr(void)
{
	byte tmp;
	if (TX_POWER_STATE_D0_MHL != fwPowerState) {
		/* Check important RGND, MHL_EST, CBUS_LOCKOUT and
		SCDT interrupts  During D3 we only get RGND but same
		ISR can work for both states*/
		int4_isr();
		if (mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT)
			return;
		tmp = read_byte_cbus(0x08);
		write_byte_cbus(0x08, tmp);
		tmp = read_byte_cbus(0x1E);
		write_byte_cbus(0x1E, tmp);
	} else if (TX_POWER_STATE_D0_MHL == fwPowerState) {
		/*
		int1_rset_isr();

		Check for any peer messages for DCAP_CHG etc
		Dispatch to have the CBUS module working only once connected.*/
		mhl_cbus_isr();
	}
	int1_rsen_isr();
}
/*
* SiiMhlTxDriverTmdsControl

* Control the TMDS output. MhlTx uses this to support RAP content on and off.
*/
void	sii_mhltx_drv_tmds_control(bool enable)
{
	if (enable) {
		SET_BIT(SA_TX_Page0_Primary, 0x80, 4);
		TX_DEBUG_PRINT(("Drv: TMDS Output Enabled\n"));
		{
			byte rsen  = i2c_read_byte(SA_TX_Page0_Primary,
								0x09) & BIT_2;
			if (vbus_mhl_est_fail == TRUE) {
				if (rsen == 0x00) {
					TX_DEBUG_PRINT(("TMDS RSEN\n"));
					CLR_BIT(SA_TX_Page0_Primary, 0x80, 4);
					mhltx_drvprocess_disconnection();
					return;
				} else {
					vbus_mhl_est_fail = FALSE;
				}
			}
		}
	} else {
		CLR_BIT(SA_TX_Page0_Primary, 0x80, 4);
		TX_DEBUG_PRINT(("Drv: TMDS Ouput Disabled\n"));
	}
}
/*
* SiiMhlTxDrvNotifyEdidChange

* MhlTx may need to inform upstream device of a EDID change. This can be
* achieved by toggling the HDMI HPD signal or by simply calling EDID read
* function.
*/
void	sii_mhltx_drvnotify_edidchange(void)
{
	TX_DEBUG_PRINT(("Drv: sii_mhltx_drvnotify_edidchange\n"));
	/* Prepare to toggle HPD to upstream
	// set reg_hpd_out_ovr_en to first control the hpd*/
	SET_BIT(SA_TX_Page0_Primary, 0x79, 4);

	/*reg_hpd_out_ovr_val = LOW to force the HPD low*/
	CLR_BIT(SA_TX_Page0_Primary, 0x79, 5);

	/* wait a bit*/
	delay_ms(110);

	/*release HPD back to high by reg_hpd_out_ovr_val = HIGH*/
	SET_BIT(SA_TX_Page0_Primary, 0x79, 5);
}
/*
* Function:    SiiMhlTxDrvSendCbusCommand

* Write the specified Sideband Channel command to the CBUS.
* Command can be a MSC_MSG command (RCP/RAP/RCPK/RCPE/RAPK), or another command
* such as READ_DEVCAP, SET_INT, WRITE_STAT, etc.

* Parameters:
*	pReq    - Pointer to a cbus_req_t structure containing the
*		command to write
* Returns:     TRUE    - successful write
		FALSE   - write failed
*/
bool sii_mhltx_drvsend_cbuscommand(struct cbus_req_t *pReq)
{
	bool  success = TRUE;
	byte i, startbit;

	/* If not connected, return with error*/
	/*if( (TX_POWER_STATE_D0_MHL != fwPowerState ) ||
			 (0 == read_byte_cbus(0x0a) || mscCmdInProgress))*/
	if ((TX_POWER_STATE_D0_MHL != fwPowerState) ||
						mscCmdInProgress) {
		TX_DEBUG_PRINT(("Error: Drv: fwPowerState: %02X,"
				 "CBUS[0x0A]: %02X or mscCmdInProgress = %d\n",
				(int) fwPowerState,
				(int) read_byte_cbus(0x0a),
				(int) mscCmdInProgress));

		return FALSE;
	}
	/*Now we are getting busyi*/
	mscCmdInProgress	= TRUE;

	TX_DEBUG_PRINT(("Drv: Sending MSC command %02X, %02X, %02X, %02X\n",
			(int)pReq->command, (int)(pReq->offsetData),
			(int)pReq->msgData[0], (int)pReq->msgData[1]));

/*******************************************************************/
/* Setup for the command - write appropriate registers and determine
						the correct
		start bit.                                        */
/*******************************************************************/

	/* Set the offset and outgoing data byte right away*/
	write_byte_cbus(0x13, pReq->offsetData);
	write_byte_cbus(0x14, pReq->msgData[0]);

	startbit = 0x00;
	switch (pReq->command) {
		/*Set one interrupt register = 0x60*/
	case MHL_SET_INT:
		write_byte_cbus((REG_CBUS_PRI_ADDR_CMD & 0xFF),
						pReq->offsetData + 0x20);
		startbit = MSC_START_BIT_WRITE_REG;
		break;
		/*Write one status register = 0x60 | 0x80*/
	case MHL_WRITE_STAT:
		write_byte_cbus((REG_CBUS_PRI_ADDR_CMD & 0xFF),
					 pReq->offsetData + 0x30);
		startbit = MSC_START_BIT_WRITE_REG;
		break;
		/* Read one device capability register = 0x61*/
	case MHL_READ_DEVCAP:
		startbit = MSC_START_BIT_READ_REG;
		break;
		/*0x62 -	*/
	case MHL_GET_STATE:
		/*0x63 - for vendor id*/
	case MHL_GET_VENDOR_ID:
		/* 0x64	- Set Hot Plug Detect in follower*/
	case MHL_SET_HPD:
		/*0x65	- Clear Hot Plug Detect in follower*/
	case MHL_CLR_HPD:
		/* 0x69	- Get channel 1 command error code*/
	case MHL_GET_SC1_ERRORCODE:
		/* 0x6A	- Get DDC channel command error code.*/
		/*case MHL_GET_DDC_ERRORCODE:*/
		/*0x6B	- Get MSC command error code.*/
	case MHL_GET_MSC_ERRORCODE:
		/*0x6D- Get channel 3 command error code.*/
	case MHL_GET_SC3_ERRORCODE:
		write_byte_cbus((REG_CBUS_PRI_ADDR_CMD & 0xFF),
							 pReq->command);
		startbit = MSC_START_BIT_MSC_CMD;
		break;
		/* 0x6A- Get DDC channel command error code.*/
	case MHL_GET_DDC_ERRORCODE:
		write_byte_cbus((REG_CBUS_PRI_ADDR_CMD & 0xFF), 0x00);
		startbit = MSC_START_BIT_MSC_CMD;
		break;
	case MHL_MSC_MSG:
		write_byte_cbus((REG_CBUS_PRI_WR_DATA_2ND & 0xFF),
							pReq->msgData[1]);
		write_byte_cbus((REG_CBUS_PRI_ADDR_CMD & 0xFF),
							 pReq->command);
		startbit = MSC_START_BIT_VS_CMD;
		break;
	case MHL_WRITE_BURST:
		write_byte_cbus((REG_CBUS_PRI_ADDR_CMD & 0xFF),
						pReq->offsetData + 0x40);
		write_byte_cbus((REG_MSC_WRITE_BURST_LEN & 0xFF),
							pReq->length - 1);
		/*Now copy all bytes from array to local scratchpad*/
		for (i = 0; i < pReq->length; i++) {
			write_byte_cbus((REG_CBUS_SCRATCHPAD_0 & 0xFF)
						 + i, pReq->msgData[i]);
		}
		startbit = MSC_START_BIT_WRITE_BURST;
		break;
	default:
		success = FALSE;
		break;
	}
    /*************************************************************************/
    /* Trigger the CBUS command transfer using the determined start bit.     */
    /*************************************************************************/
	if (success)
		write_byte_cbus(REG_CBUS_PRI_START & 0xFF, startbit);

	return success;
}
/*
* L O C A L    F U N C T I O N S

* Int1RsenIsr

* This interrupt is used only to decide if the MHL is disconnected
* The disconnection is determined by looking at RSEN LOW and applying
* all MHL compliant disconnect timings and deglitch logic.

*	Look for interrupts on INTR_1 (Register 0x71)
*		7 = RSVD		(reserved)
*		6 = MDI_HPD		(interested)
*		5 = RSEN CHANGED(interested)
*		4 = RSVD		(reserved)
*		3 = RSVD		(reserved)
*		2 = RSVD		(reserved)
*		1 = RSVD		(reserved)
*		0 = RSVD		(reserved)
*/
void	int1_rsen_isr(void)
{
	byte	reg71 = i2c_read_byte(SA_TX_Page0_Primary, 0x71);
	byte	rsen  = i2c_read_byte(SA_TX_Page0_Primary, 0x09) & BIT_2;

	/* Look at RSEN interrupt*/
	if (reg71 & BIT_5) {
		TX_DEBUG_PRINT(("Drv: Got2 INTR_1: reg71 = %02X,"
				 "rsen = %02X\n", (int) reg71, (int) rsen));
		/*pinCBusToggle = 1;	// for debug measurements. RSEN intr

		RSEN becomes LOW in SYS_STAT register 0x72:0x09[2]
		SYS_STAT	==> bit 7 = VLOW, 6:4 = MSEL, 3 = TSEL,
				 2 = RSEN, 1 = HPD, 0 = TCLK STABLE

		Start countdown timer for deglitch
		Allow RSEN to stay low this much before reactingi*/
		if (rsen == 0x00) {
			if (TX_POWER_STATE_D0_MHL != fwPowerState) {
				TX_DEBUG_PRINT(("Drv: Got1 INTR_1:"
						"reg71 = %02X, rsen = %02X\n",
						(int) reg71, (int) rsen));
				i2c_write_byte(SA_TX_Page0_Primary,
								0x71, reg71);
				return;
			}

			if (mhl_cable_status & MHL_1K_IMPEDANCE_VERIFIED) {
				TX_DEBUG_PRINT((KERN_ERR "RSEN Low and 1K"
							 "impedance\n"));
				delay_ms(100);
				if ((i2c_read_byte(SA_TX_Page0_Primary, 0x09)
							 & BIT_2) == 0x00) {
					TX_DEBUG_PRINT((KERN_ERR "Really"
								 "RSEN Low\n"));
					/*mhl_cable_status =
							MHL_INIT_POWER_OFF;*/
					mhl_cable_status =
						MHL_TV_OFF_CABLE_CONNECT;
					/*
					#ifdef CONFIG_USB_SWITCH_FSA9485
					FSA9480_MhlSwitchSel(0);
					#endif
					*/
				} else {
					TX_DEBUG_PRINT((KERN_ERR "RSEN "
								"Stable\n"));
				}
			} else {
				pr_err(KERN_ERR "%s: MHL Cable"
						 "disconnect### 2\n", __func__);
				mhl_cable_status = MHL_INIT_POWER_OFF;
				/*
				#ifdef CONFIG_USB_SWITCH_FSA9485
				FSA9480_MhlSwitchSel(0);
				pr_err(KERN_ERR "MHL_SEL to 0\n");
				#endif
				*/
			}

			mhl_hpd_handler(false);

			return;
		} else if (rsen == 0x04) {
			mhl_cable_status |= MHL_RSEN_VERIFIED;
			pr_info("MHL Cable Connection ###\n");

		}
		/* Clear MDI_RSEN interrupt*/
	}

	if (reg71 & BIT_6) {
		byte cbusInt;
		/*HPD*/
		pr_info("HPD\n");
		/* Check if a SET_HPD came from the downstream device.*/
		cbusInt = read_byte_cbus(0x0D);
		/* CBUS_HPD status bit*/
		if ((BIT_6 & cbusInt) != dsHpdStatus) {
			/*Inform upper layer of change in Downstream HPD*/
			sii_mhltx_notify_dshpd_change(cbusInt);
			TX_DEBUG_PRINT(("Drv: Downstream HPD changed to:"
						 "%02X\n", (int) cbusInt));
			/* Remember*/
			dsHpdStatus = (BIT_6 & cbusInt);
			if (!dsHpdStatus) {
				UNMASK_CBUS1_INTERRUPTS;
				UNMASK_CBUS2_INTERRUPTS;
				mhl_cable_status = MHL_INIT_POWER_OFF;
				mhl_hpd_handler(false);
				dsHpdStatus = 0;
			} else {
				MASK_CBUS1_INTERRUPTS;
				MASK_CBUS2_INTERRUPTS;

				mhl_hpd_handler(true);
				dsHpdStatus = 1;
			}
		}

	}

	i2c_write_byte(SA_TX_Page0_Primary, 0x71, reg71);
}
/*
* DeglitchRsenLow

* This function looks at the RSEN signal if it is low.

* The disconnection will be performed only if we were in fully MHL connected
* state for more than 400ms AND a 150ms deglitch from last interrupt on RSEN
* has expired.

* If MHL connection was never established but RSEN was low, we unconditionally
* and instantly process disconnection.
*/
#ifndef MHL_NOT_USED_FEATURE
static void deglitch_rsen_low(void)
{
	TX_DEBUG_PRINT(("Drv: deglitch_rsen_low RSEN <72:09[2]>"
		 "= %02X\n", (int) (i2c_read_byte(SA_TX_Page0_Primary, 0x09))));

	if ((i2c_read_byte(SA_TX_Page0_Primary, 0x09) & BIT_2) == 0x00) {
		TX_DEBUG_PRINT(("Drv: RSEN is Low.\n"));
		/*
		If no MHL cable is connected or RSEN deglitch timer has started,
		we may not receive interrupts for RSEN.
		Monitor the status of RSEN here.

		First check means we have not received any interrupts and
		just started but RSEN is low. Case of "nothing" connected
		on MHL receptacle*/
		if (TX_POWER_STATE_D0_MHL == fwPowerState) {
			/*Second condition means we were fully operational,
			then a RSEN LOW interrupt  occured and a DEGLITCH_TIMER
			per MHL specs started and completed.
			 We can disconnect now.*/
			TX_DEBUG_PRINT(("Drv:Disconnection due to"
							 "RSEN Low\n"));

			deglitchingRsenNow = FALSE;

			/*pinCBusToggle = 0;
			// for debug measurements - disconnected due to RSEN*/
			/* FP1226: Toggle MHL discovery to level the voltage
						to deterministic vale.*/
			DISABLE_DISCOVERY;
			ENABLE_DISCOVERY;
			/* We got here coz cable was never connected*/
			mhltx_drvprocess_disconnection();
		}
	} else {
		/* Deglitch here:
		 RSEN is not low anymore. Reset the flag.
		 This flag will be now set on next interrupt.
		Stay connected*/
		deglitchingRsenNow = FALSE;
	}
}
#endif
/*
* WriteInitialRegisterValues
*/
void write_initialize_register_values(void)
{
	TX_DEBUG_PRINT(("Drv: write_initialize_register_values\n"));
	/*Power Up*/
	/*Power up CVCC 1.2V core*/
	i2c_write_byte(SA_TX_Page1_Primary, 0x3D, 0x3F);
	/*Enable TxPLL Clock*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x11, 0x01);
	/*Enable Tx Clock Path & Equalizer*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x12, 0x15);
	/*Power Up TMDS Tx Core*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x08, 0x35);
	/* Analog PLL Control*/
	/* bits 5:4 = 2b00 as per characterization team.*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x10, 0xC1);
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x17, 0x03);/* PLL Calrefsel*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x1A, 0x20);/* VCO Cal*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x22, 0x8A);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x23, 0x6A);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x24, 0xAA);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x25, 0xCA);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x26, 0xEA);/* Auto EQ*/
	/* Manual zone control*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x4C, 0xA0);
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x4D, 0x00);/* PLL Mode Value*/
	/*Enable Rx PLL Clock Value*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x80, 0x34);
	/* Rx PLL BW value from I2C*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x45, 0x44);
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x31, 0x0A);/*Rx PLL BW ~ 4MHz*/
	i2c_write_byte(SA_TX_Page0_Primary, 0xA0, 0xD0);
	/*Disable internal MHL driver*/
	i2c_write_byte(SA_TX_Page0_Primary, 0xA1, 0xFC);
	i2c_write_byte(SA_TX_Page0_Primary, 0xA3, 0xEA);
	i2c_write_byte(SA_TX_Page0_Primary, 0xA6, 0x0C);
	/*Enable HDCP Compliance safety*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x2B, 0x01);

	/*
	CBUS & Discovery
	CBUS discovery cycle time for each drive and float = 150us
	*/
	read_modify_write_tpi(0x90, BIT_3 | BIT_2, BIT_3);
	/*Clear bit 6 (reg_skip_rgnd)*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x91, 0xA5);


	/*Changed from 66 to 65 for 94[1:0] = 01 = 5k reg_cbusmhl_pup_sel*/
	/* 1.8V CBUS VTH & GND threshold*/
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x94, 0x65);*/
	/*1.8V CBUS VTH & GND threshold*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x94, 0x75);

	/*set bit 2 and 3, which is Initiator Timeout*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x31,
		i2c_read_byte(SA_TX_CBUS_Primary, 0x31) | 0x0c);
	/* RGND Hysterisis.*/
	/*i2c_write_byte(SA_TX_Page0_Primary, 0xA5, 0xAC);*/
	i2c_write_byte(SA_TX_Page0_Primary, 0xA5, 0xA0);
	TX_DEBUG_PRINT(("Drv: MHL 1.0 Compliant Clock\n"));
	/* RGND & single discovery attempt (RGND blocking)*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x95, 0x31);

	/* use 1K and 2K setting*/
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x96, 0x22);*/

	/* Use VBUS path of discovery state machine*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x97, 0x00);
	/* Force USB ID switch to open*/
	read_modify_write_tpi(0x95, BIT_6, BIT_6);

/*
* For MHL compliance we need the following settings for register 93 and 94
* Bug 20686

* To allow RGND engine to operate correctly.

* When moving the chip from D2 to D0 (power up, init regs) the values should be
* 94[1:0] = 01  reg_cbusmhl_pup_sel[1:0] should be set for 5k
* 93[7:6] = 10  reg_cbusdisc_pup_sel[1:0] should be set for 10k (default)
* 93[5:4] = 00  reg_cbusidle_pup_sel[1:0] = open (default)
*/

	write_byte_tpi(0x92, 0x86);
	/* change from CC to 8C to match 5K*/
	/* Disable CBUS pull-up during RGND measurement*/
	write_byte_tpi(0x93, 0x8C);
	/* Force upstream HPD to 0 when not in MHL mode.*/
	/*read_modify_write_tpi(0x79, BIT_5 | BIT_4, BIT_4);*/
	/*Set interrupt active high*/
	read_modify_write_tpi(0x79, BIT_1 | BIT_2, 0);

	delay_ms(25);
	/* Release USB ID switch*/
	read_modify_write_tpi(0x95, BIT_6, 0x00);

	/* Enable CBUS discovery*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x90, 0x27);

	cbus_reset();

	init_cbus_regs();

	/* Enable Auto soft reset on SCDT = 0*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x05, 0x04);

	/* HDMI Transcode mode enable*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x0D, 0x1C);

	/*i2c_write_byte(SA_TX_Page0_Primary, 0x78, RGND_RDY_EN);*/
}

/*
* init_cbus_regs(void)
*/
#define MHL_DEVICE_CATEGORY             0x02 /*(MHL_DEV_CAT_SOURCE)*/
#define	MHL_LOGICAL_DEVICE_MAP			(MHL_DEV_LD_AUDIO |\
						MHL_DEV_LD_VIDEO |\
						MHL_DEV_LD_MEDIA |\
						MHL_DEV_LD_GUI)

static void init_cbus_regs(void)
{
	byte		regval;

	TX_DEBUG_PRINT(("Drv: init_cbus_regs\n"));
	/* Increase DDC translation layer timer*/
	/*i2c_write_byte(SA_TX_CBUS_Primary, 0x07, 0x36);*/
	/*for default DDC byte mode*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x07, 0x32);
	/* CBUS Drive Strength*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x40, 0x03);
	/* CBUS DDC interface ignore segment pointer*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x42, 0x06);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x36, 0x0C);

	i2c_write_byte(SA_TX_CBUS_Primary, 0x3D, 0xFD);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x1C, 0x00);

	i2c_write_byte(SA_TX_CBUS_Primary, 0x44, 0x02);

	/*Setup our devcap*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x80, MHL_DEV_ACTIVE);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x81, MHL_VERSION);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x82, MHL_DEVICE_CATEGORY);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x83, 0);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x84, 0);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x85, (MHL_DEV_VID_LINK_SUPPRGB444));
	i2c_write_byte(SA_TX_CBUS_Primary, 0x86, MHL_DEV_AUD_LINK_2CH);
	 /* not for source*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x87, 0);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x88, MHL_LOGICAL_DEVICE_MAP);
	/*not for source*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x89, 0x0F);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x8A,
			MHL_FEATURE_RCP_SUPPORT | MHL_FEATURE_RAP_SUPPORT|
						MHL_FEATURE_SP_SUPPORT);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x8B, 0);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x8C, 0);/*reserved*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x8D, MHL_SCRATCHPAD_SIZE);
	/*HL_INT_AND_STATUS_SIZE);*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x8E, 0x44);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x8F, 0);/*eserved*/

	/*Make bits 2,3 (initiator timeout) to 1,1 for registeri
	 CBUS_LINK_CONTROL_2*/
	regval = i2c_read_byte(SA_TX_CBUS_Primary, REG_CBUS_LINK_CONTROL_2);
	regval = (regval | 0x0C);
	i2c_write_byte(SA_TX_CBUS_Primary, REG_CBUS_LINK_CONTROL_2, regval);

	/*Clear legacy bit on Wolverine TX.*/
	regval = i2c_read_byte(SA_TX_CBUS_Primary, REG_MSC_TIMEOUT_LIMIT);
	i2c_write_byte(SA_TX_CBUS_Primary, REG_MSC_TIMEOUT_LIMIT,
				(regval & MSC_TIMEOUT_LIMIT_MSB_MASK));

	/*Set NMax to 1*/
	i2c_write_byte(SA_TX_CBUS_Primary, REG_CBUS_LINK_CONTROL_1, 0x01);
}
/*
* force_usb_id_switch_open
*/
static void force_usb_id_switch_open(void)
{
	/* Disable CBUS discovery*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x90, 0x26);
	/* Force USB ID switch to open*/
	read_modify_write_tpi(0x95, BIT_6, BIT_6);
	write_byte_tpi(0x92, 0x86);
	/* Force HPD to 0 when not in MHL mode.*/
	read_modify_write_tpi(0x79, BIT_5 | BIT_4, BIT_4);
}
/*
* release_usb_id_switch_open
*/
static void release_usb_id_switch_open(void)
{
	delay_ms(50);
	/* Release USB ID switch*/
	read_modify_write_tpi(0x95, BIT_6, 0x00);
	ENABLE_DISCOVERY;
}
/*
* FUNCTION     : CbusWakeUpPulseGenerator ()

* PURPOSE      : Generate Cbus Wake up pulse sequence using GPIO or I2C method.

* INPUT PARAMS :   None

* OUTPUT PARAMS:   None

* GLOBALS USED :   None

* RETURNS      :   None
*/
void cbus_wakeuppulse_generator(void)
{
	TX_DEBUG_PRINT(("Drv: cbus_wakeuppulse_generator\n"));

	if (!hrtimer_initialized) {
		hrtimer_init(&hr_wake_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		hr_wake_timer.function = &hrtimer_wakeup_callback;
		hrtimer_initialized = true;
		first_timer = true;
	}
	/* I2C method*/
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x92,
		 (i2c_read_byte(SA_TX_Page0_Primary, 0x92) | 0x10));*/

	/*Start the pulse*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) | 0xC0));
	/*mdelay(T_SRC_WAKE_PULSE_WIDTH_1-1);	// adjust for code path*/
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) & 0x3F));
	/*mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path*/
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) | 0xC0));
	/*mdelay(T_SRC_WAKE_PULSE_WIDTH_1-1);	// adjust for code path*/
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) & 0x3F));
	/*mdelay(T_SRC_WAKE_PULSE_WIDTH_2-1);	// adjust for code path*/
	start_hrtimer_ms(60);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) | 0xC0));
	/*mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path*/
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) & 0x3F));
	/*mdelay(T_SRC_WAKE_PULSE_WIDTH_1);	// adjust for code path*/
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			 (i2c_read_byte(SA_TX_Page0_Primary, 0x96) | 0xC0));
	/*mdelay(20);*/
	start_hrtimer_ms(19);
	wait_event_interruptible(wake_wq, wakeup_time_expired);

	i2c_write_byte(SA_TX_Page0_Primary, 0x96,
			(i2c_read_byte(SA_TX_Page0_Primary, 0x96) & 0x3F));
	/*mdelay(T_SRC_WAKE_TO_DISCOVER);*/
	start_hrtimer_ms(T_SRC_WAKE_TO_DISCOVER);
	wait_event_interruptible(wake_wq, wakeup_time_expired);
}
/*
* apply_ddc_abort_safety
*/
static	void	apply_ddc_abort_safety(void)
{
	byte		bTemp, bPost;

	/*TX_DEBUG_PRINT(("[%d] Drv: Do we need DDC Abort Safety\n",
	(int) (HalTimerElapsed( ELAPSED_TIMER ) * MONITORING_PERIOD)));*/

	write_byte_cbus(0x29, 0xFF);
	bTemp = read_byte_cbus(0x29);
	delay_ms(3);
	bPost = read_byte_cbus(0x29);

	if ((bPost > (bTemp + 50))) {
		TX_DEBUG_PRINT(("Drv: Applying DDC Abort"
					"Safety(SWWA 18958)\n"));

		SET_BIT(SA_TX_Page0_Primary, 0x05, 3);
		CLR_BIT(SA_TX_Page0_Primary, 0x05, 3);

		init_cbus_regs();

		/* Why do we do these?*/
		force_usb_id_switch_open();
		release_usb_id_switch_open();

		mhltx_drvprocess_disconnection();
	}
}
/*
* ProcessRgnd

* H/W has detected impedance change and interrupted.
* We look for appropriate impedance range to call it MHL and enable the
* hardware MHL discovery logic. If not, disable MHL discovery to allow
* USB to work appropriately.

* In current chip a firmware driven slow wake up pulses are sent to the
* sink to wake that and setup ourselves for full D0 operation.
*/
void	process_rgnd(void)
{
	byte		reg99RGNDRange;
	/* Impedance detection has completed - process interrupt*/
	reg99RGNDRange = i2c_read_byte(SA_TX_Page0_Primary, 0x99) & 0x03;
	TX_DEBUG_PRINT(("Drv: RGND Reg 99 = %02X : ", (int)reg99RGNDRange));

	/* Keeping DisableFSAinterrupt affects fast connect/disconnect */
	/* But disabling fsa interrupts ... and
	removing the power adapter cable from the mhl active
	while cable is connectedi
	gives multiple FSA interrupts .. Need to find a proper solution.*/

	/*DisableFSA9480Interrupts(); add this code to fsa9485*/
	/* Reg 0x99
		00 or 11 means USB.
		10 means 1K impedance (MHL)
		01 means 2K impedance (MHL)*/
	if (reg99RGNDRange == 0x00 || reg99RGNDRange == 0x03) {
		TX_DEBUG_PRINT((" : USB impedance."
			"Disable MHL discovery =%02X\n", (int)reg99RGNDRange));
		if (vbus_mhl_est_fail == TRUE) {
			mhltx_drvprocess_disconnection();
			return;
		}

		CLR_BIT(SA_TX_Page0_Primary, 0x95, 5);
		mhl_cable_status = MHL_INIT_POWER_OFF;
		TX_DEBUG_PRINT((KERN_ERR "MHL_SEL to 0\n"));
		/*#if 0
		FSA9480_CheckAndHookAudioDock(0);
		#endif*/
	} else {
		mhl_cable_status |= MHL_1K_IMPEDANCE_VERIFIED;
		if (0x01 == reg99RGNDRange) {
			TX_DEBUG_PRINT(("MHL 2K\n"));
			mhl_cable_status = MHL_TV_OFF_CABLE_CONNECT;
			pr_err("MHL Connection Fail Power off ###1\n");
			/*
			#ifdef CONFIG_USB_SWITCH_FSA9485
			FSA9480_MhlTvOff();
			#endif
			*/
			return ;
		} else if (0x02 == reg99RGNDRange) {
			/*#if 0
			FSA9480_CheckAndHookAudioDock(1);
			#endif*/
			TX_DEBUG_PRINT(("MHL 1K\n"));
			delay_ms(T_SRC_VBUS_CBUS_TO_STABLE);
			/*Discovery enabled*/
			i2c_write_byte(SA_TX_Page0_Primary, 0x90, 0x25);
			/*Send slow wake up pulse using GPIO or I2C*/
			cbus_wakeuppulse_generator();
			delay_ms(T_SRC_WAKE_PULSE_WIDTH_2);
			/*
			#ifdef CONFIG_USB_SWITCH_FSA9485
			EnableFSA9480Interrupts();
			#endif
			*/
		}
	}
}
/*
* switch_to_D0
* This function performs s/w as well as h/w state transitions.

* Chip comes up in D2. Firmware must first bring it to full operation
* mode in D0.
*/
void	switch_to_D0(void)
{
/* write_initialize_register_values switches the chip to full power mode.*/
	write_initialize_register_values();
	/* Setup interrupt masks for all those we are interested.*/
	MASK_INTR_1_INTERRUPTS;
	/*Force Power State to ON*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x90, 0x25);

	fwPowerState = TX_POWER_STATE_D0_NO_MHL;
	mhl_cable_status = MHL_POWER_ON;
}
/*
* switch_to_D3

* This function performs s/w as well as h/w state transitions.
*/
void	switch_to_D3(void)
{
	/*
	To allow RGND engine to operate correctly.
	So when moving the chip from D0 MHL connected to D3 the values should be
	94[1:0] = 00  reg_cbusmhl_pup_sel[1:0] should be set for open
	93[7:6] = 00  reg_cbusdisc_pup_sel[1:0] should be set for open
	93[5:4] = 00  reg_cbusidle_pup_sel[1:0] = open (default)

	Disable CBUS pull-up during RGND measurement
	i2c_write_byte(SA_TX_Page0_Primary, 0x93, 0x04);*/
	read_modify_write_tpi(0x93, BIT_7 | BIT_6 | BIT_5 | BIT_4, 0);
	read_modify_write_tpi(0x93, BIT_7 | BIT_6 | BIT_5 | BIT_4, 0);

	read_modify_write_tpi(0x94, BIT_1 | BIT_0, 0);

	/* 1.8V CBUS VTH & GND threshold*/
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x94, 0x64);*/

	release_usb_id_switch_open();
	pr_info("TX_POWER_STATE_D3\n");
	/* Change TMDS termination to high impedance on disconnection
	 Bits 1:0 set to 11*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x01, 0x03);
	/*Change state to D3 by clearing bit 0 of 3D (SW_TPI, Page 1) register
	// ReadModifyWriteIndexedRegister(INDEXED_PAGE_1, 0x3D, BIT_0, 0x00);*/
	CLR_BIT(SA_TX_Page1_Primary, 0x3D, 0);

	fwPowerState = TX_POWER_STATE_D3;

}
/*
* FUNCTION For_check_resen_int

* DESCRIPTION
* For_check_resen_int

*  DEPENDENCIES
*  None

*  RETURN VALUE
*  None

*  SIDE EFFECTS
*  None
*/
#ifndef MHL_NOT_USED_FEATURE
static void For_check_resen_int(void)
{
	/* Power Up*/
	/*Power up CVCC 1.2V core*/
	i2c_write_byte(SA_TX_Page1_Primary, 0x3D, 0x3F);
	/*Enable TxPLL Clock*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x11, 0x01);
	/*Enable Tx Clock Path & Equalizer*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x12, 0x15);
	/* Power Up TMDS Tx Core*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x08, 0x35);

	/* Analog PLL Control*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x17, 0x03);/* PLL Calrefsel*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x1A, 0x20);/* VCO Cal*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x22, 0x8A);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x23, 0x6A);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x24, 0xAA);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x25, 0xCA);/* Auto EQ*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x26, 0xEA);/*Auto EQ*/
	/*Manual zone control*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x4C, 0xA0);
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x4D, 0x00);/* PLL Mode Value*/
	/* Enable Rx PLL Clock Value*/
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x80, 0x34);*/

	/* Enable Rx PLL Clock Value*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x80, 0x24);

	/*/ Rx PLL BW value from I2C*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x45, 0x44);
	/*Rx PLL BW ~ 4MHz*/
	i2c_write_byte(SA_TX_HDMI_RX_Primary, 0x31, 0x0A);
	i2c_write_byte(SA_TX_Page0_Primary, 0xA0, 0xD0);
	/*Disable internal Mobile HD driver*/
	i2c_write_byte(SA_TX_Page0_Primary, 0xA1, 0xFC);

	i2c_write_byte(SA_TX_Page0_Primary, 0xA3, 0xEA);
	i2c_write_byte(SA_TX_Page0_Primary, 0xA6, 0x0C);
	/*Enable HDCP Compliance workaround*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x2B, 0x01);

	/* CBUS & Discovery*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x91, 0xA5);
	/*1.8V CBUS VTH & GND threshold*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x94, 0x66);
	/*set bit 2 and 3, which is Initiator Timeout*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x31,
			i2c_read_byte(SA_TX_CBUS_Primary, 0x31) | 0x0c);

	/* RGND Hysterisis.*/
	i2c_write_byte(SA_TX_Page0_Primary, 0xA5, 0xAC);
	/*RGND & single discovery attempt (RGND blocking)*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x95, 0x31);
	/*use 1K and 2K setting*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x96, 0x22);
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x97, 0x03);
				Auto Heartbeat failure enable*/

	/*Force USB ID switch to open*/
	read_modify_write_tpi(0x95, BIT_6, BIT_6);

	write_byte_tpi(0x92, 0x86);
	/* Disable CBUS pull-up during RGND measurement*/
	write_byte_tpi(0x93, 0xCC);

	delay_ms(25);
	read_modify_write_tpi(0x95, BIT_6, 0x00);/*Release USB ID switch*/

	/*Set interrupt active high*/
	read_modify_write_tpi(0x79, BIT_1 | BIT_2, 0);

	/*Enable CBUS discovery8*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x90, 0x27);

	/* Reset CBus to clear HPD*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x05, 0x08);
	delay_ms(2);
	i2c_write_byte(SA_TX_Page0_Primary, 0x05, 0x00);
	/* Heartbeat Max Fail Enable*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x1F, 0x02);
	/*Increase DDC translation layer timer*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x07,
			DDC_XLTN_TIMEOUT_MAX_VAL | 0x06);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x40, 0x03);/*CBUS Drive Strength*/
	/* CBUS DDC interface ignore segment pointer*/
	i2c_write_byte(SA_TX_CBUS_Primary, 0x42, 0x06);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x36, 0x0C);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x3D, 0xFD);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x1C, 0x00);
	i2c_write_byte(SA_TX_CBUS_Primary, 0x44, 0x02);
	/* Enable Auto soft reset on SCDT = 0*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x05, 0x04);
	/* HDMI Transcode mode enable*/
	i2c_write_byte(SA_TX_Page0_Primary, 0x0D, 0x1C);

	UNMASK_INTR_4_INTERRUPTS;
	/*i2c_write_byte(SA_TX_Page0_Primary, 0x78, BIT_6) */
	sii9234_start_tpi();
	write_byte_tpi(TPI_INTERRUPT_ENABLE_REG, 0x02);
	/*read_modify_write_tpi(TPI_INTERRUPT_ENABLE_REG, 0x03, 0x03);
					//enable HPD and RSEN interrupt*/

}
#endif
/*
* Int4Isr

*	Look for interrupts on INTR4 (Register 0x74)
*		7 = RSVD		(reserved)
*		6 = RGND Rdy	(interested)
*		5 = VBUS Low	(ignore)
*		4 = CBUS LKOUT	(interested)
*		3 = USB EST		(interested)
*		2 = MHL EST		(interested)
*		1 = RPWR5V Change	(ignore)
*		0 = SCDT Change	(interested during D0)
*/
static	void	int4_isr(void)
{

	byte		reg74;
	reg74 = i2c_read_byte(SA_TX_Page0_Primary, (0x74));

	pr_info("REG74 : %x\n", (int)reg74);

	/* When I2C is inoperational (say in D3) and
	 a previous interrupt brought us here, do nothing.*/
	if (0xFF == reg74) {
		pr_info("RETURN (0xFF == reg74)\n");
		return;
	}
	if (reg74 & BIT_2) { /* MHL_EST_INT*/
		MASK_CBUS1_INTERRUPTS;
		MASK_CBUS2_INTERRUPTS;
		mhltx_drvprocess_connection();
	}
	/*process USB_EST interrupt*/
	else if (reg74 & BIT_3) {/*MHL_DISC_FAIL_INT*/
		if (mhl_cable_status ==
				(MHL_1K_IMPEDANCE_VERIFIED | MHL_POWER_ON)) {
			mhl_cable_status = MHL_TV_OFF_CABLE_CONNECT;
			pr_err(KERN_ERR "MHL Connection Fail Power off ###2\n");

			if (mhl_vbus == TRUE) {
				vbus_mhl_est_fail = TRUE;
				mhltx_drvprocess_disconnection();
				return;
			} else {
			/*
			#ifdef CONFIG_USB_SWITCH_FSA9485
				FSA9480_MhlTvOff();
			#endif
			*/
			}
		} else {
			mhltx_drvprocess_disconnection();
		}
		return;
	}
	if ((TX_POWER_STATE_D3 == fwPowerState) && (reg74 & BIT_6)) {
		/* process RGND interrupt
		 Switch to full power mode.*/
		switch_to_D0();
		/* If a sink is connected but not powered on, this interrupt
		can keep coming
		Determine when to go back to sleep. Say after 1 second of
		this state.
		Check RGND register and send wake up pulse to the peer*/
		process_rgnd();
	}
	/* CBUS Lockout interrupt?*/
	if (reg74 & BIT_4) {
		TX_DEBUG_PRINT(("Drv: CBus Lockout\n"));
		force_usb_id_switch_open();
		release_usb_id_switch_open();
	}
	/* clear all interrupts*/
	i2c_write_byte(SA_TX_Page0_Primary, (0x74), reg74);
}
/*
* mhltx_drvprocess_connection
*/
void mhltx_drvprocess_connection(void)
{
	bool	mhlConnected = TRUE;

	TX_DEBUG_PRINT(("Drv: MHL Cable Connected. CBUS:0x0A = %02X\n",
						 (int) read_byte_cbus(0x0a)));
	if (TX_POWER_STATE_D0_MHL == fwPowerState) {
		TX_DEBUG_PRINT(("POWER_STATE_D0_MHL == fwPowerState\n"));
		return;
	}

	TX_DEBUG_PRINT(("$$$$$$$\n"));
	/* Discovery over-ride: reg_disc_ovride*/
	i2c_write_byte(SA_TX_Page0_Primary, 0xA0, 0x10);

	fwPowerState = TX_POWER_STATE_D0_MHL;

	/* Increase DDC translation layer timer (byte mode)
	// Setting DDC Byte Mode*/
	write_byte_cbus(0x07, 0x32);

	/* Enable segment pointer safety*/
	SET_BIT(SA_TX_CBUS_Primary, 0x44, 1);

	/* Un-force HPD (it was kept low, now propagate to source*/
	CLR_BIT(SA_TX_Page0_Primary, 0x79, 4);

	/*Enable TMDS*/
	sii_mhltx_drv_tmds_control(TRUE);

	/* Keep the discovery enabled. Need RGND interrupt
	// SET_BIT(SA_TX_Page0_Primary, 0x90, 0);*/
	ENABLE_DISCOVERY;

	/*Notify upper layer of cable connection*/
	sii_mhltx_notify_connection(mhlConnected = TRUE);
}
/*
* mhltx_drvprocess_disconnection
*/
void mhltx_drvprocess_disconnection(void)
{
	bool	mhlConnected = FALSE;

	TX_DEBUG_PRINT(("Drv: mhltx_drvprocess_disconnection\n"));

	/* clear all interrupts*/
	i2c_write_byte(SA_TX_Page0_Primary, (0x74),
			 i2c_read_byte(SA_TX_Page0_Primary, (0x74)));

	i2c_write_byte(SA_TX_Page0_Primary, 0xA0, 0xD0);

	/* Reset CBus to clear register contents
	 This may need some key reinitializations*/
	cbus_reset();

	/* Disable TMDS*/
	sii_mhltx_drv_tmds_control(FALSE);

	if (TX_POWER_STATE_D0_MHL == fwPowerState) {
		/*Notify upper layer of cable connection*/
		sii_mhltx_notify_connection(mhlConnected = FALSE);
	}

	/*Now put chip in sleep mode*/
	switch_to_D3();
}
/*
* CbusReset
*/
void	cbus_reset(void)
{
	SET_BIT(SA_TX_Page0_Primary, 0x05, 3);
	delay_ms(2);
	CLR_BIT(SA_TX_Page0_Primary, 0x05, 3);

	mscCmdInProgress = FALSE;

	/*Adjust interrupt mask everytime reset is performed.*/
	UNMASK_CBUS1_INTERRUPTS;
	UNMASK_CBUS2_INTERRUPTS;
}

/*
* CBusProcessErrors
*/
static byte cbus_process_errors(byte intStatus)
{
	byte result          = 0;
	byte mscAbortReason  = 0;
	byte ddcAbortReason  = 0;

	/* At this point, we only need to look at the abort interrupts. */

	intStatus &=  (BIT_MSC_ABORT | BIT_MSC_XFR_ABORT);

	if (intStatus) {
		/*      result = ERROR_CBUS_ABORT;	// No Retry will help*/
		/* If transfer abort or MSC abort, clear the
				 abort reason register. */
		if (intStatus & BIT_DDC_ABORT) {
			result = ddcAbortReason =
					 read_byte_cbus(REG_DDC_ABORT_REASON);
			TX_DEBUG_PRINT(("CBUS DDC ABORT happened,"
				 "reason:: %02X\n", (int)(ddcAbortReason)));
		}

		if (intStatus & BIT_MSC_XFR_ABORT) {
			result = mscAbortReason =
				read_byte_cbus(REG_PRI_XFR_ABORT_REASON);
			TX_DEBUG_PRINT(("CBUS:: MSC Transfer ABORTED."
						"Clearing 0x0D\n"));
			write_byte_cbus(REG_PRI_XFR_ABORT_REASON, 0xFF);
		}
		if (intStatus & BIT_MSC_ABORT) {
			TX_DEBUG_PRINT(("CBUS:: MSC Peer sent an ABORT."
						 "Clearing 0x0E\n"));
			write_byte_cbus(REG_CBUS_PRI_FWR_ABORT_REASON, 0xFF);
		}
		/* Now display the abort reason.*/
		if (mscAbortReason != 0) {
			TX_DEBUG_PRINT(("CBUS:: Reason for ABORT is...."
					"0x%02X = ", (int)mscAbortReason));
			if (mscAbortReason & CBUSABORT_BIT_REQ_MAXFAIL)
				TX_DEBUG_PRINT(("Requestor MAXFAIL - retry"
						 "threshold exceeded\n"));
			if (mscAbortReason & CBUSABORT_BIT_PROTOCOL_ERROR)
				TX_DEBUG_PRINT(("Protocol Error\n"));
			if (mscAbortReason & CBUSABORT_BIT_REQ_TIMEOUT)
				TX_DEBUG_PRINT(("Requestor translation"
							 "layer timeout\n"));
			if (mscAbortReason & CBUSABORT_BIT_PEER_ABORTED)
				TX_DEBUG_PRINT(("Peer sent an abort\n"));
			if (mscAbortReason & CBUSABORT_BIT_UNDEFINED_OPCODE)
				TX_DEBUG_PRINT(("Undefined opcode\n"));
		}
	}
	return result;
}

/*
* MhlCbusIsr

*  Only when MHL connection has been established. This is where we have the
*  first looks on the CBUS incoming commands or returned data bytes for the
*  previous outgoing command.

*  It simply stores the event and allows application to pick up the event
*  and respond at leisure.

*  Look for interrupts on CBUS:CBUS_INTR_STATUS [0xC8:0x08]
*		7 = RSVD			(reserved)
*		6 = MSC_RESP_ABORT	(interested)
*		5 = MSC_REQ_ABORT	(interested)
*		4 = MSC_REQ_DONE	(interested)
*		3 = MSC_MSG_RCVD	(interested)
*		2 = DDC_ABORT		(interested)
*		1 = RSVD			(reserved)
*		0 = rsvd			(reserved)
*/
static void mhl_cbus_isr(void)
{
	byte		cbusInt;
	byte     gotData[4];	/* Max four status and int registers.*/
	byte		i;

	/* Main CBUS interrupts on CBUS_INTR_STATUS*/
	cbusInt = read_byte_cbus(0x08);

	/*When I2C is inoperational (say in D3) and a previous interrupt
	 brought us here, do nothing.*/
	if (cbusInt == 0xFF)
		return;
	if (cbusInt)
		TX_DEBUG_PRINT(("Drv: CBUS INTR_1: %d\n", (int) cbusInt));

	/* Look for DDC_ABORT*/
	if (cbusInt & BIT_2)
		apply_ddc_abort_safety();
	/* MSC_MSG (RCP/RAP)*/
	if ((cbusInt & BIT_3)) {
		TX_DEBUG_PRINT(("Drv: MSC_MSG Received: %02X\n",
							(int) cbusInt));
		/*Two bytes arrive at registers 0x18 and 0x19*/
		sii_mhltx_got_mhlmscmsg(read_byte_cbus(0x18),
						read_byte_cbus(0x19));
	}
/*
  #if 0
	// MSC_REQ_DONE received.
	if(cbusInt & BIT_4)
	{
	    TX_DEBUG_PRINT(("Drv: MSC_REQ_DONE: %02X\n", (int) cbusInt));

		mscCmdInProgress = FALSE;

		sii_mhltx_msc_command_done( read_byte_cbus( 0x16 ) );
	}
  #endif
*/
	if ((cbusInt & BIT_5) || (cbusInt & BIT_6))
		/* MSC_REQ_ABORT or MSC_RESP_ABORT*/
		gotData[0] = cbus_process_errors(cbusInt);

	if (cbusInt) {
		/* Clear all interrupts that were raised even if we did not
		 process*/
		write_byte_cbus(0x08, cbusInt);
		TX_DEBUG_PRINT(("Drv: Clear CBUS INTR_1: %02X\n",
						(int) cbusInt));
	}
	/* MSC_REQ_DONE received.*/
	if (cbusInt & BIT_4) {
		TX_DEBUG_PRINT(("Drv: MSC_REQ_DONE: %02X\n", (int) cbusInt));
		mscCmdInProgress = FALSE;
		sii_mhltx_msc_command_done(read_byte_cbus(0x16));
	}
	/* Clear all interrupts that were raised even if we did not process*/

	/* Now look for interrupts on register 0x1E. CBUS_MSC_INT2
	 7:4 = Reserved
	   3 = msc_mr_write_state = We got a WRITE_STAT
	   2 = msc_mr_set_int. We got a SET_INT
	   1 = reserved
	   0 = msc_mr_write_burst. We received WRITE_BURST*/
	cbusInt = read_byte_cbus(0x1E);
	if (cbusInt)
		TX_DEBUG_PRINT(("Drv: CBUS INTR_2: %x\n", (int) cbusInt));
	if (cbusInt & BIT_2) {
		TX_DEBUG_PRINT(("Drv: INT Received: %x\n", (int) cbusInt));
		/* We are interested only in first two bytes.*/
		sii_mhltx_got_mhlintr(read_byte_cbus(0xA0),
						read_byte_cbus(0xA1));

		for (i = 0; i < 4; i++) {
			/* Clear all*/
			write_byte_cbus((0xA0 + i), read_byte_cbus(0xA0 + i));
		}
	}
	if (cbusInt & BIT_3) {
		TX_DEBUG_PRINT(("Drv: STATUS Received: %x\n", (int) cbusInt));

		/* We are interested only in first two bytes.*/
		sii_mhltx_got_mhlstatus(read_byte_cbus(0xB0),
					read_byte_cbus(0xB1));

		for (i = 0; i < 4; i++) {
			/*Clear all*/
			write_byte_cbus((0xB0 + i), read_byte_cbus(0xB0 + i));
		}
	}
	if (cbusInt) {
		/*Clear all interrupts that were raised even
		if we did not process*/
		write_byte_cbus(0x1E, cbusInt);
		TX_DEBUG_PRINT(("Drv: Clear CBUS INTR_2: %02X\n",
						 (int) cbusInt));
	}
/*
	//
	// Check if a SET_HPD came from the downstream device.
	//
	cbusInt = read_byte_cbus(0x0D);

	// CBUS_HPD status bit
	if((BIT_6 & cbusInt) != dsHpdStatus)
	{
		// Inform upper layer of change in Downstream HPD
		sii_mhltx_notify_dshpd_change( cbusInt );
		TX_DEBUG_PRINT(("Drv: Downstream HPD changed to: %02X\n",
							 (int) cbusInt));

		// Remember
		dsHpdStatus = (BIT_6 & cbusInt);
	}
*/
}

/*
* SiiMhlTxInitialize

* Sets the transmitter component firmware up for operation, brings up chip
* into power on state first and then back to reduced-power mode D3 to conserve
* power until an MHL cable connection has been established. If the MHL port is
* used for USB operation, the chip and firmware continue to stay in D3 mode.
* Only a small circuit in the chip observes the impedance variations to see if
* processor should be interrupted to continue MHL discovery process or not.

* interruptDriven	If TRUE, MhlTx component will not look at its status
*			registers in a polled manner from timer handler
*			(SiiMhlTxGetEvents). It will expect that all device
*			events will result in call to the function
*			SiiMhlTxDeviceIsr() by host's hardware or software
*			(a master interrupt handler in host software can call
*			it directly). interruptDriven == TRUE also implies that
*			the MhlTx component shall make use of
*			AppDisableInterrupts() and AppRestoreInterrupts() for
*			any critical section work to prevent concurrency issues
*			When interruptDriven == FALSE, MhlTx component will do
*			all chip status analysis via looking at its register
*			when called periodically into the function
*			SiiMhlTxGetEvents() described below.

* pollIntervalMs	This number should be higher than 0 and lower than
*			51 milliseconds for effective operation of the firmware.
*			A higher number will only imply a slower response to an
*			event on MHL side which can lead to violation of a
*			connection disconnection related timing or a slower
*			response to RCP messages.
*/
void sii_mhltx_initialize(void)
{
	TX_DEBUG_PRINT(("MhlTx: sii_mhltx_initialize\n"));

	mhltx_reset_states();

	sii_mhltx_chip_initialize();
	mhl_cable_status = MHL_POWER_ON;
}

/*
* SiiMhlTxGetEvents

* This is a function in MhlTx that must be called by application in a periodic
* fashion. The accuracy of frequency (adherence to the parameter pollIntervalMs)
* will determine adherence to some timings in the MHL specifications, however,
* MhlTx component keeps a tolerance of up to 50 milliseconds for most of the
* timings and deploys interrupt disabled mode of operation (applicable only to
* Sii 9244) for creating precise pulse of smaller duration such as 20 ms.

* This function does not return anything but it does modify the contents of the
* two pointers passed as parameter.

* It is advantageous for application to call this function in task context so
* that interrupt nesting or concurrency issues do not arise. In addition, by
* collecting the events in the same periodic polling mechanism prevents a call
* back from the MhlTx which can result in sending yet another MHL message.

* An example of this is responding back to an RCP message by another message
* such as RCPK or RCPE.

* *event	MhlTx returns a value in this field when function completes
*		execution. If this field is 0, the next
		parameter is undefined.
		The following values may be returned.
*/
void rcp_cbus_uevent(u8 rcpCode)
{
	char env_buf[120];
	char *envp[2];
	int env_offset = 0;

	memset(env_buf, 0, sizeof(env_buf));
	TX_DEBUG_PRINT(("%s : RCP Message Recvd ,"
			"rcpCode = 0x%x\n", __func__, rcpCode));
	snprintf(env_buf, sizeof(env_buf), "MHL_RCP=%x", rcpCode);
	envp[env_offset++] = env_buf;
	envp[env_offset] = NULL;

	if (uevent_rcp == NULL) {
		pr_info("error : rcp kobject doesn't have value");
		return;
	}

	kobject_uevent_env(uevent_rcp, KOBJ_CHANGE, envp);
	return;
}


void sii_mhltx_get_events(byte *event, byte *eventParameter)
{

	sii_mhltx_device_isr();

	if (mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT)
		return ;

	mhltx_drive_states();
	*event = MHL_TX_EVENT_NONE;
	*eventParameter = 0;

	if (mhlTxConfig.mhlConnectionEvent) {
		TX_DEBUG_PRINT(("MhlTx: sii_mhltx_get_events"
					"mhlConnectionEvent\n"));
		/*Consume the message*/
		mhlTxConfig.mhlConnectionEvent = FALSE;
		/*Let app know the connection went away.*/
		*event          = mhlTxConfig.mhlConnected;
		*eventParameter	= mhlTxConfig.mscFeatureFlag;
		/*If connection has been lost, reset all state flags.*/
		if (MHL_TX_EVENT_DISCONNECTION == mhlTxConfig.mhlConnected)
			mhltx_reset_states();

	} else if (mhlTxConfig.mscMsgArrived) {
		TX_DEBUG_PRINT(("MhlTx: sii_mhltx_get_events"
					"MSC MSG Arrived\n"));
		/*Consume the message*/
		mhlTxConfig.mscMsgArrived = FALSE;
		/*Map sub-command to an event id*/
		switch (mhlTxConfig.mscMsgSubCommand) {
		case	MHL_MSC_MSG_RAP:
			/*RAP is fully handled here.
			Handle RAP sub-commands here itself*/
			if (MHL_RAP_CONTENT_ON == mhlTxConfig.mscMsgData) {
					sii_mhltx_drv_tmds_control(TRUE);
				} else if (MHL_RAP_CONTENT_OFF ==
						mhlTxConfig.mscMsgData) {
					sii_mhltx_drv_tmds_control(FALSE);
			}
			/*Always RAPK to the peer*/
			sii_mhltx_rapk_send();
			break;
		case	MHL_MSC_MSG_RCP:
			/* If we get a RCP key that we do NOT support,
			send back RCPE Do not notify app layer.*/
			if (MHL_LOGICAL_DEVICE_MAP &
				rcpSupportTable[mhlTxConfig.mscMsgData]) {
				*event          =
					 MHL_TX_EVENT_RCP_RECEIVED;
				*eventParameter =
					 mhlTxConfig.mscMsgData;
				rcp_cbus_uevent(*eventParameter);
				pr_info("Key Code:%x\n",
						(int)mhlTxConfig.mscMsgData);
			} else {
				pr_info("Key Code Error:%x\n",
						(int)mhlTxConfig.mscMsgData);
				mhlTxConfig.mscSaveRcpKeyCode =
							 mhlTxConfig.mscMsgData;
				sii_mhltx_rcpe_send(0x01);
			}
			break;
		case	MHL_MSC_MSG_RCPK:
			*event = MHL_TX_EVENT_RCPK_RECEIVED;
			*eventParameter = mhlTxConfig.mscMsgData;
			break;
		case	MHL_MSC_MSG_RCPE:
			*event = MHL_TX_EVENT_RCPE_RECEIVED;
			*eventParameter = mhlTxConfig.mscMsgData;
			break;
		case	MHL_MSC_MSG_RAPK:
			/*Do nothing if RAPK comes*/
			break;
		default:
			/*Any freak value here would continue with
			no event to app*/
			break;
		}

	}
}
/*
* mhltx_drive_states

* This is an internal function to move the MSC engine to do the next thing
* before allowing the application to run RCP APIs.

* It is called in interrupt context to meet some MHL specified timings,
* therefore, It should not have to call app layer and do negligible
* processing, no pr_infos.
*/
static	void	mhltx_drive_states(void)
{
	switch (mhlTxConfig.mscState) {
	case MSC_STATE_BEGIN:
		pr_info("MSC_STATE_BEGIN\n");
		sii_mhltx_read_devcap(MHL_DEV_CATEGORY_OFFSET);
		break;
	case MSC_STATE_POW_DONE:
		/* Send out Read Devcap for MHL_DEV_FEATURE_FLAG_OFFSET
		to check if it supports RCP/RAP etc*/
		pr_info("MSC_STATE_POW_DONE\n");
		sii_mhltx_read_devcap(MHL_DEV_FEATURE_FLAG_OFFSET);
		break;
	case MSC_STATE_IDLE:
	case MSC_STATE_RCP_READY:
		break;
	default:
		break;

	}
}
/*
* SiiMhlTxMscCommandDone

* This function is called by the driver to inform of completion of last command.
* It is called in interrupt context to meet some MHL specified timings,
* therefore, it should not have to call app layer and do negligible processing,
* no pr_infos.
*/
void	sii_mhltx_msc_command_done(byte data1)
{
	TX_DEBUG_PRINT(("MhlTx:sii_mhltx_msc_command_done. data1 = %02X\n",
							 (int) data1));

	if ((MHL_READ_DEVCAP == mhlTxConfig.mscLastCommand) &&
			(MHL_DEV_CATEGORY_OFFSET ==
				 mhlTxConfig.mscLastOffset)) {
		/* We are done reading POW. Next we read Feature Flag*/
		mhlTxConfig.mscState	= MSC_STATE_POW_DONE;

		app_vbus_control((bool)(data1 & MHL_DEV_CATEGORY_POW_BIT));

		/* Send out Read Devcap for MHL_DEV_FEATURE_FLAG_OFFSET
		to check if it supports RCP/RAP etc */
	} else if ((MHL_READ_DEVCAP == mhlTxConfig.mscLastCommand) &&
				(MHL_DEV_FEATURE_FLAG_OFFSET ==
						 mhlTxConfig.mscLastOffset)) {
		/* We are done reading Feature Flag. Let app know we are done.*/
		mhlTxConfig.mscState	= MSC_STATE_RCP_READY;

		/* Remember features of the peer*/
		mhlTxConfig.mscFeatureFlag	= data1;

		/* Now we can entertain App commands for RCP
		 Let app know this state*/
		mhlTxConfig.mhlConnectionEvent = TRUE;
		mhlTxConfig.mhlConnected = MHL_TX_EVENT_RCP_READY;

		/*These variables are used to remember if we issued
		 a READ_DEVCAP  Since we are done, reset them.*/
		mhlTxConfig.mscLastCommand = 0;
		mhlTxConfig.mscLastOffset  = 0;

		TX_DEBUG_PRINT(("MhlTx: Peer's Feature Flag = %02X\n\n",
					 (int) data1));
	} else if (MHL_MSC_MSG_RCPE == mhlTxConfig.mscMsgLastCommand) {
		/*RCPE is always followed by an RCPK with original key code
		 that came.*/
		if (sii_mhltx_rcpk_send(mhlTxConfig.mscSaveRcpKeyCode)) {
			/* Once the command has been sent out successfully,
			forget this case.*/
			mhlTxConfig.mscMsgLastCommand = 0;
			mhlTxConfig.mscMsgLastData    = 0;
		}
	}
}
/***********************************************************************
* SiiMhlTxGotMhlMscMsg

* This function is called by the driver to inform of arrival of a MHL MSC_MSG
* such as RCP, RCPK, RCPE. To quickly return back to interrupt, this function
* remembers the event (to be picked up by app later in task context).

* It is called in interrupt context to meet some MHL specified
* timings,therefore, it should not have to call app layer and

* Application shall not call this function.
*/
void	sii_mhltx_got_mhlmscmsg(byte subCommand, byte cmdData)
{
	/* Remeber the event.*/
	mhlTxConfig.mscMsgArrived		= TRUE;
	mhlTxConfig.mscMsgSubCommand	= subCommand;
	mhlTxConfig.mscMsgData			= cmdData;
}
/*
* sii_mhltx_got_mhlintr

* This function is called by the driver to inform of arrival of
* a MHL INTERRUPT.

* It is called in interrupt context to meet some MHL specified
* timings,therefore, it should not have to call app layer and
* do negligible processing, no printks.
*/
void	sii_mhltx_got_mhlintr(byte intr_0, byte intr_1)
{
	TX_DEBUG_PRINT(("MhlTx: INTERRUPT Arrived. %02X, %02X\n",
					(int) intr_0, (int) intr_1));

	/* Handle DCAP_CHG INTR here*/
	if (MHL_INT_DCAP_CHG & intr_0) {
		sii_mhltx_read_devcap(MHL_DEV_CATEGORY_OFFSET);
	} else if (MHL_INT_EDID_CHG & intr_1) {
		/* force upstream source to read the EDID again.
		 Most likely by appropriate togggling of HDMI HPD*/
		sii_mhltx_drvnotify_edidchange();
	}
}

/*
* sii_mhltx_got_mhlstatus

* This function is called by the driver to inform of arrival of a MHL STATUS.
* It is called in interrupt context to meet some MHL specified
* timings,therefore, Tt should not have to call app layer and
* ligible processing, no printks.
*/
void	sii_mhltx_got_mhlstatus(byte status_0, byte status_1)
{
	TX_DEBUG_PRINT(("MhlTx: STATUS Arrived. %02X, %02X\n",
				(int) status_0, (int) status_1));
	/* Handle DCAP_RDY STATUS here itself*/
	if (MHL_STATUS_DCAP_RDY & status_0) {
		/*		mhltx_drive_states();
			sii_mhltx_read_devcap( MHL_DEV_CATEGORY_OFFSET );*/
		mhlTxConfig.mscState	 = MSC_STATE_BEGIN;
	}
	/* status_1 has the PATH_EN etc. Not yet implemented
	 Remeber the event.*/
	mhlTxConfig.status_0 = status_0;
	mhlTxConfig.status_1 = status_1;
}

/*
* SiiMhlTxRcpSend

* This function checks if the peer device supports RCP and
* sends rcpKeyCode. The function will return a value of TRUE
* if it could successfully send the RCP subcommand and the key code.
* Otherwise FALSE.

* The followings are not yet utilized.
* (MHL_FEATURE_RAP_SUPPORT & mhlTxConfig.mscFeatureFlag)
* (MHL_FEATURE_SP_SUPPORT & mhlTxConfig.mscFeatureFlag)
*/
bool sii_mhltx_rcp_send(byte rcpKeyCode)
{
	/* If peer does not support do not send RCP or RCPK/RCPE commands*/
	if ((0 == (MHL_FEATURE_RCP_SUPPORT & mhlTxConfig.mscFeatureFlag)) ||
		(MSC_STATE_RCP_READY != mhlTxConfig.mscState)) {
		return	FALSE;
	}
	return	mhltx_send_msc_msg(MHL_MSC_MSG_RCP, rcpKeyCode);
}

/*
* sii_mhltx_rcpk_send

* This function sends RCPK to the peer device.
*/
bool sii_mhltx_rcpk_send(byte rcpKeyCode)
{
	return	mhltx_send_msc_msg(MHL_MSC_MSG_RCPK, rcpKeyCode);
}

/*
* SiiMhlTxRapkSend
* This function sends RAPK to the peer device.
*/
static	bool sii_mhltx_rapk_send(void)
{
	return	mhltx_send_msc_msg(MHL_MSC_MSG_RAPK, 0);
}

/*
* SiiMhlTxRcpeSend

* The function will return a value of true if it could
* successfully send the RCPE subcommand. Otherwise false.

* When successful, MhlTx internally sends RCPK with original (last known)
* keycode.
*/

bool sii_mhltx_rcpe_send(byte rcpeErrorCode)
{
	return mhltx_send_msc_msg(MHL_MSC_MSG_RCPE, rcpeErrorCode);
}

/*
* sii_mhltx_read_devcap

* This function sends a READ DEVCAP MHL command to the peer.
* It  returns TRUE if successful in doing so.

* The value of devcap should be obtained by making a call to SiiMhlTxGetEvents()
* offset	Which byte in devcap register is required to be read. 0..0x0E
*/
bool sii_mhltx_read_devcap(byte offset)
{
	struct cbus_req_t	req;
	/* Send MHL_READ_DEVCAP command*/
	req.command     = mhlTxConfig.mscLastCommand = MHL_READ_DEVCAP;
	req.offsetData  = mhlTxConfig.mscLastOffset  = offset;
	return sii_mhltx_drvsend_cbuscommand(&req);
}

/*
* mhltx_send_msc_msg

* This function sends a MSC_MSG command to the peer.
* It  returns TRUE if successful in doing so.
* The value of devcap should be obtained by making a call
* to SiiMhlTxGetEvents()
* offset	Which byte in devcap register is required to be read. 0..0x0E
*/
static bool mhltx_send_msc_msg(byte command, byte cmdData)
{
	struct cbus_req_t	req;
	byte		ccode;

	/* Send MSC_MSG command*/
	req.command     = MHL_MSC_MSG;
	req.msgData[0]  = mhlTxConfig.mscMsgLastCommand = command;
	req.msgData[1]  = mhlTxConfig.mscMsgLastData    = cmdData;

	ccode = sii_mhltx_drvsend_cbuscommand(&req);
	return (bool)ccode;
}
/*
sii_mhltx_notify_connection
*/
void	sii_mhltx_notify_connection(bool mhlConnected)
{
	/*pr_info("sii_mhltx_notify_connection %01X\n", (int) mhlConnected );*/
	mhlTxConfig.mhlConnectionEvent = TRUE;
	mhlTxConfig.mscState	 = MSC_STATE_IDLE;

	if (mhlConnected)
		mhlTxConfig.mhlConnected = MHL_TX_EVENT_CONNECTION;
	else
		mhlTxConfig.mhlConnected = MHL_TX_EVENT_DISCONNECTION;
}
/*
* sii_mhltx_notify_dshpd_change
* Driver tells about arrival of SET_HPD or CLEAR_HPD by calling this function.
*
* Turn the content off or on based on what we got.
*/
void	sii_mhltx_notify_dshpd_change(byte dsHpdStatus)
{
	if (0 == dsHpdStatus) {
		TX_DEBUG_PRINT(("MhlTx: Disable TMDS\n"));
		sii_mhltx_drv_tmds_control(FALSE);
	} else {
		TX_DEBUG_PRINT(("MhlTx: Enable TMDS\n"));
		sii_mhltx_drv_tmds_control(TRUE);
	}
}
/*
 mhltx_reset_states

 Application picks up mhl connection and rcp events at periodic intervals.
 Interrupt handler feeds these variables. Reset them on disconnection.
*/
static void	mhltx_reset_states(void)
{
	mhlTxConfig.mhlConnectionEvent	= FALSE;
	mhlTxConfig.mhlConnected		= MHL_TX_EVENT_DISCONNECTION;
	mhlTxConfig.mscMsgArrived		= FALSE;
	mhlTxConfig.mscState			= MSC_STATE_IDLE;
}

#define	APP_DEMO_RCP_SEND_KEY_CODE 0x44

/*
* app_rcp_demo

* This function is supposed to provide a demo code to elicit how to call RCP
* API function.
*/
void	app_rcp_demo(byte event, byte eventParameter)
{
	byte	rcpKeyCode;
	/*pr_info("App: Got event = %02X, eventParameter = %02X\n",
					(int)event, (int)eventParameter);*/
	switch (event) {
	case	MHL_TX_EVENT_DISCONNECTION:
		pr_info("App:Got event=MHL_TX_EVENT_DISCONNECTION\n");
		break;
	case	MHL_TX_EVENT_CONNECTION:
		pr_info("App:Got event=MHL_TX_EVENT_CONNECTION\n");
		break;
	case	MHL_TX_EVENT_RCP_READY:
		/* Demo RCP key code PLAY*/
		rcpKeyCode = APP_DEMO_RCP_SEND_KEY_CODE;
		pr_info("App: Got event = MHL_TX_EVENT_RCP_READY..."
				"Sending RCP (%02X)\n", (int) rcpKeyCode);
		break;
	case	MHL_TX_EVENT_RCP_RECEIVED:
		/* Check if we got an RCP. Application can perform
		the operation here and send RCPK or RCPE. For now,
		we send the RCPK*/
		pr_info("App: Received an RCP key code = %02X\n",
							 eventParameter);
		switch (eventParameter) {
		case MHD_RCP_CMD_SELECT:
			TX_DEBUG_PRINT(("\nSelect received\n\n"));
			break;
		case MHD_RCP_CMD_UP:
			TX_DEBUG_PRINT(("\nUp received\n\n"));
			break;
		case MHD_RCP_CMD_DOWN:
			TX_DEBUG_PRINT(("\nDown received\n\n"));
			break;
		case MHD_RCP_CMD_LEFT:
			TX_DEBUG_PRINT(("\nLeft received\n\n"));
			break;
		case MHD_RCP_CMD_RIGHT:
			TX_DEBUG_PRINT(("\nRight received\n\n"));
			break;
		case MHD_RCP_CMD_RIGHT_UP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_RIGHT_UP\n\n"));
			break;
		case MHD_RCP_CMD_RIGHT_DOWN:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_RIGHT_DOWN\n\n"));
			break;
		case MHD_RCP_CMD_LEFT_UP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_LEFT_UP\n\n"));
			break;
		case MHD_RCP_CMD_LEFT_DOWN:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_LEFT_DOWN\n\n"));
			break;
		case MHD_RCP_CMD_ROOT_MENU:
			TX_DEBUG_PRINT(("\nRoot Menu received\n\n"));
			break;
		case MHD_RCP_CMD_SETUP_MENU:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_SETUP_MENU\n\n"));
			break;
		case MHD_RCP_CMD_CONTENTS_MENU:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_CONTENTS_MENU\n\n"));
			break;
		case MHD_RCP_CMD_FAVORITE_MENU:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_FAVORITE_MENU\n\n"));
			break;
		case MHD_RCP_CMD_EXIT:
			TX_DEBUG_PRINT(("\nExit received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_0:
			TX_DEBUG_PRINT(("\nNumber 0 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_1:
			TX_DEBUG_PRINT(("\nNumber 1 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_2:
			TX_DEBUG_PRINT(("\nNumber 2 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_3:
			TX_DEBUG_PRINT(("\nNumber 3 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_4:
			TX_DEBUG_PRINT(("\nNumber 4 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_5:
			TX_DEBUG_PRINT(("\nNumber 5 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_6:
			TX_DEBUG_PRINT(("\nNumber 6 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_7:
			TX_DEBUG_PRINT(("\nNumber 7 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_8:
			TX_DEBUG_PRINT(("\nNumber 8 received\n\n"));
			break;
		case MHD_RCP_CMD_NUM_9:
			TX_DEBUG_PRINT(("\nNumber 9 received\n\n"));
			break;
		case MHD_RCP_CMD_DOT:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_DOT\n\n"));
			break;
		case MHD_RCP_CMD_ENTER:
			TX_DEBUG_PRINT(("\nEnter received\n\n"));
			break;
		case MHD_RCP_CMD_CLEAR:
			TX_DEBUG_PRINT(("\nClear received\n\n"));
			break;
		case MHD_RCP_CMD_CH_UP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_CH_UP\n\n"));
			break;
		case MHD_RCP_CMD_CH_DOWN:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_CH_DOWN\n\n"));
			break;
		case MHD_RCP_CMD_PRE_CH:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_PRE_CH\n\n"));
			break;
		case MHD_RCP_CMD_SOUND_SELECT:
			TX_DEBUG_PRINT(("\nSound Select received\n\n"));
			break;
		case MHD_RCP_CMD_INPUT_SELECT:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_INPUT_SELECT\n\n"));
			break;
		case MHD_RCP_CMD_SHOW_INFO:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_SHOW_INFO\n\n"));
			break;
		case MHD_RCP_CMD_HELP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_HELP\n\n"));
			break;
		case MHD_RCP_CMD_PAGE_UP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_PAGE_UP\n\n"));
			break;
		case MHD_RCP_CMD_PAGE_DOWN:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_PAGE_DOWN\n\n"));
			break;
		case MHD_RCP_CMD_VOL_UP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_VOL_UP\n\n"));
			break;
		case MHD_RCP_CMD_VOL_DOWN:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_VOL_DOWN\n\n"));
			break;
		case MHD_RCP_CMD_MUTE:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_MUTE\n\n"));
			break;
		case MHD_RCP_CMD_PLAY:
			TX_DEBUG_PRINT(("\nPlay received\n\n"));
			break;
		case MHD_RCP_CMD_STOP:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_STOP\n\n"));
			break;
		case MHD_RCP_CMD_PAUSE:
			TX_DEBUG_PRINT(("\nPause received\n\n"));
			break;
		case MHD_RCP_CMD_RECORD:
			TX_DEBUG_PRINT(("\n MHD_RCP_CMD_RECORD\n\n"));
			break;
		case MHD_RCP_CMD_FAST_FWD:
			TX_DEBUG_PRINT(("\nFastfwd received\n\n"));
			break;
		case MHD_RCP_CMD_REWIND:
			TX_DEBUG_PRINT(("\nRewind received\n\n"));
			break;
		case MHD_RCP_CMD_EJECT:
			TX_DEBUG_PRINT(("\nEject received\n\n"));
			break;
		case MHD_RCP_CMD_FWD:
			TX_DEBUG_PRINT(("\nForward received\n\n"));
			break;
		case MHD_RCP_CMD_BKWD:
			TX_DEBUG_PRINT(("\nBackward received\n\n"));
			break;
		case MHD_RCP_CMD_PLAY_FUNC:
			TX_DEBUG_PRINT(("\nPlay Function received\n\n"));
			break;
		case MHD_RCP_CMD_PAUSE_PLAY_FUNC:
			TX_DEBUG_PRINT(("\nPause_Play Function received\n\n"));
			break;
		case MHD_RCP_CMD_STOP_FUNC:
			TX_DEBUG_PRINT(("\nStop Function received\n\n"));
			break;
		default:
			break;
		}
		rcpKeyCode = eventParameter;
		sii_mhltx_rcpk_send(rcpKeyCode);
		break;

	case	MHL_TX_EVENT_RCPK_RECEIVED:
		pr_info("App: Received an RCPK\n");
		break;
	case	MHL_TX_EVENT_RCPE_RECEIVED:
		pr_info("App: Received an RCPE\n");
		break;
	default:
		break;
	}
}
/*
* app_vbus_control

* This function or macro is invoked from MhlTx driver to ask application to
* control the VBUS power. If powerOn is sent as non-zero, one should assume
* peer does not need power so quickly remove VBUS power.
* if value of "powerOn" is 0, then application must turn the VBUS power on
* within 50ms of this call to meet MHL specs timing.

* Application module must provide this function.
*/
void	app_vbus_control(bool powerOn)
{
	if (powerOn)
		pr_info("App: Peer's POW bit is set. "
			"Turn the VBUS power OFF here.\n");
	else
		pr_info("App: Peer's POW bit is cleared. "
			"Turn the VBUS power ON here.\n");
}


/*
*  FUNCTION sii9234_interrupt_event

*  DESCRIPTION
*  When sii9234 H/W interrupt happen, call this event function

*  DEPENDENCIES
*  None

*  RETURN VALUE
*  None

*  SIDE EFFECTS
*  None
*/
void sii9234_interrupt_event(void)
{
	byte	event;
	byte	eventParameter;
	byte	flag;

	do {
		/*
		 Look for any events that might have occurred.
		*/
		flag = 0;
		sii_mhltx_get_events(&event, &eventParameter);

		if (MHL_TX_EVENT_NONE != event)
			app_rcp_demo(event, eventParameter);

		if (mhl_cable_status == MHL_TV_OFF_CABLE_CONNECT) {
			byte tmp;
			tmp = i2c_read_byte(SA_TX_Page0_Primary, (0x74));
			i2c_write_byte(SA_TX_Page0_Primary, (0x74), tmp);
			tmp = i2c_read_byte(SA_TX_Page0_Primary, 0x71);
			i2c_write_byte(SA_TX_Page0_Primary, 0x71, tmp);

			tmp = read_byte_cbus(0x08);
			write_byte_cbus(0x08, tmp);

			tmp = read_byte_cbus(0x1E);
			write_byte_cbus(0x1E, tmp);
			TX_DEBUG_PRINT(("mhl_cable_status =="
					"MHL_TV_OFF_CABLE_CONNECT\n"));
			return ;

		} else if (((fwPowerState == TX_POWER_STATE_D0_MHL) ||
				(fwPowerState == TX_POWER_STATE_D0_NO_MHL))
				 && mhl_cable_status) {
			byte tmp;
			tmp = i2c_read_byte(SA_TX_Page0_Primary, (0x74));
			flag |= (tmp&INTR_4_DESIRED_MASK);
			pr_info("#1 (0x74) flag: %x\n", (int) flag);

			tmp = i2c_read_byte(SA_TX_Page0_Primary, 0x71);
			flag |= (tmp&INTR_1_DESIRED_MASK);
			pr_info("#1 (0x71) flag: %x\n", (int) flag);

			if (mhlTxConfig.mhlConnected ==
					 MHL_TX_EVENT_DISCONNECTION) {
				tmp = read_byte_cbus(0x08);
				pr_info("#2 (read_byte_cbus(0x08)) Temp:"
							"%x\n", (int) tmp);
				write_byte_cbus(0x08, tmp);
				tmp = read_byte_cbus(0x1E);
				pr_info("#2 (read_byte_cbus(0x1E)) Temp:"
							"%x\n", (int) tmp);
				write_byte_cbus(0x1E, tmp);
			} else {
				tmp = read_byte_cbus(0x08);
				flag |= (tmp&INTR_CBUS1_DESIRED_MASK);
				pr_info("#1 (read_byte_cbus(0x08)) Temp:"
							" %x\n", (int) flag);
				tmp = read_byte_cbus(0x1E);
				flag |= (tmp&INTR_CBUS2_DESIRED_MASK);
				pr_info("#1 (read_byte_cbus(0x1E)) Temp:"
							 "%x\n", (int) flag);
			}
			if ((flag == 0xFA) || (flag == 0xFF))
				flag = 0;
		}
	} while (flag);
	pr_info("#$$$$$$$$$$$$$$$ flag: %x\n", (int) flag);
}
EXPORT_SYMBOL(sii9234_interrupt_event);

