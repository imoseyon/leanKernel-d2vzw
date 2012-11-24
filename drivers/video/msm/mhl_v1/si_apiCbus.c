/***************************************************************************
*
*  si_apiCbus.c
*
*  CBUS API
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

#include <linux/delay.h>
#include <linux/fcntl.h>
#include <linux/freezer.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include "Common_Def.h"
#include "si_apiCbus.h"
#include "si_RegioCbus.h"
#include "si_cbusDefs.h"


#define MHD_DEVICE_CATEGORY             (MHD_DEV_CAT_SOURCE)
#define	MHD_LOGICAL_DEVICE_MAP	(MHD_DEV_LD_AUDIO | MHD_DEV_LD_VIDEO\
					| MHD_DEV_LD_MEDIA | MHD_DEV_LD_GUI)
#define CONFIG_CHECK_DDC_ABORT		0
#define CBUS_FW_INTR_POLL_MILLISECS     50
			/* wait this to issue next status poll i2c read*/


static cbusChannelState_t l_cbus[MHD_MAX_CHANNELS];

Bool dev_cap_regs_ready_bit;


/*------------------------------------------------------------------------------

	Function:    SI_CbusRequestStatus
	Description: Return the status of the message currently in process,
			if any.
	Parameters:  channel - CBUS channel to check
	Returns:     CBUS_REQ_IDLE, CBUS_REQ_PENDING, CBUS_REQ_SENT,
			or CBUS_REQ_RECEIVED

------------------------------------------------------------------------------*/

