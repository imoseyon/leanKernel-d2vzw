/***************************************************************************
*
* file     si_cpCBUS.c
*
* brief    CP 9387 Starter Kit CDC demonstration code.
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
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include "Common_Def.h"
#include "si_apiCbus.h"
/*extern void rcp_cbus_uevent(void);*/


#if (IS_CBUS == 1)

/*------------------------------------------------------------------------------

	Module variables

------------------------------------------------------------------------------*/



typedef struct {
	byte rcpKeyCode;
	/*!< RCP CBUS Key Code*/
	char   rcpName[30];
} SI_Rc5RcpConversion_t;



SI_Rc5RcpConversion_t RcpSourceToSink[] = {
	{ MHD_RCP_CMD_SELECT,			"Select"},
	{ MHD_RCP_CMD_UP,				"Up"},
	{ MHD_RCP_CMD_DOWN,				"Down"},
	{ MHD_RCP_CMD_LEFT,				"Left"},
	{ MHD_RCP_CMD_RIGHT,			"Right"},
	/*{ MHD_RCP_CMD_RIGHT_UP,		"Right Up"},
	{ MHD_RCP_CMD_RIGHT_DOWN,		"Right Down"},
	{ MHD_RCP_CMD_LEFT_UP,		"Left Up"},
	{ MHD_RCP_CMD_LEFT_DOWN,		"Left Down"},*/
	{ MHD_RCP_CMD_ROOT_MENU,		"Root Menu"},
	/*{ MHD_RCP_CMD_SETUP_MENU,		"Setup Menu"},
	{ MHD_RCP_CMD_CONTENTS_MENU,	"Contents Menu"},
	{ MHD_RCP_CMD_FAVORITE_MENU,	"Favorite Menu"},*/
	{ MHD_RCP_CMD_EXIT,				"Exit"},
				/* 0x0E - 0x1F Reserved */
	{ MHD_RCP_CMD_NUM_0,			"Num 0"},
	{ MHD_RCP_CMD_NUM_1,			"Num 1"},
	{ MHD_RCP_CMD_NUM_2,			"Num 2"},
	{ MHD_RCP_CMD_NUM_3,			"Num 3"},
	{ MHD_RCP_CMD_NUM_4,			"Num 4"},
	{ MHD_RCP_CMD_NUM_5,			"Num 5"},
	{ MHD_RCP_CMD_NUM_6,			"Num 6"},
	{ MHD_RCP_CMD_NUM_7,			"Num 7"},
	{ MHD_RCP_CMD_NUM_8,			"Num 8"},
	{ MHD_RCP_CMD_NUM_9,			"Num 9"},
	{ MHD_RCP_CMD_ENTER,			"Enter"},
	{ MHD_RCP_CMD_CLEAR,			"Clear"},
				/* 0x2D - 0x2F Reserved */
	{ MHD_RCP_CMD_CH_UP,			"Channel Up"},
	{ MHD_RCP_CMD_CH_DOWN,			"Channel Down"},
	{ MHD_RCP_CMD_PRE_CH,			"Previous Channel"},
	{ MHD_RCP_CMD_SOUND_SELECT,		"Sound Select"},
	/*{ MHD_RCP_CMD_INPUT_SELECT,	"Input Select"},
	{ MHD_RCP_CMD_SHOW_INFO,		"Show Info"},
	{ MHD_RCP_CMD_HELP,			"Help"},
	{ MHD_RCP_CMD_PAGE_UP,		"Page Up"},
	{ MHD_RCP_CMD_PAGE_DOWN,		"Page Down"},
				0x39 - 0x40 Reserved */
	{ MHD_RCP_CMD_VOL_UP,			"Volume Up"},
	{ MHD_RCP_CMD_VOL_DOWN,			"Volume Down"},
	{ MHD_RCP_CMD_MUTE,				"Mute"},
	{ MHD_RCP_CMD_PLAY,				"Play"},
	{ MHD_RCP_CMD_STOP,				"Stop"},
	{ MHD_RCP_CMD_PAUSE,			"Pause"},
	{ MHD_RCP_CMD_RECORD,			"Record"},
	{ MHD_RCP_CMD_REWIND,			"Rewind"},
	{ MHD_RCP_CMD_FAST_FWD,			"Fast Fwd"},
	{ MHD_RCP_CMD_EJECT,			"Eject"},
	{ MHD_RCP_CMD_FWD,				"Forward"},
	{ MHD_RCP_CMD_BKWD,				"Backward"},
				/* 0X4D - 0x4F Reserved
	{ MHD_RCP_CMD_ANGLE,			"Angle"},
	{ MHD_RCP_CMD_SUBPICTURE,		"Subpicture"},
				0x52 - 0x5F Reserved*/
	{ MHD_RCP_CMD_PLAY_FUNC,		"Play Function"},
	{ MHD_RCP_CMD_PAUSE_PLAY_FUNC,	"Pause Play Function"},
	{ MHD_RCP_CMD_RECORD_FUNC,		"Record Function"},
	{ MHD_RCP_CMD_PAUSE_REC_FUNC,	"Pause Record Function"},
	{ MHD_RCP_CMD_STOP_FUNC,		"Stop Function"},
	{ MHD_RCP_CMD_MUTE_FUNC,		"Mute Function"},
	{ MHD_RCP_CMD_UN_MUTE_FUNC,		"Un-Mute Function"},
	/*{ MHD_RCP_CMD_TUNE_FUNC,		"Tune Function"},
	{ MHD_RCP_CMD_MEDIA_FUNC,		"Media Function"},
				0x69 - 0x70 Reserved
	{ MHD_RCP_CMD_F1,				"F1"},
	{ MHD_RCP_CMD_F2,				"F2"},
	{ MHD_RCP_CMD_F3,				"F3"},
	{ MHD_RCP_CMD_F4,				"F4"},
	{ MHD_RCP_CMD_F5,				"F5"},
				0x76 - 0x7D Reserved
	{ MHD_RCP_CMD_VS,				"Vendor Specific"}
				0x7F Reserved*/
};

