/*
 *  wacom_i2c_flash.c - Wacom Digitizer Controller Flash Driver
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

#include "wacom_i2c_flash.h"

int wacom_i2c_flash_chksum(struct wacom_i2c *wac_i2c, unsigned char *flash_data,
			   unsigned long *max_address)
{
	unsigned long i;
	unsigned long chksum = 0;

	for (i = 0x0000; i <= *max_address; i++)
		chksum += flash_data[i];

	chksum &= 0xFFFF;

	return (int)chksum;
}

int wacom_i2c_flash_cmd(struct wacom_i2c *wac_i2c)
{
	int ret, len;
	u8 buf[8];

	pr_info("wacom: %s\n", __func__);

	buf[0] = 0x0d;
	buf[1] = FLASH_START0;
	buf[2] = FLASH_START1;
	buf[3] = FLASH_START2;
	buf[4] = FLASH_START3;
	buf[5] = FLASH_START4;
	buf[6] = FLASH_START5;
	buf[7] = 0x0d;

	len = 8;
	ret = wacom_i2c_send(wac_i2c, buf, len, true);
	if (ret < 0) {
		pr_err(
			"wacom: sending flash command failed 2\n");
		return false;
	}
	msleep(270);

	return 0;
}

bool flash_cmd(struct wacom_i2c *wac_i2c)
{
	int rv, len;
	u8 buf[10];

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x32;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c,
			buf, len, true);
	if (rv < 0)
		return false;

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 2;
	buf[len++] = 2;
	rv = wacom_i2c_send(wac_i2c,
			buf, len, true);
	if (rv < 0)
		return false;

	return true;
}

bool flash_query(struct wacom_i2c *wac_i2c)
{
	int rv, ECH;
	u8 buf[4];
	u16 len;
	unsigned char command[CMD_SIZE];
	unsigned char response[RSP_SIZE];

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	pr_info(
		"wacom: %s started buf[3]:%d len:%d\n",
		__func__, buf[3], len);
	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 5;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_QUERY;
	command[6] = ECH = 7;

	rv = wacom_i2c_send(wac_i2c, command, 7, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	rv = wacom_i2c_recv(wac_i2c, response,
		BOOT_RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	if ((response[3] != QUERY_CMD) ||
	     (response[4] != ECH)) {
		pr_err("wacom: %s res3:%d res4:%d\n", __func__,
				response[3], response[4]);
		return false;
	}
	if (response[5] != QUERY_RSP) {
		pr_err("wacom: %s res5:%d\n", __func__, response[5]);
		return false;
	}

	return true;
}

bool flash_blver(struct wacom_i2c *wac_i2c, int *blver)
{
	int rv, ECH;
	u8 buf[4];
	u16 len;
	unsigned char command[CMD_SIZE];
	unsigned char response[RSP_SIZE];

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 5;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_BLVER;
	command[6] = ECH = 7;

	rv = wacom_i2c_send(wac_i2c, command, 7, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	rv = wacom_i2c_recv(wac_i2c, response, BOOT_RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	if ((response[3] != BOOT_CMD) ||
		(response[4] != ECH))
		return false;

	*blver = (int)response[5];

	return true;
}

bool flash_mputype(struct wacom_i2c *wac_i2c, int *pMpuType)
{
	int rv, ECH;
	u8 buf[4];
	u16 len;
	unsigned char command[CMD_SIZE];
	unsigned char response[RSP_SIZE];

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 5;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_MPU;
	command[6] = ECH = 7;

	rv = wacom_i2c_send(wac_i2c, command, 7, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	rv = wacom_i2c_recv(wac_i2c, response, BOOT_RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	if ((response[3] != MPU_CMD) ||
		(response[4] != ECH))
		return false;

	*pMpuType = (int)response[5];

	return true;
}

bool flash_security_unlock(struct wacom_i2c *wac_i2c, int *status)
{
	int rv, ECH;
	u8 buf[4];
	u16 len;
	unsigned char command[CMD_SIZE];
	unsigned char response[RSP_SIZE];

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 5;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_SECURITY_UNLOCK;
	command[6] = ECH = 7;

	rv = wacom_i2c_send(wac_i2c, command, 7, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	rv = wacom_i2c_recv(wac_i2c, response, BOOT_RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	if ((response[3] != SEC_CMD) ||
		(response[4] != ECH))
		return false;

	*status = (int)response[5];

	return true;
}

bool flash_end(struct wacom_i2c *wac_i2c)
{
	int rv, ECH;
	u8 buf[4];
	u16 len;
	unsigned char command[CMD_SIZE];

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 5;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_EXIT;
	command[6] = ECH = 7;

	rv = wacom_i2c_send(wac_i2c, command, 7, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	return true;
}


bool flash_devcieType(struct wacom_i2c *wac_i2c)
{
	int rv;
	u8 buf[4];
	u16 len;

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x32;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	rv = wacom_i2c_recv(wac_i2c, buf, 4, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	return true;
}

int GetBLVersion(struct wacom_i2c *wac_i2c, int *pBLVer)
{
	int rv;
	wacom_i2c_flash_cmd(wac_i2c);
	if (!flash_query(wac_i2c)) {
		if (!wacom_i2c_flash_cmd(wac_i2c))
			return EXIT_FAIL_ENTER_FLASH_MODE;
		else {
			msleep(100);
			if (!flash_query(wac_i2c))
				return EXIT_FAIL_FLASH_QUERY;
		}
	}

	rv = flash_blver(wac_i2c, pBLVer);
	if (rv)
		return EXIT_OK;
	else
		return EXIT_FAIL_GET_BOOT_LOADER_VERSION;
}

int
GetMpuType(struct wacom_i2c *wac_i2c, int *pMpuType)
{
	int rv;

	if (!flash_query(wac_i2c)) {
		if (!wacom_i2c_flash_cmd(wac_i2c))
			return EXIT_FAIL_ENTER_FLASH_MODE;
		else {
			msleep(100);
			if (!flash_query(wac_i2c))
				return EXIT_FAIL_FLASH_QUERY;
		}
	}

	rv = flash_mputype(wac_i2c, pMpuType);
	if (rv)
		return EXIT_OK;
	else
		return EXIT_FAIL_GET_MPU_TYPE;
}

int SetSecurityUnlock(struct wacom_i2c *wac_i2c, int *pStatus)
{
	int rv;

	if (!flash_query(wac_i2c)) {
		if (!wacom_i2c_flash_cmd(wac_i2c))
			return EXIT_FAIL_ENTER_FLASH_MODE;
		else{
			msleep(100);
			if (!flash_query(wac_i2c))
				return EXIT_FAIL_FLASH_QUERY;
		}
	}

	rv = flash_security_unlock(wac_i2c, pStatus);
	if (rv)
		return EXIT_OK;
	else
		return EXIT_FAIL;
}

bool flash_erase(struct wacom_i2c *wac_i2c, bool bAllUserArea,
		int *eraseBlock, int num)
{
	int rv, ECH;
	unsigned char sum;
	unsigned char buf[72];
	unsigned char cmd_chksum;
	u16 len;
	int i, j;
	unsigned char command[CMD_SIZE];
	unsigned char response[RSP_SIZE];

	for (i = 0; i < num; i++) {
		msleep(500);
retry:
		len = 0;
		buf[len++] = 4;
		buf[len++] = 0;
		buf[len++] = 0x37;
		buf[len++] = CMD_SET_FEATURE;

		pr_info("wacom: %s sending SET_FEATURE:%d\n", __func__, i);

		rv = wacom_i2c_send(wac_i2c, buf, len, false);
		if (rv < 0) {
			pr_err("wacom: %s(%d) %d\n",
				__func__, rv, __LINE__);
			return false;
		}

		pr_info("wacom: %s setting a command:%d\n", __func__, i);

		command[0] = 5;
		command[1] = 0;
		command[2] = 7;
		command[3] = 0;
		command[4] = BOOT_CMD_REPORT_ID;
		command[5] = BOOT_ERASE_FLASH;
		command[6] = ECH = i;
		command[7] = *eraseBlock;
		eraseBlock++;

		sum = 0;
		for (j = 0; j < 8; j++)
			sum += command[j];
		cmd_chksum = ~sum+1;
		command[8] = cmd_chksum;

		rv = wacom_i2c_send(wac_i2c, command, 9, false);
		if (rv < 0) {
			pr_err("wacom: %s(%d) %d\n",
				__func__, rv, __LINE__);
			return false;
		}

		msleep(5000);

		len = 0;
		buf[len++] = 4;
		buf[len++] = 0;
		buf[len++] = 0x38;
		buf[len++] = CMD_GET_FEATURE;

		pr_info("wacom: %s sending GET_FEATURE :%d\n", __func__, i);
		rv = wacom_i2c_send(wac_i2c, buf, len, false);
		if (rv < 0) {
			pr_err("wacom: %s(%d) %d\n",
				__func__, rv, __LINE__);
			return false;
		}

		len = 0;
		buf[len++] = 5;
		buf[len++] = 0;

		rv = wacom_i2c_send(wac_i2c, buf, len, false);
		if (rv < 0) {
			pr_err("wacom: %s(%d) %d\n",
				__func__, rv, __LINE__);
			return false;
		}

		rv = wacom_i2c_recv(wac_i2c, response, BOOT_RSP_SIZE, false);
		if (rv < 0) {
			pr_err("wacom: %s(%d) %d\n",
				__func__, rv, __LINE__);
			return false;
		}

		if ((response[3] != ERS_CMD) ||
		    (response[4] != ECH)) {
			pr_err("wacom: %s failing 5:%d\n", __func__, i);
			return false;
		}

		if (response[5] == 0x80) {
			pr_err("wacom: %s retry\n", __func__);
			goto retry;
		}
		if (response[5] != ACK) {
			pr_err("wacom: %s failing 6:%d res5:%d\n", __func__,
					i, response[5]);
			return false;
		}
		pr_info("wacom: %s %d\n", __func__, i);
	}
	return true;
}

bool is_flash_marking(struct wacom_i2c *wac_i2c,
		bool *bMarking, int iMpuID)
{
	const int MAX_CMD_SIZE = (12 + FLASH_BLOCK_SIZE + 2);
	int rv, ECH;
	unsigned char flash_data[FLASH_BLOCK_SIZE];
	unsigned char buf[300];
	unsigned char sum;
	int len;
	unsigned int i, j;
	unsigned char response[RSP_SIZE];
	unsigned char command[MAX_CMD_SIZE];

	*bMarking = false;

	pr_info("wacom: %s started\n", __func__);
	for (i = 0; i < FLASH_BLOCK_SIZE; i++)
		flash_data[i] = 0xFF;

	flash_data[56] = 0x00;

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 76;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_VERIFY_FLASH;
	command[6] = ECH = 1;
	command[7] = 0xC0;
	command[8] = 0x1F;
	command[9] = 0x01;
	command[10] = 0x00;
	command[11] = 8;

	sum = 0;
	for (j = 0; j < 12; j++)
		sum += command[j];

	command[MAX_CMD_SIZE - 2] = ~sum+1;

	sum = 0;
	pr_info("wacom: %s start writing command\n", __func__);
	for (i = 12; i < (FLASH_BLOCK_SIZE + 12); i++) {
		command[i] = flash_data[i - 12];
		sum += flash_data[i - 12];
	}
	command[MAX_CMD_SIZE - 1] = ~sum + 1;

	pr_info("wacom: %s sending command\n", __func__);
	rv = wacom_i2c_send(wac_i2c, command, MAX_CMD_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}


	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	pr_info("wacom: %s sending GET_FEATURE 1\n", __func__);
	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}


	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	pr_info("wacom: %s sending GET_FEATURE 2\n", __func__);
	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}


	pr_info("wacom: %s receiving GET_FEATURE\n", __func__);
	rv = wacom_i2c_recv(wac_i2c, response, RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}


	pr_info("wacom: %s checking response\n", __func__);
	if ((response[3] != MARK_CMD) ||
	    (response[4] != ECH) ||
	    (response[5] != ACK)) {
		pr_err("wacom: %s fails res3:%d res4:%d res5:%d\n", __func__,
				response[3], response[4], response[5]);
		return false;
	}

	*bMarking = true;
	return true;
}

bool flash_write_block(struct wacom_i2c *wac_i2c, char *flash_data,
		unsigned long ulAddress, u8 *pcommand_id)
{
	const int MAX_COM_SIZE = (12 + FLASH_BLOCK_SIZE + 2);
	int len, ECH;
	unsigned char buf[300];
	int rv;
	unsigned char sum;
	unsigned char command[MAX_COM_SIZE];
	unsigned char response[RSP_SIZE];
	unsigned int i;

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	command[0] = 5;
	command[1] = 0;
	command[2] = 76;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_WRITE_FLASH;
	command[6] = ECH = ++(*pcommand_id);
	command[7] = ulAddress&0x000000ff;
	command[8] = (ulAddress&0x0000ff00) >> 8;
	command[9] = (ulAddress&0x00ff0000) >> 16;
	command[10] = (ulAddress&0xff000000) >> 24;
	command[11] = 8;
	sum = 0;
	for (i = 0; i < 12; i++)
		sum += command[i];
	command[MAX_COM_SIZE - 2] = ~sum+1;

	sum = 0;
	for (i = 12; i < (FLASH_BLOCK_SIZE + 12); i++) {
		command[i] = flash_data[ulAddress+(i-12)];
		sum += flash_data[ulAddress+(i-12)];
	}
	command[MAX_COM_SIZE - 1] = ~sum+1;

	rv = wacom_i2c_send(wac_i2c, command, BOOT_CMD_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c, buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	rv = wacom_i2c_send(wac_i2c, response, BOOT_RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	if ((response[3] != WRITE_CMD) ||
		(response[4] != ECH) ||
		response[5] != ACK)
		return false;

	return true;
}

bool flash_write(struct wacom_i2c *wac_i2c,
		unsigned char *flash_data, unsigned long start_address,
		unsigned long *max_address, int mpuType)
{
	unsigned long ulAddress;
	int rv;
	unsigned long pageNo = 0;
	u8 command_id = 0;

	pr_info("wacom: %s flash_write start\n", __func__);

	for (ulAddress = start_address; ulAddress <
			*max_address; ulAddress += FLASH_BLOCK_SIZE) {
		unsigned int j;
		bool bWrite = false;

		for (j = 0; j < FLASH_BLOCK_SIZE; j++) {
			if (flash_data[ulAddress+j] == 0xFF)
				continue;
			else	{
				bWrite = true;
				break;
			}
		}

		if (!bWrite) {
			pageNo++;
			continue;
		}

		rv = flash_write_block(wac_i2c, flash_data,
				ulAddress, &command_id);
		if (rv < 0)
			return false;

		pageNo++;
	}

	return true;
}

bool flash_verify(struct wacom_i2c *wac_i2c,
		unsigned char *flash_data, unsigned long start_address,
		unsigned long *max_address, int mpuType)
{
	int ECH;
	unsigned long ulAddress;
	bool rv;
	unsigned long pageNo = 0;
	u8 command_id = 0;
	pr_info("wacom: %s verify starts\n", __func__);
	for (ulAddress = start_address; ulAddress <
			*max_address; ulAddress += FLASH_BLOCK_SIZE) {
		const int MAX_CMD_SIZE = 12 + FLASH_BLOCK_SIZE + 2;
		unsigned char buf[300];
		unsigned char sum;
		int len;
		unsigned int i, j;
		unsigned char command[MAX_CMD_SIZE];
		unsigned char response[RSP_SIZE];

		len = 0;
		buf[len++] = 4;
		buf[len++] = 0;
		buf[len++] = 0x37;
		buf[len++] = CMD_SET_FEATURE;

		rv = wacom_i2c_send(wac_i2c, buf, len, false);
		if (rv < 0)
			return false;

		command[0] = 5;
		command[1] = 0;
		command[2] = 76;
		command[3] = 0;
		command[4] = BOOT_CMD_REPORT_ID;
		command[5] = BOOT_VERIFY_FLASH;
		command[6] = ECH = ++command_id;
		command[7] = ulAddress&0x000000ff;
		command[8] = (ulAddress&0x0000ff00) >> 8;
		command[9] = (ulAddress&0x00ff0000) >> 16;
		command[10] = (ulAddress&0xff000000) >> 24;
		command[11] = 8;

		sum = 0;
		for (j = 0; j < 12; j++)
			sum += command[j];
		command[MAX_CMD_SIZE - 2] = ~sum+1;

		sum = 0;
		for (i = 12; i < (FLASH_BLOCK_SIZE + 12); i++) {
			command[i] = flash_data[ulAddress+(i-12)];
			sum += flash_data[ulAddress+(i-12)];
		}
		command[MAX_CMD_SIZE - 1] = ~sum+1;

		rv = wacom_i2c_send(wac_i2c, command, BOOT_CMD_SIZE, false);
		if (rv < 0)
			return false;

		usleep(10000);

		len = 0;
		buf[len++] = 4;
		buf[len++] = 0;
		buf[len++] = 0x38;
		buf[len++] = CMD_GET_FEATURE;

		rv = wacom_i2c_send(wac_i2c, buf, len, false);
		if (rv < 0)
			return false;

		len = 0;
		buf[len++] = 5;
		buf[len++] = 0;

		rv = wacom_i2c_send(wac_i2c, buf, len,
			false);
		if (rv < 0)
			return false;

		rv = wacom_i2c_recv(wac_i2c, response,
			BOOT_RSP_SIZE, false);
		if (rv < 0)
			return false;

		if ((response[3] != VERIFY_CMD) ||
		    (response[4] != ECH) || (response[5] != ACK)) {
			pr_err(
				"wacom: %s res3:%d res4:%d res5:%d\n", __func__,
				response[3], response[4], response[5]);
			return false;
		}
		pageNo++;
	}

	return true;
}

bool flash_marking(struct wacom_i2c *wac_i2c,
		bool bMarking, int iMpuID)
{
	const int MAX_CMD_SIZE = 12 + FLASH_BLOCK_SIZE + 2;
	int rv, ECH;
	unsigned char flash_data[FLASH_BLOCK_SIZE];
	unsigned char buf[300];
	unsigned char response[RSP_SIZE];
	unsigned char sum;
	int len;
	unsigned int i, j;
	unsigned char command[MAX_CMD_SIZE];

	for (i = 0; i < FLASH_BLOCK_SIZE; i++)
		flash_data[i] = 0xFF;

	if (bMarking)
		flash_data[56] = 0x00;

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x37;
	buf[len++] = CMD_SET_FEATURE;

	rv = wacom_i2c_send(wac_i2c,
		buf, len, false);
	if (rv < 0)
		return false;

	command[0] = 5;
	command[1] = 0;
	command[2] = 76;
	command[3] = 0;
	command[4] = BOOT_CMD_REPORT_ID;
	command[5] = BOOT_WRITE_FLASH;
	command[6] = ECH = 1;
	command[7] = 0xC0;
	command[8] = 0x1F;
	command[9] = 0x01;
	command[10] = 0x00;
	command[11] = 8;

	sum = 0;
	for (j = 0; j < 12; j++)
		sum += command[j];
	command[MAX_CMD_SIZE - 2] = ~sum + 1;

	sum = 0;
	for (i = 12; i < (FLASH_BLOCK_SIZE + 12); i++) {
		command[i] = flash_data[i-12];
		sum += flash_data[i-12];
	}

	/* Report:data checksum */
	command[MAX_CMD_SIZE - 1] = ~sum + 1;


	rv = wacom_i2c_send(wac_i2c,
		command, BOOT_CMD_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	usleep(10000);

	len = 0;
	buf[len++] = 4;
	buf[len++] = 0;
	buf[len++] = 0x38;
	buf[len++] = CMD_GET_FEATURE;

	rv = wacom_i2c_send(wac_i2c,
		buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	len = 0;
	buf[len++] = 5;
	buf[len++] = 0;

	rv = wacom_i2c_send(wac_i2c,
		buf, len, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	pr_info("wacom: %s confirming marking\n", __func__);
	rv = wacom_i2c_recv(wac_i2c,
		response, BOOT_RSP_SIZE, false);
	if (rv < 0) {
		pr_err("wacom: %s(%d) %d\n",
			__func__, rv, __LINE__);
		return false;
	}

	if ((response[3] != 1) ||
	    (response[4] != ECH) ||
	    (response[5] != ACK)) {
		pr_err(
			"wacom: %s failing res3:%d res4:%d res5:%d\n",
			__func__, response[3], response[4], response[5]);
		return false;
	}

	return true;
}

int FlashWrite(struct wacom_i2c *wac_i2c, char *filename)
{
	unsigned long	max_address = 0;
	unsigned long	start_address = 0x4000;
	int eraseBlock[32], eraseBlockNum;
	bool bRet;
	unsigned long ulMaxRange;
	int iChecksum;
	int iBLVer, iMpuType, iStatus;
	int iRet;
	bool bBootFlash = false;

	iRet = GetBLVersion(wac_i2c, &iBLVer);
	if (iRet != EXIT_OK) {
		pr_err(
			"wacom: %s Failed to get Boot Loader version\n",
			__func__);
		return iRet;
	}

	iRet = GetMpuType(wac_i2c, &iMpuType);
	if (iRet != EXIT_OK) {
		pr_err("wacom: %s Failed to get MPU type\n", __func__);
		return iRet;
	}

	pr_info("wacom: %s start reading hex file\n", __func__);

	eraseBlockNum = 0;
	start_address = 0x4000;
	max_address = 0x12FFF;
	eraseBlock[eraseBlockNum++] = 2;
	eraseBlock[eraseBlockNum++] = 1;
	eraseBlock[eraseBlockNum++] = 0;
	eraseBlock[eraseBlockNum++] = 3;

	if (bBootFlash)
		eraseBlock[eraseBlockNum++] = 4;

	iChecksum =
		wacom_i2c_flash_chksum(wac_i2c,
			wac_i2c->fw_bin, &max_address);
	pr_info("wacom: %s iChecksum:%d\n",
		__func__, iChecksum);

	bRet = true;

	iRet = SetSecurityUnlock(wac_i2c, &iStatus);
	if (iRet != EXIT_OK)
		return iRet;

	ulMaxRange = max_address;
	ulMaxRange -= start_address;
	ulMaxRange >>= 6;
	if (max_address > (ulMaxRange << 6))
		ulMaxRange++;

	pr_info("wacom: %s connecting to wacom digitizer\n", __func__);

	if (!bBootFlash) {
		pr_info(
			"wacom: %s erasing the user program\n",
			__func__);

		bRet = flash_erase(wac_i2c, true, eraseBlock, eraseBlockNum);
		if (!bRet) {
			pr_err(
				"wacom: %s failed to erase the user program\n",
				__func__);
			return EXIT_FAIL_ERASE;
		}
	}

	pr_info("wacom: %s writing new user program\n", __func__);

	bRet = flash_write(wac_i2c, wac_i2c->fw_bin, start_address,
		&max_address, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: %s failed to write the user program\n",
			__func__);
		return EXIT_FAIL_WRITE_FIRMWARE;
	}

	bRet = flash_marking(wac_i2c, true, iMpuType);
	if (!bRet) {
		pr_err("wacom: %s failed to set mark\n", __func__);
		return EXIT_FAIL_WRITE_FIRMWARE;
	}

	pr_info("wacom: %s writing completed\n", __func__);
	return EXIT_OK;
}

int FlashVerify(struct wacom_i2c *wac_i2c, char *filename)
{
	unsigned long max_address = 0;
	unsigned long start_address = 0x4000;
	bool bRet;
	int iChecksum;
	int iBLVer, iMpuType;
	unsigned long ulMaxRange;
	bool bMarking;
	int iRet;

	iRet = GetBLVersion(wac_i2c, &iBLVer);
	if (iRet != EXIT_OK) {
		pr_err(
			"wacom: %s failed to get Boot Loader version\n",
			__func__);
		return iRet;
	}

	iRet = GetMpuType(wac_i2c, &iMpuType);
	if (iRet != EXIT_OK) {
		pr_err(
			"wacom: %s failed to get MPU type\n",
			__func__);
		return iRet;
	}

	start_address = 0x4000;
	max_address = 0x11FBF;

	iChecksum = wacom_i2c_flash_chksum(wac_i2c,
		wac_i2c->fw_bin, &max_address);
	pr_info(
		"wacom: %s check sum is: %d\n",
		__func__, iChecksum);

	ulMaxRange = max_address;
	ulMaxRange -= start_address;
	ulMaxRange >>= 6;
	if (max_address > (ulMaxRange << 6))
		ulMaxRange++;

	bRet = flash_verify(wac_i2c, wac_i2c->fw_bin, start_address,
		&max_address, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: %s failed to verify the firmware\n",
			__func__);
		return EXIT_FAIL_VERIFY_FIRMWARE;
	}

	bRet = is_flash_marking(wac_i2c, &bMarking, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: %s there's no marking\n",
			__func__);
		return EXIT_FAIL_VERIFY_WRITING_MARK;
	}

	pr_info(
		"wacom: %s verifying completed\n",
		__func__);

	return EXIT_OK;
}