byte si_cbus_request_status(byte channel)
{
	return l_cbus[channel].request[l_cbus[channel].activeIndex].reqStatus;
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusRequestSetIdle
	Description: Set the active request to the specified state
	Parameters:  channel - CBUS channel to set

------------------------------------------------------------------------------*/


void si_cbus_request_set_idle(byte channel, byte newState)
{

	l_cbus[channel].request[l_cbus[channel].activeIndex].reqStatus = newState;
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusRequestData
	Description: Return a pointer to the currently active request structure
	Parameters:  channel - CBUS channel.
	Returns:     Pointer to a cbus_req_t structure.

------------------------------------------------------------------------------*/

cbus_req_t *si_cbus_request_data(byte channel)
{
	return &l_cbus[channel].request[l_cbus[channel].activeIndex];
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusChannelConnected
	Description: Return the CBUS channel connected status for this channel.
	Returns:        TRUE if connected.
			FALSE if disconnected.

------------------------------------------------------------------------------*/

Bool si_cbus_channel_connected(byte channel)
{
	return l_cbus[channel].connected;
}

/*******************************************************************************

	FUNCTION         : cbus_display_registers
	PURPOSE          : For debugging purposes, show salient register on UART
	INPUT PARAMETERS : startFrom	= First register to show
			howmany		= number of registers to show.
			This will be rounded to next multiple of 4.
	OUTPUT PARAMETERS:   None.
	RETURNED VALUES  :   None
	GLOBALS USED     :

*******************************************************************************/

void cbus_display_registers(int startfrom, int howmany)
{
	int	regnum, regval, i;
	int end_at;

	end_at = startfrom + howmany;

	for (regnum = startfrom; regnum <= end_at;) {
		for (i = 0 ; i <= 7; i++, regnum++)
			regval = sii_regio_cub_read(regnum, 0);
	}
}

/*------------------------------------------------------------------------------

	Function:    CBusProcessConnectionChange
	Description: Process a connection change interrupt
	Returns:

------------------------------------------------------------------------------*/

static byte cbus_process_connection_change(int channel)
{
	channel = 0;
	return ERROR_CBUS_TIMEOUT;
}

/*------------------------------------------------------------------------------

	Function:    CBusProcessFailureInterrupts
	Description: Check for and process any failure interrupts.
	Returns:     SUCCESS or ERROR_CBUS_ABORT

------------------------------------------------------------------------------*/

static byte cbus_process_failure_interrupts(byte channel,
					byte intStatus, byte inResult)
{

	byte result          = inResult;
	byte mscAbortReason  = STATUS_SUCCESS;
	byte ddcAbortReason  = STATUS_SUCCESS;
/* At this point, we only need to look at the abort interrupts.*/
	intStatus &=  /*BIT_DDC_ABORT |*/ BIT_MSC_ABORT | BIT_MSC_XFR_ABORT;

	if (intStatus) {
		result = ERROR_CBUS_ABORT;	/* No Retry will help*/

/* If transfer abort or MSC abort, clear the abort reason register. */
		if (intStatus & BIT_CONNECT_CHG)
			pr_info("CBUS Connection Change Detected\n");


		if (intStatus & BIT_DDC_ABORT) {
			ddcAbortReason = sii_regio_cub_read(REG_DDC_ABORT_REASON, channel);
			pr_info("CBUS DDC ABORT happened, reason:: %02X\n",
							(int)(ddcAbortReason));
		}

		if (intStatus & BIT_MSC_XFR_ABORT) {
			mscAbortReason = sii_regio_cub_read(REG_PRI_XFR_ABORT_REASON, channel);
			pr_info("CBUS:: MSC Transfer ABORTED. Clearing 0x0D\n");
			sii_regio_cbus_write(REG_PRI_XFR_ABORT_REASON, channel, 0xFF);
		}

		if (intStatus & BIT_MSC_ABORT) {
			pr_info("CBUS:: MSC Peer sent an ABORT. Clearing 0x0E\n");
			sii_regio_cbus_write(REG_CBUS_PRI_FWR_ABORT_REASON, channel, 0xFF);
		}

/* Now display the abort reason.*/
		if (mscAbortReason != 0) {
			pr_info("CBUS:: Reason for ABORT is ....0x%02X = ",
							(int)mscAbortReason);
			if (mscAbortReason & CBUSABORT_BIT_REQ_MAXFAIL)
				pr_info("Requestor MAXFAIL - retry threshold exceeded\n");

			if (mscAbortReason & CBUSABORT_BIT_PROTOCOL_ERROR)
				pr_info("Protocol Error\n");

			if (mscAbortReason & CBUSABORT_BIT_REQ_TIMEOUT)
				pr_info("Requestor translation layer timeout\n");

			if (mscAbortReason & CBUSABORT_BIT_PEER_ABORTED)
				pr_info("Peer sent an abort\n");

			if (mscAbortReason & CBUSABORT_BIT_UNDEFINED_OPCODE)
				pr_info("Undefined opcode\n");

		}

	}


/* Clear any failure interrupt that we received.    */
	sii_regio_cbus_write(REG_CBUS_INTR_STATUS, channel, intStatus);

	return result;
}



/*------------------------------------------------------------------------------

	Function:    CBusProcessSubCommand
	Description: Process a sub-command (RCP) or sub-command response (RCPK).
		Modifies channel state as necessary.
	Parameters:  channel - CBUS channel that received the command.
	Returns:     SUCCESS or CBUS_SOFTWARE_ERRORS_t
	If SUCCESS, command data is returned in l_cbus[channel].msgData[i]

------------------------------------------------------------------------------*/



static byte cbus_process_subcommand(int channel, byte vs_cmd, byte vs_data)
{
/*Save RCP message data in the channel request structure to be returned */
/* to the upper level.                                                */
	l_cbus[channel].request[l_cbus[channel].activeIndex].command = vs_cmd;
	l_cbus[channel].request[l_cbus[channel].activeIndex].offsetData = vs_data;
	/* Parse it a little before telling the upper level about it.   */
	switch (vs_cmd) {
	case MHD_MSC_MSG_RCP:
	/* Received a Remote Control Protocol message.  Signal that */
	/* it has been received if we are in the right state,       */
	/* otherwise, it is a bogus message.  Don't send RCPK now   */
	/* because the upper layer must validate the command.       */

		pr_info("CBUS:: Received <-- MHD_MSC_MSG_RCP:: cbus state = %02X\n",
					(int)(l_cbus[channel].state));

		switch (l_cbus[channel].state) {
		case CBUS_IDLE:
		case CBUS_SENT:
			l_cbus[channel].request[l_cbus[channel].activeIndex].reqStatus
				= CBUS_REQ_RECEIVED;
			l_cbus[channel].state = CBUS_RECEIVED;
			break;
		default:
			l_cbus[channel].state = CBUS_IDLE;
			break;
		}
		break;
	case MHD_MSC_MSG_RCPK:
		pr_info("CBUS:: Received <-- MHD_MSC_MSG_RCPK\n");
		break;
	case MHD_MSC_MSG_RCPE:
		pr_info("CBUS:: Received <-- MHD_MSC_MSG_RCPE\n");
		break;
	case MHD_MSC_MSG_RAP:
		pr_info("CBUS:: Received <-- MHD_MSC_MSG_RAP:: cbus state = %02X\n",
						(int)(l_cbus[channel].state));

		switch (l_cbus[channel].state) {
		case CBUS_IDLE:
		case CBUS_SENT:
			l_cbus[channel].request[l_cbus[channel].activeIndex].reqStatus
				= CBUS_REQ_RECEIVED;
			l_cbus[channel].state = CBUS_RECEIVED;
			break;
		default:
			l_cbus[channel].state = CBUS_IDLE;
			break;
		}
		break;
	case MHD_MSC_MSG_RAPK:
		pr_info("CBUS:: Received <-- MHD_MSC_MSG_RAPK\n");
		break;
#if MSC_TESTER
	case MHD_MSC_MSG_NEW_TESTER:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_NEW_TESTER_REPLY, 0x00);
		break;
	case MHD_MSC_MSG_NEW_TESTER_REPLY:
		msc_return_cmd = vs_cmd;
		break;
	case MHD_MSC_MSG_STATE_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_0, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_STATE_CHANGE_REPLY, 0x00);
		break;
	case MHD_MSC_MSG_DEVCAP0_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_0, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_0, channel);
		break;
	case MHD_MSC_MSG_DEVCAP1_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_1, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_1, channel);
		break;
	case MHD_MSC_MSG_DEVCAP2_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_2, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_2, channel);
		break;
	case MHD_MSC_MSG_DEVCAP3_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_3, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_3, channel);
		break;
	case MHD_MSC_MSG_DEVCAP4_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_4, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_4, channel);
		break;
	case MHD_MSC_MSG_DEVCAP5_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_5, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_5, channel);
		break;
	case MHD_MSC_MSG_DEVCAP6_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_6, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_6, channel);
		break;
	case MHD_MSC_MSG_DEVCAP7_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_7, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_7, channel);
		break;
	case MHD_MSC_MSG_DEVCAP8_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_8, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_8, channel);
		break;
	case MHD_MSC_MSG_DEVCAP9_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_9, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_9, channel);
		break;
	case MHD_MSC_MSG_DEVCAP10_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_A, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_A, channel);
		break;
	case MHD_MSC_MSG_DEVCAP11_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_B, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_B, channel);
		break;
	case MHD_MSC_MSG_DEVCAP12_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_C, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_C, channel);

		break;
	case MHD_MSC_MSG_DEVCAP13_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_D, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_D, channel);
		break;
	case MHD_MSC_MSG_DEVCAP14_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_E, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_E, channel);
		break;
	case MHD_MSC_MSG_DEVCAP15_CHANGE:
		sii_regio_cbus_write(REG_CBUS_DEVICE_CAP_F, channel, vs_data);
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_DEVCAP_CHANGE_REPLY,
			sii_regio_cub_read(REG_CBUS_DEVICE_CAP_F, channel);
		break;
	case MHD_MSC_MSG_SET_INT0_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_SET_INT_REPLY,
				sii_regio_cub_read(REG_CBUS_SET_INT_0, channel);
		break;
	case MHD_MSC_MSG_SET_INT1_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_SET_INT_REPLY,
				sii_regio_cub_read(REG_CBUS_SET_INT_1, channel);
		break;
	case MHD_MSC_MSG_SET_INT2_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_SET_INT_REPLY,
				sii_regio_cub_read(REG_CBUS_SET_INT_2, channel);
		break;
	case MHD_MSC_MSG_SET_INT3_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_SET_INT_REPLY,
				sii_regio_cub_read(REG_CBUS_SET_INT_3, channel);
		break;
	case MHD_MSC_MSG_WRITE_STAT0_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_WRITE_STAT_REPLY,
			sii_regio_cub_read(REG_CBUS_WRITE_STAT_0, channel);
		break;
	case MHD_MSC_MSG_WRITE_STAT1_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_WRITE_STAT_REPLY,
			sii_regio_cub_read(REG_CBUS_WRITE_STAT_1, channel);
		break;
	case MHD_MSC_MSG_WRITE_STAT2_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_WRITE_STAT_REPLY,
			sii_regio_cub_read(REG_CBUS_WRITE_STAT_2, channel);
		break;
	case MHD_MSC_MSG_WRITE_STAT3_CHECK:
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_WRITE_STAT_REPLY,
			sii_regio_cub_read(REG_CBUS_WRITE_STAT_3, channel);
		break;
	case MHD_MSC_MSG_STATE_CHANGE_REPLY:
		msc_return_cmd = vs_cmd;
		break;
	case MHD_MSC_MSG_DEVCAP_CHANGE_REPLY:
		msc_return_cmd = vs_cmd;
		break;
	case MHD_MSC_MSG_SET_INT_REPLY:
		msc_return_value = vs_data;
		msc_return_cmd = vs_cmd;
		break;
	case MHD_MSC_MSG_WRITE_STAT_REPLY:
		msc_return_value = vs_data;
		msc_return_cmd = vs_cmd;
		break;