/*------------------------------------------------------------------------------
Function: CbusRc5toRcpConvert
Description: Translate RC5 command to CBUS RCP command
Parameters:  keyData: key code from the remote controller
Returns: RCP code equivalent to passed RC5 key code or 0xFF if not in list.
------------------------------------------------------------------------------*/
static byte cbus_rc5_to_rcpconvert(byte keyCode)
{
	byte i;
	byte retVal = 0xFF;
	byte length = sizeof(RcpSourceToSink)/sizeof(SI_Rc5RcpConversion_t);

	for (i = 0; i < length ; i++) {
		if (keyCode == RcpSourceToSink[i].rcpKeyCode) {
			retVal = RcpSourceToSink[i].rcpKeyCode;
#if (RCP_DEBUG == 1)
		pr_info("CPCBUS:: Send ----> %s\n", RcpSourceToSink[i].rcpName);
#endif
		break;
		}
	}
	/* Return the new code or 0xFF if not found.    */
	return ((i == length) ? 0xFF : retVal);
}

/*------------------------------------------------------------------------------
Function:    CpCbusSendRcpMessage
Description: Convert input port number to CBUS channel and send the
passed RC5 key code as a CBUS RCP key code.
Parameters:  port    - Port Processor input port index
	keyCode - Remote control button code.
Returns:     true if successful, false if not MHD port or other failure.
------------------------------------------------------------------------------*/
Bool CpCbusSendRcpMessage(byte channel, byte keyCode)
{
	Bool  success;
	success = FALSE;
	for ( ;; ) {
		pr_info("CPCBUS:: Sending RCP Msg:: %02X keycode to channel %d CBUS\n\n",
						(int)keyCode, (int)channel);
		if (channel == 0xFF) {
			pr_info("\n::::::: Bad channel -- ");
		break;
		}
		keyCode = cbus_rc5_to_rcpconvert(keyCode);
		if (keyCode == 0xFF) {
			pr_info("\n::::::: Bad KeyCode -- ");
			break;
		}
		success = si_cbus_mscmsg_subcmd_send(channel, MHD_MSC_MSG_RCP, keyCode);
		break;
	}

	if (!success)
		pr_info("Unable to send command :::::::\n");

	return success;
}