int wacom_i2c_flash(struct wacom_i2c *wac_i2c)
{
	unsigned long max_address = 0;
	unsigned long start_address = 0x4000;
	int eraseBlock[32], eraseBlockNum;
	bool bRet;
	int iChecksum;
	int iBLVer, iMpuType, iStatus;
	bool bBootFlash = false;
	bool bMarking;
	int iRet;
	unsigned long ulMaxRange;

	pr_info("wacom: %s\n", __func__);
	pr_info(
		"wacom: start getting the boot loader version\n");
	/*Obtain boot loader version*/
	iRet = GetBLVersion(wac_i2c, &iBLVer);
	if (iRet != EXIT_OK) {
		pr_err(
			"wacom: %s failed to get Boot Loader version\n",
			__func__);
		return EXIT_FAIL_GET_BOOT_LOADER_VERSION;
	}

	pr_info(
		"wacom: start getting the MPU version\n");
	/*Obtain MPU type: this can be manually done in user space*/
	iRet = GetMpuType(wac_i2c, &iMpuType);
	if (iRet != EXIT_OK) {
		pr_err(
			"wacom: %s failed to get MPU type\n",
			__func__);
		return EXIT_FAIL_GET_MPU_TYPE;
	}

	/*Set start and end address and block numbers*/
	eraseBlockNum = 0;
	start_address = 0x4000;
	max_address = 0x12FFF;
	eraseBlock[eraseBlockNum++] = 2;
	eraseBlock[eraseBlockNum++] = 1;
	eraseBlock[eraseBlockNum++] = 0;
	eraseBlock[eraseBlockNum++] = 3;

	/*If MPU is in Boot mode, do below*/
	if (bBootFlash)
		eraseBlock[eraseBlockNum++] = 4;

	pr_info(
		"wacom: obtaining the checksum\n");
	/*Calculate checksum*/
	iChecksum = wacom_i2c_flash_chksum(wac_i2c,
		wac_i2c->fw_bin, &max_address);
	pr_info(
		"wacom: Checksum is :%d\n",
		iChecksum);

	bRet = true;

	pr_info(
		"wacom: setting the security unlock\n");
	/*Unlock security*/
	iRet = SetSecurityUnlock(wac_i2c, &iStatus);
	if (iRet != EXIT_OK) {
		pr_err(
			"wacom: %s failed to set security unlock\n",
			__func__);
		return iRet;
	}

	/*Set adress range*/
	ulMaxRange = max_address;
	ulMaxRange -= start_address;
	ulMaxRange >>= 6;
	if (max_address > (ulMaxRange<<6))
		ulMaxRange++;

	pr_info(
		"wacom: connecting to Wacom Digitizer\n");
	pr_info(
		"wacom: erasing the current firmware\n");
	/*Erase the old program*/
	bRet = flash_erase(wac_i2c, true, eraseBlock,  eraseBlockNum);
	if (!bRet) {
		pr_err(
			"wacom: %s failed to erase the user program\n",
			__func__);
		return EXIT_FAIL_ERASE;
	}
	pr_info(
		"wacom: erasing done\n");

	max_address = 0x11FC0;

	pr_info(
		"wacom: writing new firmware\n");
	/*Write the new program*/
	bRet = flash_write(wac_i2c, wac_i2c->fw_bin, start_address,
		&max_address, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: %s failed to write firmware\n",
			__func__);
		return EXIT_FAIL_WRITE_FIRMWARE;
	}

	pr_info(
		"wacom: start marking\n");

	/*Set mark in writing process*/
	bRet = flash_marking(wac_i2c, true, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: %s failed to mark firmware\n",
			__func__);
		return EXIT_FAIL_WRITE_FIRMWARE;
	}

	/*Set the address for verify*/
	start_address = 0x4000;
	max_address = 0x11FBF;

	pr_info(
		"wacom: start the verification\n");
	/*Verify the written program*/
	bRet = flash_verify(wac_i2c, wac_i2c->fw_bin, start_address,
		&max_address, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: failed to verify the firmware\n");
		return EXIT_FAIL_VERIFY_FIRMWARE;
	}


	pr_info(
		"wacom: checking the mark\n");
	/*Set mark*/
	bRet = is_flash_marking(wac_i2c, &bMarking, iMpuType);
	if (!bRet) {
		pr_err(
			"wacom: %s marking firmwrae failed\n",
			__func__);
		return EXIT_FAIL_WRITING_MARK_NOT_SET;
	}

	/*Enable */
	pr_info(
		"wacom: closing the boot mode\n");

	bRet = flash_end(wac_i2c);
	if (!bRet) {
		pr_err(
			"wacom: %s closing boot mode failed\n",
			__func__);
		return EXIT_FAIL_WRITING_MARK_NOT_SET;
	}

	pr_info(
		"wacom: write and verify completed\n");
	return EXIT_OK;
}