#endif
	default:
		break;
	}

	pr_info("CBUS:: MSG_MSC CMD:  0x%02X\n", (int)vs_cmd);
	pr_info("CBUS:: MSG_MSC Data: 0x%02X\n\n", (int)vs_data);

	return STATUS_SUCCESS;
}

#if MSC_TESTER
void si_cbus_mscreset_return_cmd()
{
	msc_return_cmd = 0;
}

void si_cbus_mscreset_return_value()
{
	msc_return_value = 0;
}

byte si_cbus_mscreturn_cmd()
{
	return msc_return_cmd;
}

byte si_cbus_mscreturn_value()
{

	return msc_return_value;

}
#endif /* MSC_TESTER*/

/*------------------------------------------------------------------------------

Function:    CBusWriteCommand
Description: Write the specified Sideband Channel command to the CBUS.
	Command can be a MSC_MSG command (RCP/MCW/RAP), or another command
	such as READ_DEVCAP, GET_VENDOR_ID, SET_HPD, CLR_HPD, etc.
Parameters:     channel - CBUS channel to write
		pReq    - Pointer to a cbus_req_t structure containing the
		command to write
Returns:        TRUE    - successful write
		FALSE   - write failed

------------------------------------------------------------------------------*/


static Bool cbus_write_command(int channel, cbus_req_t *pReq)
{

	byte i, startbit;
	Bool  success = TRUE;

	pr_info("CBUS:: Sending MSC command %02X, %02X, %02X\n",
		(int)pReq->command, (int)pReq->msgData[0], (int)pReq->msgData[1]);

	/******************************************************************/
	/* Setup for the command - write appropriate registers and        */
	/*			determine the correct start bit.          */
	/******************************************************************/

	/* Set the offset and outgoing data byte right away*/
	sii_regio_cbus_write(REG_CBUS_PRI_ADDR_CMD, channel, pReq->offsetData); /* set offset*/
	sii_regio_cbus_write(REG_CBUS_PRI_WR_DATA_1ST, channel, pReq->msgData[0]);

	startbit = 0x00;

	switch (pReq->command) {
	case MHD_SET_INT:	/* Set one interrupt register = 0x60 */
		sii_regio_cbus_write(REG_CBUS_PRI_ADDR_CMD, channel, pReq->offsetData + 0x20);/* set offset*/
		startbit = MSC_START_BIT_WRITE_REG;
		break;
	case MHD_WRITE_STAT: /* Write one status register = 0x60 | 0x80*/
		sii_regio_cbus_write(REG_CBUS_PRI_ADDR_CMD, channel, pReq->offsetData + 0x30);/*set offset*/
		startbit = MSC_START_BIT_WRITE_REG;
		break;
	case MHD_READ_DEVCAP:  /* Read one device capability register = 0x61*/
		startbit = MSC_START_BIT_READ_REG;
		break;
	case MHD_GET_STATE:	/*0x62 - Used for heartbeat*/
	case MHD_GET_VENDOR_ID: /* 0x63 - for vendor id	*/
	case MHD_SET_HPD: /* 0x64 - Set Hot Plug Detect in follower*/
	case MHD_CLR_HPD: /* 0x65 - Clear Hot Plug Detect in follower*/
	case MHD_GET_SC1_ERRORCODE: /* 0x69 - Get channel 1 command error code*/
	case MHD_GET_DDC_ERRORCODE: /* 0x6A - Get DDC channel command error code.*/
	case MHD_GET_MSC_ERRORCODE: /* 0x6B - Get MSC command error code.*/
	case MHD_GET_SC3_ERRORCODE: /* 0x6D - Get channel 3 command error code.*/
		sii_regio_cbus_write(REG_CBUS_PRI_ADDR_CMD, channel, pReq->command);
		startbit = MSC_START_BIT_MSC_CMD;
		break;
	case MHD_MSC_MSG:
		sii_regio_cbus_write(REG_CBUS_PRI_WR_DATA_2ND, channel, pReq->msgData[1]);
		sii_regio_cbus_write(REG_CBUS_PRI_ADDR_CMD, channel, pReq->command);
		startbit = MSC_START_BIT_VS_CMD;
		break;
	case MHD_WRITE_BURST:
		sii_regio_cbus_write(REG_CBUS_PRI_ADDR_CMD, channel, pReq->offsetData + 0x40);
		sii_regio_cbus_write(REG_MSC_WRITE_BURST_LEN, channel, pReq->length - 1);
		pr_info("CBUS:: pReq->length: 0x%02X\n\n", (int)pReq->length);

	/* Now copy all bytes from array to local scratchpad*/
		for (i = 0; i < pReq->length; i++)
			sii_regio_cbus_write(REG_CBUS_SCRATCHPAD_0 + i, channel, pReq->msgData[i]);

		startbit = MSC_START_BIT_WRITE_BURST;
		break;
	default:
		success = FALSE;
		break;
	}


	/********************************************************************/
	/* Trigger the CBUS command transfer using the determined start bit.*/
	/********************************************************************/

	if (success)
		sii_regio_cbus_write(REG_CBUS_PRI_START, channel, startbit);

	return success;
}