/*------------------------------------------------------------------------------
Function:    CpCbusSendRapMessage
Parameters:  port    - Port Processor input port index
	actionCode - Action code.
Returns:     true if successful, false if not MHD port or other failure.
------------------------------------------------------------------------------*/
Bool cp_cbus_send_rapmessage(byte channel, byte actCode)
{
	Bool  success;
	success = FALSE;
	for ( ;; ) {

		pr_info("CPCBUS:: Sending RAP Msg:: %02X action code \
			to channel %d CBUS\n\n", (int)actCode, (int)channel);

		if (channel == 0xFF) {
			pr_info("\n::::::: Bad channel -- ");
			break;
		}

		if ((actCode == MHD_RAP_CMD_POLL) || (actCode == MHD_RAP_CMD_CHG_QUIET)
			|| (actCode != MHD_RAP_CMD_CHG_ACTIVE_PWR)) {
			success = si_cbus_mscmsg_subcmd_send(channel,
						MHD_MSC_MSG_RAP, actCode);
			break;
		} else {
			pr_info("\n::::::: Bad action code -- ");
			break;
		}
	}

	if (!success)
		pr_info("Unable to send action command :::::::\n");

	return success;
}

/*------------------------------------------------------------------------------
Function:    CpProcessRcpMessage
Description: Process the passed RCP message.
Returns:     The RCPK status code.
------------------------------------------------------------------------------*/
static byte cp_process_rcpmessage(byte channel, byte rcpData)
{

	byte rcpkStatus = MHD_MSC_MSG_NO_ERROR;
	pr_info("RCP Key Code: 0x%02X, channel: 0x%02X\n",
					(int)rcpData, (int)channel);

	switch (rcpData) {
	case MHD_RCP_CMD_SELECT:
		pr_info("\nSelect received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_UP:
		pr_info("\nUp received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_DOWN:
		pr_info("\nDown received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_LEFT:
		pr_info("\nLeft received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_RIGHT:
		pr_info("\nRight received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_RIGHT_UP:
		pr_info("\n MHD_RCP_CMD_RIGHT_UP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_RIGHT_DOWN:
		pr_info("\n MHD_RCP_CMD_RIGHT_DOWN 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_LEFT_UP:
		pr_info("\n MHD_RCP_CMD_LEFT_UP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_LEFT_DOWN:
		pr_info("\n MHD_RCP_CMD_LEFT_DOWN 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_ROOT_MENU:
		pr_info("\nRoot Menu received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_SETUP_MENU:
		pr_info("\n MHD_RCP_CMD_SETUP_MENU 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_CONTENTS_MENU:
		pr_info("\n MHD_RCP_CMD_CONTENTS_MENU 0x%02x\n\n",
							(int)rcpData);
		break;
	case MHD_RCP_CMD_FAVORITE_MENU:
		pr_info("\n MHD_RCP_CMD_FAVORITE_MENU 0x%02x\n\n",
							(int)rcpData);
		break;
	case MHD_RCP_CMD_EXIT:
		pr_info("\nExit received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_0:
		pr_info("\nNumber 0 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_1:
		pr_info("\nNumber 1 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_2:
		pr_info("\nNumber 2 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_3:
		pr_info("\nNumber 3 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_4:
		pr_info("\nNumber 4 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_5:
		pr_info("\nNumber 5 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_6:
		pr_info("\nNumber 6 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_7:
		pr_info("\nNumber 7 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_8:
		pr_info("\nNumber 8 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_NUM_9:
		pr_info("\nNumber 9 received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_DOT:
		pr_info("\n MHD_RCP_CMD_DOT 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_ENTER:
		pr_info("\nEnter received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_CLEAR:
		pr_info("\nClear received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_CH_UP:
		pr_info("\n MHD_RCP_CMD_CH_UP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_CH_DOWN:
		pr_info("\n MHD_RCP_CMD_CH_DOWN 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PRE_CH:
		pr_info("\n MHD_RCP_CMD_PRE_CH 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_SOUND_SELECT:
		pr_info("\nSound Select received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_INPUT_SELECT:
		pr_info("\n MHD_RCP_CMD_INPUT_SELECT 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_SHOW_INFO:
		pr_info("\n MHD_RCP_CMD_SHOW_INFO 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_HELP:
		pr_info("\n MHD_RCP_CMD_HELP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PAGE_UP:
		pr_info("\n MHD_RCP_CMD_PAGE_UP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PAGE_DOWN:
		pr_info("\n MHD_RCP_CMD_PAGE_DOWN 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_VOL_UP:
		pr_info("\n MHD_RCP_CMD_VOL_UP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_VOL_DOWN:
		pr_info("\n MHD_RCP_CMD_VOL_DOWN 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_MUTE:
		pr_info("\n MHD_RCP_CMD_MUTE 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PLAY:
		pr_info("\nPlay received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_STOP:
		pr_info("\n MHD_RCP_CMD_STOP 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PAUSE:
		pr_info("\nPause received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_RECORD:
		pr_info("\n MHD_RCP_CMD_RECORD 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_FAST_FWD:
		pr_info("\nFastfwd received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_REWIND:
		pr_info("\nRewind received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_EJECT:
		pr_info("\nEject received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_FWD:
		pr_info("\nForward received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_BKWD:
		pr_info("\nBackward received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PLAY_FUNC:
		pr_info("\nPlay Function received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RCP_CMD_PAUSE_PLAY_FUNC:
		pr_info("\nPause_Play Function received 0x%02x\n\n",
							(int)rcpData);
		break;
	case MHD_RCP_CMD_STOP_FUNC:
		pr_info("\nStop Function received 0x%02x\n\n", (int)rcpData);
		break;
	default:
		rcpkStatus = MHD_MSC_MSG_INEFFECTIVE_KEY_CODE;
		break;
	}
	if (rcpkStatus == MHD_MSC_MSG_INEFFECTIVE_KEY_CODE)
		pr_info("\nKeyCode not recognized or supported.\n\n");

	return rcpkStatus;

}

/*------------------------------------------------------------------------------
	Function:    CpProcessRapMessage
	Description: Process the passed RAP message.
	Returns:     The RAPK status code.
------------------------------------------------------------------------------*/
static byte cp_process_rapmessage(byte channel, byte rcpData)
{

	byte rapkStatus = MHD_MSC_MSG_NO_ERROR;
	pr_info("RAP Key Code: 0x%02X, channel: 0x%02X\n",
					(int)rcpData, (int)channel);

	switch (rcpData) {
	case MHD_RAP_CMD_POLL:
		pr_info("\nPOLL received 0x%02x\n\n", (int)rcpData);
		break;
	case MHD_RAP_CMD_CHG_ACTIVE_PWR:
		pr_info("\nCHANGE TO ACTIVE POWER STATE received 0x%02x\n\n",
								(int)rcpData);
		break;
	case MHD_RAP_CMD_CHG_QUIET:
		pr_info("\nCHANGE TO QUIET STATE received 0x%02x\n\n",
							(int)rcpData);
		/*TPI_GoToD3();*/
		break;
	default:
		rapkStatus = MHD_MSC_MSG_RAP_UNRECOGNIZED_ACT_CODE;
		break;
	}
	if (rapkStatus == MHD_MSC_MSG_RAP_UNRECOGNIZED_ACT_CODE)
		pr_info("\nKeyCode not recognized !!\n\n");

		return rapkStatus;
}


/*------------------------------------------------------------------------------
Function:    CbusConnectionCheck
Description: Display any change in CBUS connection state and enable
	CBUS heartbeat if channel has been connected.
Parameters:  channel - CBUS channel to check
------------------------------------------------------------------------------*/
static void cbus_connection_check(byte channel)
{
	static byte busConnected[MHD_MAX_CHANNELS] = {0};

	/* If CBUS connection state has changed for this channel,   */
	/* update channel state and hardware.                       */
	if (busConnected[channel] != si_cbus_channel_connected(channel)) {
		busConnected[channel] = si_cbus_channel_connected(channel);

	/* heartbeat has been disabled in all products
	si_cbus_heartbeat( channel, busConnected[ channel ] );*/
	pr_info("CPCBUS:: ***Channel: %d,  CBUS %s ****\n",
	(int)channel, busConnected[channel] ? "Connected" : "Unconnected");

	}
}

/*Disabling //NAGSM_Android_SEL_Kernel_Aakash_20101206*/
/*static byte CbusRcpData = 0xFF; //Intialize with Junk Value
byte GetCbusRcpData (void)
{
	return CbusRcpData;
}

void ResetCbusRcpData(void)
{
	CbusRcpData = 0xFF;	//Junk Data
	return;
}*/

/*------------------------------------------------------------------------------
Function:    CpCbusProcessPrivateMessage
Description: Get the result of the last message sent and use it appropriately
or process a request from the connected device.
Parameters:  channel - CBUS channel that has message data for us.
------------------------------------------------------------------------------*/
static void cp_cbus_process_privatemessage(byte channel)
{
	byte     status;
	cbus_req_t  *pCmdRequest;
	pCmdRequest = si_cbus_request_data(channel);

	/* CbusRcpData = pCmdRequest->offsetData; //Disabling
	//NAGSM_Android_SEL_Kernel_Aakash_20101207*/
	switch (pCmdRequest->command) {
	case MHD_MSC_MSG_RCP:
		pr_info("MHD_MSC_MSG_RCP 1\n");
	/* Acknowledge receipt of command and process it.  Note that
	we could send the ack before processing anything, because it
	is an indicator that the command was properly received, not
	that it was executed, however, we use one function to parse
	the command for errors AND for processing. The only thing we
	must do is make sure that the processing does not exceed the
	ACK response time limit.                                     */

	/*Disabling //NAGSM_Android_SEL_Kernel_Aakash_20101206/MHL v1/
	NAGSM_Android_SEL_Kernel_Aakash_20101130
	Inform Kernel only on MHD_MSC_MSG_RCP */
	/*CbusRcpData = pCmdRequest->offsetData;
	rcp_cbus_uevent(); MHL v1 /NAGSM_Android_SEL_Kernel_Aakash_20101126*/
		status = cp_process_rcpmessage(channel, pCmdRequest->offsetData);
		si_cbus_rcpmessage_ack(channel, status, pCmdRequest->offsetData);
		break;
	case MHD_MSC_MSG_RCPK:
		pr_info("MHD_MSC_MSG_RCPK 1\n");
		break;
	case MHD_MSC_MSG_RCPE:
		pr_info("MHD_MSC_MSG_RCPE 1\n");
		break;
	case MHD_MSC_MSG_RAP:
		pr_info("MHD_MSC_MSG_RAP 1\n");
		status = cp_process_rapmessage(channel, pCmdRequest->offsetData);
		si_cbus_rapmessage_ack(channel, status);
		break;
	case MHD_MSC_MSG_RAPK:
		break;
	}
}


/*------------------------------------------------------------------------------
Function:    CpCbusHandler
Description: Polls the send/receive state of the CBUS hardware.
------------------------------------------------------------------------------*/
void cp_cbus_handler(void)
{

	byte channel, status;
	/* Monitor all CBUS channels.   */
	for (channel = 0; channel < MHD_MAX_CHANNELS; channel++) {
		/* Update CBUS status.  */
		si_cbus_update_bus_status(channel);
		cbus_connection_check(channel);
		/* Monitor CBUS interrupts. */
		status = si_cbus_handler(channel);
		if (status == STATUS_SUCCESS) {
		/* Get status of current request, if any.   */
			status = si_cbus_request_status(channel);
			switch (status)	{
			case CBUS_REQ_IDLE:
				pr_info("CBUS_REQ_IDLE\n");
				break;
			case CBUS_REQ_PENDING:
				pr_info("CBUS_REQ_PENDING\n");
				break;
			case CBUS_REQ_SENT:
				pr_info("CBUS_REQ_SENT\n");
				break;
			case CBUS_REQ_RECEIVED:
				pr_info("CBUS_REQ_RECEIVED\n");
			/* Received a message or message response.  */
			/* Go do what is appropriate.               */
				cp_cbus_process_privatemessage(channel);
				break;
			default:
			break;
		}

		} else if (status == ERROR_NO_HEARTBEAT) {
			pr_info("Lost CBUS channel %d heartbeat\n", (int)channel);
		} else if (status == ERROR_NACK_FROM_PEER) {
			pr_info("NACK received from peer,cmd should be sent again. 0x%02x\n",
							(int)channel);
		} else {
			/* Lee: Only thing that comes here
			is interrupt timeout -- is this bad? */
		}
	}
}


/* Function:    CpCbusInitialize
Description: Initialize the CBUS subsystem and enabled the default channels */
void cp_cbus_initialize(void)
{
	/* Initialize the basic hardware.   */
	si_cbus_initialize();
}
#endif