/*------------------------------------------------------------------------------

Function:    CBusConmmandGetNextInQueue
Description: find out the next command in the queue to be sent
Parameters:  channel - CBUS channel that received the command.
Returns:     the index of the next command

------------------------------------------------------------------------------*/


static byte cbus_command_get_nextinqueue(byte channel)
{

	byte   result = STATUS_SUCCESS;
	byte nextIndex = (l_cbus[channel].activeIndex == (CBUS_MAX_COMMAND_QUEUE - 1)) ?
		 0 : (l_cbus[channel].activeIndex + 1)	;

	while (l_cbus[channel].request[nextIndex].reqStatus != CBUS_REQ_PENDING) {
		if (nextIndex == l_cbus[channel].activeIndex)
					/*searched whole queue, no pending */
			return 0;

	nextIndex = (nextIndex == (CBUS_MAX_COMMAND_QUEUE - 1)) ?
					0 : (nextIndex + 1);

	}

	pr_info("channel:%x nextIndex:%x\n", (int)channel, (int)nextIndex);

	if (cbus_write_command(channel, &l_cbus[channel].request[nextIndex])) {
		l_cbus[channel].request[nextIndex].reqStatus = CBUS_REQ_SENT;
		l_cbus[channel].activeIndex = nextIndex;
		l_cbus[channel].state = CBUS_SENT;
	} else {
		pr_info("CBUS:: cbus_write_command failed\n");
		result = ERROR_WRITE_FAILED;

	}

	return result;
}


/*------------------------------------------------------------------------------

Function:    CBusResetToIdle
Description: Set the specified channel state to IDLE. Clears any messages that
	are in progress or queued.  Usually used if a channel connection
	changed or the channel heartbeat has been lost.
Parameters:  channel - CBUS channel to reset

------------------------------------------------------------------------------*/
static void cbus_reset_to_idle(byte channel)
{

	byte queueIndex;
	l_cbus[channel].state = CBUS_IDLE;

	for (queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE; queueIndex++)
		l_cbus[channel].request[queueIndex].reqStatus = CBUS_REQ_IDLE;

}

/*------------------------------------------------------------------------------

Function:    CBusCheckInterruptStatus
Description: If any interrupts on the specified channel are set, process them.
Parameters:  channel - CBUS channel to check
Returns:     SUCCESS or CBUS_SOFTWARE_ERRORS_t error code.

------------------------------------------------------------------------------*/
static byte cbus_check_interrupt_status(byte channel)
{

	byte	intStatus, result;
	byte	vs_cmd, vs_data;
	byte	writeBurstLen = 0;

	/* Read CBUS interrupt status.  */
	intStatus = sii_regio_cub_read(REG_CBUS_INTR_STATUS, channel);

	if (intStatus & BIT_MSC_MSG_RCV) {
		pr_info("( intStatus & BIT_MSC_MSG_RCV )\n");
		vs_cmd  = sii_regio_cub_read(REG_CBUS_PRI_VS_CMD, channel);
		vs_data = sii_regio_cub_read(REG_CBUS_PRI_VS_DATA, channel);

	}

	sii_regio_cbus_write(REG_CBUS_INTR_STATUS, channel, intStatus);

	/* Check for interrupts.  */
	result = STATUS_SUCCESS;
	intStatus &= (~BIT_HEARTBEAT_TIMEOUT);	 /*don't check heartbeat*/

	if (intStatus != 0) {
		if (intStatus & BIT_CONNECT_CHG) {
			pr_info("( intStatus & BIT_CONNECT_CHG )\n");

	/* The connection change interrupt has been received.   */
			result = cbus_process_connection_change(channel);
			sii_regio_cbus_write(REG_CBUS_BUS_STATUS,
						channel, BIT_CONNECT_CHG);
		}

		if (intStatus & BIT_MSC_XFR_DONE) {
			pr_info("( intStatus & BIT_MSC_XFR_DONE )\n");

	/* A previous MSC sub-command has been acknowledged by the responder.*/
	/* Does not include MSC MSG commands. */
			l_cbus[channel].state = CBUS_XFR_DONE;

	/* Save any response data in the channel
			request structure to be returned*/
	/* to the upper level.*/

	msc_return_cmd = l_cbus[channel].request[l_cbus[channel].activeIndex].msgData[0] =
		sii_regio_cub_read(REG_CBUS_PRI_RD_DATA_1ST, channel);
	msc_return_value = l_cbus[channel].request[l_cbus[channel].activeIndex].msgData[1] =
		sii_regio_cub_read(REG_CBUS_PRI_RD_DATA_2ND, channel);

			pr_info("\nCBUS:: Transfer Done\n");
			pr_info("Response data Received:: %02X\n\n",
		(int)l_cbus[channel].request[l_cbus[channel].activeIndex].msgData[0]);

			result = STATUS_SUCCESS;

	/* Check if we received NACK from Peer*/
			writeBurstLen = sii_regio_cub_read(REG_MSC_WRITE_BURST_LEN, channel);

			if (writeBurstLen & MSC_REQUESTOR_DONE_NACK) {
				result = ERROR_NACK_FROM_PEER;
				pr_info("NACK received!!! :: %02X\n", (int)writeBurstLen) ;
			}

		result = cbus_process_failure_interrupts(channel, intStatus, result);
		}

		if (intStatus & BIT_MSC_MSG_RCV) {
			pr_info("( intStatus & BIT_MSC_MSG_RCV )\n");
	/* Receiving a sub-command, either an actual command or */
	/* the response to a command we sent.                   */
			result = cbus_process_subcommand(channel, vs_cmd, vs_data);

		}
	}
	return result;
}

#if MSC_TESTER
/*------------------------------------------------------------------------------

	Function:    SI_CbusHeartBeat
	Description:  Enable/Disable Heartbeat

------------------------------------------------------------------------------*/
void si_cbus_heartbeat(byte channel, byte enable)
{

	byte  value;
	value = sii_regio_cub_read(REG_MSC_HEARTBEAT_CONTROL, channel);
	sii_regio_cbus_write(REG_MSC_HEARTBEAT_CONTROL, channel, enable ?
			(value | MSC_HEARTBEAT_ENABLE) : (value & 0x7F));

}
#endif /* MSC_TESTER*/

/*------------------------------------------------------------------------------

Function:    SI_CbusMscMsgSubCmdSend
Description:Send MSC_MSG(RCP) message to the specified CBUS channel(port)
Parameters:     channel     - CBUS channel
		vsCommand   - MSC_MSG cmd (RCP, RCPK or RCPE)
		cmdData     - MSC_MSG data
Returns:        TRUE        - successful queue/write
		FALSE       - write and/or queue failed

------------------------------------------------------------------------------*/
Bool si_cbus_mscmsg_subcmd_send(byte channel, byte vsCommand, byte cmdData)
{

	cbus_req_t	req;
	/*Send MSC_MSG command (Vendor Specific command)*/
	req.command     = MHD_MSC_MSG;
	req.msgData[0]  = vsCommand;
	req.msgData[1]  = cmdData;
	return si_cbus_write_command(channel, &req);

}

/*------------------------------------------------------------------------------

	Function:    SI_CbusRcpMessageAck
	Description: Send RCP_K (acknowledge) message to the specified CBUSi
	channel	and set the request status to idle.
	Parameters:  channel     - CBUS channel
	Returns:     TRUE        - successful queue/write
	FALSE       - write and/or queue failed

------------------------------------------------------------------------------*/
Bool si_cbus_rcpmessage_ack(byte channel, byte cmdStatus, byte keyCode)
{

	si_cbus_request_set_idle(channel, CBUS_REQ_IDLE);

	if (cmdStatus != MHD_MSC_MSG_NO_ERROR)
		si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_RCPE, cmdStatus);

	return si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_RCPK, keyCode);
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusRapMessageAck
	Description: Send RAPK (acknowledge) message to the specified CBUS
		channel and set the request status to idle.
	Parameters:  channel     - CBUS channel
	Returns:     TRUE        - successful queue/write
	FALSE       - write and/or queue failed

------------------------------------------------------------------------------*/
Bool si_cbus_rapmessage_ack(byte channel, byte cmdStatus)
{
	pr_info("si_cbus_rapmessage_ack:%x\n", (int)cmdStatus);
	si_cbus_request_set_idle(channel, CBUS_REQ_IDLE);

	return si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_RAPK, cmdStatus);

}

/*------------------------------------------------------------------------------

	Function:    SI_CbusSendDcapRdyMsg
	Description: Send a msg to peer informing the devive capability
	registers are ready to be read.
	Parameters:  channel to check
	Returns: TRUE    - success
		FALSE   - failure

------------------------------------------------------------------------------*/
Bool si_cbus_send_dcap_rdymsg(byte channel)
{

	/*cbus_req_t *pReq; // SIMG old code */
	cbus_req_t pReq;
	Bool result = TRUE;

	if (l_cbus[channel].connected) {
		pr_info("si_cbus_send_dcap_rdymsg Called!!\n");
	/*send a msg to peer that the device capability registers are
						ready to be read.*/
		/*set DCAP_RDY bit */
		pReq.command = MHD_WRITE_STAT;
		pReq.offsetData = 0x00;
		pReq.msgData[0] = BIT_0;
	/*result = si_cbus_write_command(0, pReq); // SIMG old code */
		result = si_cbus_write_command(0, &pReq);
		/*et DCAP_CHG bit*/
		pReq.command = MHD_SET_INT;
		pReq.offsetData = 0x00;
		pReq.msgData[0] = BIT_0;
	/*result = si_cbus_write_command(0, pReq); // SIMG old code */
		result = si_cbus_write_command(0, &pReq);
		dev_cap_regs_ready_bit = TRUE;
	}
	return result;
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusHandler
	Description: Check the state of any current CBUS message
	on specified channel. Handle responses or failures and send any
	pending message if channel is IDLE.
	Parameters:  channel - CBUS channel to check, must be in range, NOT 0xFF
	Returns:     SUCCESS or one of CBUS_SOFTWARE_ERRORS_t

------------------------------------------------------------------------------*/
byte si_cbus_handler(byte channel)
{

	byte result = STATUS_SUCCESS;

	/* Check the channel interrupt status to see if anybody is  */
	/* talking to us. If they are, talk back.                   */
	result = cbus_check_interrupt_status(channel);
	/* Don't bother with the rest if the heart is gone. */
	if ((result == ERROR_NO_HEARTBEAT) || (result == ERROR_NACK_FROM_PEER)) {
		pr_info("si_cbus_handler:: cbus_check_interrupt_status returned -->> %02X\n",
								(int)result);
		return result;
	}
	/* Update the channel state machine as necessary.   */
	switch (l_cbus[channel].state) {
	case CBUS_IDLE:
		result = cbus_command_get_nextinqueue(channel);
		break;
	case CBUS_SENT:
		break;
	case CBUS_XFR_DONE:
		l_cbus[channel].state = CBUS_IDLE;
	/* We may be waiting for a response message, but the    */
	/* request queue is idle.                               */
		l_cbus[channel].request[l_cbus[channel].activeIndex].reqStatus
							= CBUS_REQ_IDLE;
		break;
	case CBUS_WAIT_RESPONSE:
		break;
	case CBUS_RECEIVED:
	/* pr_info(MSG_ALWAYS, ("si_cbus_handler::
	l_cbus[ channel].state -->> %02X\n", (int)(l_cbus[ channel].state));*/
	/* pr_info(MSG_ALWAYS, ("result -->> %02X\n", (int)(result));*/
	/* Either command or response data has been received.   */
		break;
	default:
	/* Not a valid state, reset to IDLE and get out with failure. */
		l_cbus[channel].state = CBUS_IDLE;
		result = ERROR_INVALID;
		break;

	}

	return result;
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusWriteCommand
	Description: Place a command in the CBUS message queue.
	If queue was empty, send the new command immediately.
	Parameters:  channel - CBUS channel to write
	pReq    - Pointer to a cbus_req_t structure containing the
		command to write
	Returns:     TRUE    - successful queue/write
	FALSE   - write and/or queue failed

------------------------------------------------------------------------------*/



Bool si_cbus_write_command(byte channel, cbus_req_t *pReq)
{
	byte queueIndex;
	Bool  success = FALSE;

	if (l_cbus[channel].connected) {
		pr_info("CBUS Write command\n");
		/* Copy the request to the queue.   */
		for (queueIndex = 0; queueIndex < CBUS_MAX_COMMAND_QUEUE;
								queueIndex++) {
			if (l_cbus[channel].request[queueIndex].reqStatus ==
								CBUS_REQ_IDLE) {
				/*Found an idle queue entry, copy the request
						and set to pending.*/
				memcpy(&l_cbus[channel].request[queueIndex],
						pReq, sizeof(cbus_req_t));
				l_cbus[channel].request[queueIndex].reqStatus
						= CBUS_REQ_PENDING;
				success = TRUE;
				break;
			}
		}
	/* If successful at putting the request into the queue, decide  */
	/* whether it can be sent now or later.                         */
		pr_info("state:%x\n", (int)l_cbus[channel].state);
		pr_info("channel:%x queueIndex:%x\n",
					(int)channel, (int)queueIndex);
		pr_info("CBUS:: Sending MSC command %02X, %02X, %02X\n",
		(int)pReq->command, (int)pReq->msgData[0], (int)pReq->msgData[1]);

		if (success) {
			switch (l_cbus[channel].state) {
			case CBUS_IDLE:
			case CBUS_RECEIVED:
				success = cbus_command_get_nextinqueue(channel);
				break;
			case CBUS_WAIT_RESPONSE:
			case CBUS_SENT:
			case CBUS_XFR_DONE:
		/* Another command is in progress, the Handler loop will*/
		/* send the new command when the bus is free. */
				break;
			default:
			/* Illegal values return to IDLE state.     */
				pr_info("CBUS:: Channel State: %02X (illegal)\n",
					(int)l_cbus[channel].state);
				l_cbus[channel].state = CBUS_IDLE;
				l_cbus[channel].request[queueIndex].reqStatus
								= CBUS_REQ_IDLE;
				success = FALSE;
				break;
			}
		} else {
			pr_info("CBUS:: Queue full - Request0: %02X Request1: %02X\n",
			(int)l_cbus[channel].request[0].reqStatus,
			(int)l_cbus[channel].request[1].reqStatus);
		}
	}
	return success;
}



/*------------------------------------------------------------------------------

Function:    SI_CbusUpdateBusStatus
Description: Check the BUS status interrupt register for this channel and
	update the channel data as needed. channel.
Parameters:  channel to check
Returns:     TRUE    - connected
	FALSE   - not connected
Note: This function should be called periodically to update the bus status

------------------------------------------------------------------------------*/

Bool si_cbus_update_bus_status(byte channel)
{
	byte busStatus;

	busStatus = sii_regio_cub_read(REG_CBUS_BUS_STATUS, channel);
	pr_info("CBUS status:%x\n", (int)busStatus);
	l_cbus[channel].connected = (busStatus & BIT_BUS_CONNECTED) != 0;
	pr_info("CBUS connected:%x\n", (int)l_cbus[channel].connected);

	/* Clear the interrupt register bits.   */
	sii_regio_cbus_write(REG_CBUS_BUS_STATUS, channel, busStatus);

	return l_cbus[channel].connected;
}

/*------------------------------------------------------------------------------

	Function:    SI_CbusInitialize
	Description: Attempts to intialize the CBUS. If register
	reads return 0xFF, it declares error in initialization.
	Initializes discovery enabling registers and anything needed in
	config register, interrupt masks.
	Returns:     TRUE if no problem

------------------------------------------------------------------------------*/

Bool si_cbus_initialize(void)
{

	byte     channel;
	int	result = STATUS_SUCCESS;
	word	devcap_reg;
	int	regval;

	memset(&l_cbus, 0, sizeof(l_cbus));
	dev_cap_regs_ready_bit = FALSE;

	/* Determine the Port Switch input ports that are selected for MHD  */
	/* operation and initialize the port to channel decode array.       */
	channel = 0;

	/* Setup local DEVCAP registers for read by the peer*/
	devcap_reg = REG_CBUS_DEVICE_CAP_0;

	sii_regio_cbus_write(devcap_reg++, channel, MHD_DEV_ACTIVE);
	sii_regio_cbus_write(devcap_reg++, channel, MHD_VERSION);
	sii_regio_cbus_write(devcap_reg++, channel, MHD_DEVICE_CATEGORY);
	sii_regio_cbus_write(devcap_reg++, channel, 0);
	sii_regio_cbus_write(devcap_reg++, channel, 0);
	sii_regio_cbus_write(devcap_reg++, channel, MHD_DEV_VID_LINK_SUPPRGB444);
	sii_regio_cbus_write(devcap_reg++, channel, MHD_DEV_AUD_LINK_2CH);
	sii_regio_cbus_write(devcap_reg++, channel, 0);	/* not for source*/
	sii_regio_cbus_write(devcap_reg++, channel, MHD_LOGICAL_DEVICE_MAP);
	sii_regio_cbus_write(devcap_reg++, channel, 0);	/* not for source*/
	sii_regio_cbus_write(devcap_reg++, channel,
		MHD_RCP_SUPPORT | MHD_RAP_SUPPORT);	/* feature flag*/
	sii_regio_cbus_write(devcap_reg++, channel, 0);
	sii_regio_cbus_write(devcap_reg++, channel, 0);	/* reserved*/
	sii_regio_cbus_write(devcap_reg++, channel, MHD_SCRATCHPAD_SIZE);
	sii_regio_cbus_write(devcap_reg++, channel, MHD_INTERRUPT_SIZE);
	sii_regio_cbus_write(devcap_reg++, channel, 0);	/*reserved*/

	if (sii_regio_cub_read(REG_CBUS_SUPPORT, channel) == 0xff) {
		/*Display all registers for debugging. Only at initialization.*/
		pr_info("cbus initialization failed\n");
		cbus_display_registers(0, 0x30);
		return ERROR_INIT;
	}

	sii_regio_cbus_write(REG_CBUS_INTR_ENABLE, channel,
		(BIT_CONNECT_CHG | BIT_MSC_MSG_RCV | BIT_MSC_XFR_DONE
		| BIT_MSC_XFR_ABORT | BIT_MSC_ABORT | BIT_HEARTBEAT_TIMEOUT));
	regval = sii_regio_cub_read(REG_CBUS_LINK_CONTROL_2, channel);
	regval = (regval | 0x0C);
	sii_regio_cbus_write(REG_CBUS_LINK_CONTROL_2, channel, regval);
	/* Clear legacy bit on Wolverine TX.*/
	regval = sii_regio_cub_read(REG_MSC_TIMEOUT_LIMIT, channel);
	sii_regio_cbus_write(REG_MSC_TIMEOUT_LIMIT, channel,
			(regval & MSC_TIMEOUT_LIMIT_MSB_MASK));
	/* Set NMax to 1*/
	sii_regio_cbus_write(REG_CBUS_LINK_CONTROL_1, channel, 0x01);
	pr_info("cbus_initialize. Poll interval = %d ms. CBUS Connected = %d\n",
		(int)CBUS_FW_INTR_POLL_MILLISECS,
			(int)si_cbus_channel_connected(channel));

	return result;
}
