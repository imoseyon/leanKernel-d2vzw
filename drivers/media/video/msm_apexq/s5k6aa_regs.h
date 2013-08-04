/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifdef	__S5K6AA_H__
#define __S5K6AA_H__


typedef struct s5k6aa_reg {
	unsigned short subaddr;
	unsigned int value;
}s5k6aa_short_t;

s5k6aa_short_t s5k6aa_init_reg[] =
{
//================================================================
// Device : S5K6AAFX
// MIPI Interface for Noncontious Clock
//================================================================

//================================================================
// History
// 20101214 Hyungseok.Yoon T&P Update from SIRC(ISP06AA00C00000.ASIC.TNP)
//================================================================

//================================================================
// ARM GO and Delay
//================================================================
{0xFCFC, 0xD000},
{0x0010, 0x0001},	// Reset
{0x0004, 0x0000},	// Disable Auto Address Increment : 0 Chunghwan Park
{0x1030, 0x0000},	// Clear host interrupt so main will wait
{0x0014, 0x0001},	// ARM Go
{0xFFFE, 0x0064},		// Wait100ms

//================================================================
// Trap and Patch
//================================================================
// svn://transrdsrv/svn/svnroot/System/Software/tcevb/SDK+FW/ISP_Oscar/Firmware
// Rev: 33110-33110
// Signature:
// md5 f0ba942df15b96de5c09e6cf13fed9c9 .btp
// md5 8bc59f72129cb36e6f6db4be5ddca1f6 .htp
// md5 954ec97efcabad291d89f63e29f32490 .RegsMap.h
// md5 5c29fe50b51e7e860313f5b3b6452bfd .RegsMap.bin
// md5 6211407baaa234b753431cde4ba32402 .base.RegsMap.h
// md5 90cc21d42cc5f02eb80b2586e5c46d9b .base.RegsMap.bin
{0xFCFC, 0xD000},        
{0x0004, 0x0001},	// ensable Auto Address Increment : 1
{0x0028, 0x7000},
{0x002A, 0x1D60},
{0x0F12, 0xB570},
{0x0F12, 0x4936},
{0x0F12, 0x4836},
{0x0F12, 0x2205},
{0x0F12, 0xF000},
{0x0F12, 0xFA4E},
{0x0F12, 0x4935},
{0x0F12, 0x2002},
{0x0F12, 0x83C8},
{0x0F12, 0x2001},
{0x0F12, 0x3120},
{0x0F12, 0x8088},
{0x0F12, 0x4933},
{0x0F12, 0x0200},
{0x0F12, 0x8008},
{0x0F12, 0x4933},
{0x0F12, 0x8048},
{0x0F12, 0x4933},
{0x0F12, 0x4833},
{0x0F12, 0x2204},
{0x0F12, 0xF000},
{0x0F12, 0xFA3E},
{0x0F12, 0x4932},
{0x0F12, 0x4833},
{0x0F12, 0x2206},
{0x0F12, 0xF000},
{0x0F12, 0xFA39},
{0x0F12, 0x4932},
{0x0F12, 0x4832},
{0x0F12, 0x2207},
{0x0F12, 0xF000},
{0x0F12, 0xFA34},
{0x0F12, 0x4931},
{0x0F12, 0x4832},
{0x0F12, 0x2208},
{0x0F12, 0xF000},
{0x0F12, 0xFA2F},
{0x0F12, 0x4931},
{0x0F12, 0x4831},
{0x0F12, 0x2209},
{0x0F12, 0xF000},
{0x0F12, 0xFA2A},
{0x0F12, 0x4930},
{0x0F12, 0x4831},
{0x0F12, 0x220A},
{0x0F12, 0xF000},
{0x0F12, 0xFA25},
{0x0F12, 0x4930},
{0x0F12, 0x4830},
{0x0F12, 0x220B},
{0x0F12, 0xF000},
{0x0F12, 0xFA20},
{0x0F12, 0x482F},
{0x0F12, 0x4930},
{0x0F12, 0x6108},
{0x0F12, 0x4830},
{0x0F12, 0x39FF},
{0x0F12, 0x3901},
{0x0F12, 0x6748},
{0x0F12, 0x482F},
{0x0F12, 0x1C0A},
{0x0F12, 0x32C0},
{0x0F12, 0x6390},
{0x0F12, 0x482E},
{0x0F12, 0x6708},
{0x0F12, 0x491A},
{0x0F12, 0x482D},
{0x0F12, 0x3108},
{0x0F12, 0x60C1},
{0x0F12, 0x6882},
{0x0F12, 0x1A51},
{0x0F12, 0x8201},
{0x0F12, 0x4C2B},
{0x0F12, 0x2607},
{0x0F12, 0x6821},
{0x0F12, 0x0736},
{0x0F12, 0x42B1},
{0x0F12, 0xDA05},
{0x0F12, 0x4829},
{0x0F12, 0x22D8},
{0x0F12, 0x1C05},
{0x0F12, 0xF000},
{0x0F12, 0xFA09},
{0x0F12, 0x6025},
{0x0F12, 0x68A1},
{0x0F12, 0x42B1},
{0x0F12, 0xDA07},
{0x0F12, 0x4825},
{0x0F12, 0x2224},
{0x0F12, 0x3824},
{0x0F12, 0xF000},
{0x0F12, 0xFA00},
{0x0F12, 0x4822},
{0x0F12, 0x3824},
{0x0F12, 0x60A0},
{0x0F12, 0x4D22},
{0x0F12, 0x6D29},
{0x0F12, 0x42B1},
{0x0F12, 0xDA07},
{0x0F12, 0x481F},
{0x0F12, 0x228F},
{0x0F12, 0x00D2},
{0x0F12, 0x30D8},
{0x0F12, 0x1C04},
{0x0F12, 0xF000},
{0x0F12, 0xF9F2},
{0x0F12, 0x652C},
{0x0F12, 0xBC70},
{0x0F12, 0xBC08},
{0x0F12, 0x4718},
{0x0F12, 0x218B},
{0x0F12, 0x7000},
{0x0F12, 0x127B},
{0x0F12, 0x0000},
{0x0F12, 0x0398},
{0x0F12, 0x7000},
{0x0F12, 0x1376},
{0x0F12, 0x7000},
{0x0F12, 0x2370},
{0x0F12, 0x7000},
{0x0F12, 0x1F0D},
{0x0F12, 0x7000},
{0x0F12, 0x890D},
{0x0F12, 0x0000},
{0x0F12, 0x1F2F},
{0x0F12, 0x7000},
{0x0F12, 0x27A9},
{0x0F12, 0x0000},
{0x0F12, 0x1FE1},
{0x0F12, 0x7000},
{0x0F12, 0x27C5},
{0x0F12, 0x0000},
{0x0F12, 0x2043},
{0x0F12, 0x7000},
{0x0F12, 0x285F},
{0x0F12, 0x0000},
{0x0F12, 0x2003},
{0x0F12, 0x7000},
{0x0F12, 0x28FF},
{0x0F12, 0x0000},
{0x0F12, 0x20CD},
{0x0F12, 0x7000},
{0x0F12, 0x6181},
{0x0F12, 0x0000},
{0x0F12, 0x20EF},
{0x0F12, 0x7000},
{0x0F12, 0x6663},
{0x0F12, 0x0000},
{0x0F12, 0x2123},
{0x0F12, 0x7000},
{0x0F12, 0x0100},
{0x0F12, 0x7000},
{0x0F12, 0x1EC1},
{0x0F12, 0x7000},
{0x0F12, 0x1EAD},
{0x0F12, 0x7000},
{0x0F12, 0x1F79},
{0x0F12, 0x7000},
{0x0F12, 0x04AC},
{0x0F12, 0x7000},
{0x0F12, 0x06CC},
{0x0F12, 0x7000},
{0x0F12, 0x23A4},
{0x0F12, 0x7000},
{0x0F12, 0x0704},
{0x0F12, 0x7000},
{0x0F12, 0xB510},
{0x0F12, 0xF000},
{0x0F12, 0xF9B9},
{0x0F12, 0x48C3},
{0x0F12, 0x49C3},
{0x0F12, 0x8800},
{0x0F12, 0x8048},
{0x0F12, 0xBC10},
{0x0F12, 0xBC08},
{0x0F12, 0x4718},
{0x0F12, 0xB5F8},
{0x0F12, 0x1C06},
{0x0F12, 0x4DC0},
{0x0F12, 0x68AC},
{0x0F12, 0x1C30},
{0x0F12, 0xF000},
{0x0F12, 0xF9B3},
{0x0F12, 0x68A9},
{0x0F12, 0x4ABC},
{0x0F12, 0x42A1},
{0x0F12, 0xD003},
{0x0F12, 0x4BBD},
{0x0F12, 0x8A1B},
{0x0F12, 0x3301},
{0x0F12, 0x8013},
{0x0F12, 0x8813},
{0x0F12, 0x1C14},
{0x0F12, 0x2B00},
{0x0F12, 0xD00F},
{0x0F12, 0x2201},
{0x0F12, 0x4281},
{0x0F12, 0xD003},
{0x0F12, 0x8C2F},
{0x0F12, 0x42B9},
{0x0F12, 0xD300},
{0x0F12, 0x2200},
{0x0F12, 0x60AE},
{0x0F12, 0x2A00},
{0x0F12, 0xD003},
{0x0F12, 0x8C28},
{0x0F12, 0x42B0},
{0x0F12, 0xD800},
{0x0F12, 0x1C30},
{0x0F12, 0x1E59},
{0x0F12, 0x8021},
{0x0F12, 0xBCF8},
{0x0F12, 0xBC08},
{0x0F12, 0x4718},
{0x0F12, 0xB510},
{0x0F12, 0x1C04},
{0x0F12, 0x48AF},
{0x0F12, 0xF000},
{0x0F12, 0xF997},
{0x0F12, 0x4AAD},
{0x0F12, 0x4BAE},
{0x0F12, 0x8811},
{0x0F12, 0x885B},
{0x0F12, 0x8852},
{0x0F12, 0x4359},
{0x0F12, 0x1889},
{0x0F12, 0x4288},
{0x0F12, 0xD800},
{0x0F12, 0x1C08},
{0x0F12, 0x6020},
{0x0F12, 0xE7C5},
{0x0F12, 0xB570},
{0x0F12, 0x1C05},
{0x0F12, 0xF000},
{0x0F12, 0xF98F},
{0x0F12, 0x49A5},
{0x0F12, 0x8989},
{0x0F12, 0x4348},
{0x0F12, 0x0200},
{0x0F12, 0x0C00},
{0x0F12, 0x2101},
{0x0F12, 0x0349},
{0x0F12, 0xF000},
{0x0F12, 0xF98E},
{0x0F12, 0x1C04},
{0x0F12, 0x489F},
{0x0F12, 0x8F80},
{0x0F12, 0xF000},
{0x0F12, 0xF991},
{0x0F12, 0x1C01},
{0x0F12, 0x20FF},
{0x0F12, 0x43C0},
{0x0F12, 0xF000},
{0x0F12, 0xF994},
{0x0F12, 0xF000},
{0x0F12, 0xF998},
{0x0F12, 0x1C01},
{0x0F12, 0x4898},
{0x0F12, 0x8840},
{0x0F12, 0x4360},
{0x0F12, 0x0200},
{0x0F12, 0x0C00},
{0x0F12, 0xF000},
{0x0F12, 0xF97A},
{0x0F12, 0x6028},
{0x0F12, 0xBC70},
{0x0F12, 0xBC08},
{0x0F12, 0x4718},
{0x0F12, 0xB5F1},
{0x0F12, 0xB082},
{0x0F12, 0x4D96},
{0x0F12, 0x4E91},
{0x0F12, 0x88A8},
{0x0F12, 0x1C2C},
{0x0F12, 0x3420},
{0x0F12, 0x4F90},
{0x0F12, 0x2800},
{0x0F12, 0xD018},
{0x0F12, 0xF000},
{0x0F12, 0xF988},
{0x0F12, 0x9001},
{0x0F12, 0x9802},
{0x0F12, 0x6B39},
{0x0F12, 0x0200},
{0x0F12, 0xF000},
{0x0F12, 0xF974},
{0x0F12, 0xF000},
{0x0F12, 0xF978},
{0x0F12, 0x9901},
{0x0F12, 0xF000},
{0x0F12, 0xF95F},
{0x0F12, 0x8020},
{0x0F12, 0x8871},
{0x0F12, 0x0200},
{0x0F12, 0xF000},
{0x0F12, 0xF96A},
{0x0F12, 0x0400},
{0x0F12, 0x0C00},
{0x0F12, 0x21FF},
{0x0F12, 0x3101},
{0x0F12, 0xF000},
{0x0F12, 0xF97A},
{0x0F12, 0x8020},
{0x0F12, 0x88E8},
{0x0F12, 0x2800},
{0x0F12, 0xD00A},
{0x0F12, 0x4980},
{0x0F12, 0x8820},
{0x0F12, 0x3128},
{0x0F12, 0xF000},
{0x0F12, 0xF979},
{0x0F12, 0x8D38},
{0x0F12, 0x8871},
{0x0F12, 0x4348},
{0x0F12, 0x0200},
{0x0F12, 0x0C00},
{0x0F12, 0x8538},
{0x0F12, 0xBCFE},
{0x0F12, 0xBC08},
{0x0F12, 0x4718},
{0x0F12, 0xB510},
{0x0F12, 0x1C04},
{0x0F12, 0xF000},
{0x0F12, 0xF974},
{0x0F12, 0x6821},
{0x0F12, 0x0409},
{0x0F12, 0x0C09},
{0x0F12, 0x1A40},
{0x0F12, 0x4976},
{0x0F12, 0x6849},
{0x0F12, 0x4281},
{0x0F12, 0xD800},
{0x0F12, 0x1C08},
{0x0F12, 0xF000},
{0x0F12, 0xF971},
{0x0F12, 0x6020},
{0x0F12, 0xE75B},
{0x0F12, 0xB570},
{0x0F12, 0x6801},
{0x0F12, 0x040D},
{0x0F12, 0x0C2D},
{0x0F12, 0x6844},
{0x0F12, 0x486F},
{0x0F12, 0x8981},
{0x0F12, 0x1C28},
{0x0F12, 0xF000},
{0x0F12, 0xF927},
{0x0F12, 0x8060},
{0x0F12, 0x4970},
{0x0F12, 0x69C9},
{0x0F12, 0xF000},
{0x0F12, 0xF968},
{0x0F12, 0x1C01},
{0x0F12, 0x80A0},
{0x0F12, 0x0228},
{0x0F12, 0xF000},
{0x0F12, 0xF92D},
{0x0F12, 0x0400},
{0x0F12, 0x0C00},
{0x0F12, 0x8020},
{0x0F12, 0x496B},
{0x0F12, 0x2300},
{0x0F12, 0x5EC9},
{0x0F12, 0x4288},
{0x0F12, 0xDA02},
{0x0F12, 0x20FF},
{0x0F12, 0x3001},
{0x0F12, 0x8020},
{0x0F12, 0xE797},
{0x0F12, 0xB5F8},
{0x0F12, 0x1C04},
{0x0F12, 0x4867},
{0x0F12, 0x4E65},
{0x0F12, 0x7800},
{0x0F12, 0x6AB7},
{0x0F12, 0x2800},
{0x0F12, 0xD100},
{0x0F12, 0x6A37},
{0x0F12, 0x495D},
{0x0F12, 0x2800},
{0x0F12, 0x688D},
{0x0F12, 0xD100},
{0x0F12, 0x684D},
{0x0F12, 0x4859},
{0x0F12, 0x8841},
{0x0F12, 0x6820},
{0x0F12, 0x0200},
{0x0F12, 0xF000},
{0x0F12, 0xF94B},
{0x0F12, 0x8DF1},
{0x0F12, 0x434F},
{0x0F12, 0x0A3A},
{0x0F12, 0x4282},
{0x0F12, 0xD30C},
{0x0F12, 0x4D5C},
{0x0F12, 0x26FF},
{0x0F12, 0x8829},
{0x0F12, 0x3601},
{0x0F12, 0x43B1},
{0x0F12, 0x8029},
{0x0F12, 0xF000},
{0x0F12, 0xF944},
{0x0F12, 0x6020},
{0x0F12, 0x8828},
{0x0F12, 0x4330},
{0x0F12, 0x8028},
{0x0F12, 0xE73B},
{0x0F12, 0x1C0A},
{0x0F12, 0x436A},
{0x0F12, 0x0A12},
{0x0F12, 0x4282},
{0x0F12, 0xD304},
{0x0F12, 0x0200},
{0x0F12, 0xF000},
{0x0F12, 0xF8F3},
{0x0F12, 0x6020},
{0x0F12, 0xE7F4},
{0x0F12, 0x6025},
{0x0F12, 0xE7F2},
{0x0F12, 0xB410},
{0x0F12, 0x4848},
{0x0F12, 0x4950},
{0x0F12, 0x89C0},
{0x0F12, 0x2316},
{0x0F12, 0x5ECC},
{0x0F12, 0x1C02},
{0x0F12, 0x42A0},
{0x0F12, 0xDC00},
{0x0F12, 0x1C22},
{0x0F12, 0x82CA},
{0x0F12, 0x2318},
{0x0F12, 0x5ECA},
{0x0F12, 0x4290},
{0x0F12, 0xDC00},
{0x0F12, 0x1C10},
{0x0F12, 0x8308},
{0x0F12, 0xBC10},
{0x0F12, 0x4770},
{0x0F12, 0xB570},
{0x0F12, 0x1C06},
{0x0F12, 0x4C45},
{0x0F12, 0x2501},
{0x0F12, 0x8820},
{0x0F12, 0x02AD},
{0x0F12, 0x43A8},
{0x0F12, 0x8020},
{0x0F12, 0xF000},
{0x0F12, 0xF91E},
{0x0F12, 0x6030},
{0x0F12, 0xF7FF},
{0x0F12, 0xFFE0},
{0x0F12, 0x8820},
{0x0F12, 0x4328},
{0x0F12, 0x8020},
{0x0F12, 0xE741},
{0x0F12, 0xB570},
{0x0F12, 0x4C3D},
{0x0F12, 0x2501},
{0x0F12, 0x8820},
{0x0F12, 0x02ED},
{0x0F12, 0x43A8},
{0x0F12, 0x8020},
{0x0F12, 0xF000},
{0x0F12, 0xF916},
{0x0F12, 0xF7FF},
{0x0F12, 0xFFD1},
{0x0F12, 0x8820},
{0x0F12, 0x4328},
{0x0F12, 0x8020},
{0x0F12, 0xE732},
{0x0F12, 0x230D},
{0x0F12, 0x071B},
{0x0F12, 0x18C3},
{0x0F12, 0x8818},
{0x0F12, 0x2A00},
{0x0F12, 0xD001},
{0x0F12, 0x4308},
{0x0F12, 0xE000},
{0x0F12, 0x4388},
{0x0F12, 0x8018},
{0x0F12, 0x4770},
{0x0F12, 0xB570},
{0x0F12, 0x2402},
{0x0F12, 0x4932},
{0x0F12, 0x8809},
{0x0F12, 0x078A},
{0x0F12, 0xD500},
{0x0F12, 0x2406},
{0x0F12, 0x2900},
{0x0F12, 0xD01F},
{0x0F12, 0x1C02},
{0x0F12, 0x207D},
{0x0F12, 0x00C0},
{0x0F12, 0x2600},
{0x0F12, 0x4D2D},
{0x0F12, 0x2A00},
{0x0F12, 0xD019},
{0x0F12, 0x2101},
{0x0F12, 0x8229},
{0x0F12, 0xF000},
{0x0F12, 0xF8F9},
{0x0F12, 0x2200},
{0x0F12, 0x2101},
{0x0F12, 0x482A},
{0x0F12, 0x0309},
{0x0F12, 0xF7FF},
{0x0F12, 0xFFDB},
{0x0F12, 0x2008},
{0x0F12, 0x4304},
{0x0F12, 0x1C21},
{0x0F12, 0x4C26},
{0x0F12, 0x2200},
{0x0F12, 0x3C14},
{0x0F12, 0x1C20},
{0x0F12, 0xF7FF},
{0x0F12, 0xFFD2},
{0x0F12, 0x2200},
{0x0F12, 0x2121},
{0x0F12, 0x1C20},
{0x0F12, 0xF7FF},
{0x0F12, 0xFFCD},
{0x0F12, 0x802E},
{0x0F12, 0xE6FD},
{0x0F12, 0x822E},
{0x0F12, 0x0789},
{0x0F12, 0x0FC9},
{0x0F12, 0x0089},
{0x0F12, 0x223B},
{0x0F12, 0x4311},
{0x0F12, 0x8029},
{0x0F12, 0xF000},
{0x0F12, 0xF8DA},
{0x0F12, 0xE7F4},
{0x0F12, 0xB510},
{0x0F12, 0x491B},
{0x0F12, 0x8FC8},
{0x0F12, 0x2800},
{0x0F12, 0xD007},
{0x0F12, 0x2000},
{0x0F12, 0x87C8},
{0x0F12, 0x8F88},
{0x0F12, 0x4C19},
{0x0F12, 0x2800},
{0x0F12, 0xD002},
{0x0F12, 0x2008},
{0x0F12, 0x8020},
{0x0F12, 0xE689},
{0x0F12, 0x4815},
{0x0F12, 0x3060},
{0x0F12, 0x8900},
{0x0F12, 0x2800},
{0x0F12, 0xD103},
{0x0F12, 0x4814},
{0x0F12, 0x2101},
{0x0F12, 0xF000},
{0x0F12, 0xF8CA},
{0x0F12, 0x2010},
{0x0F12, 0x8020},
{0x0F12, 0xE7F2},
{0x0F12, 0x0000},
{0x0F12, 0x1376},
{0x0F12, 0x7000},
{0x0F12, 0x2370},
{0x0F12, 0x7000},
{0x0F12, 0x14D8},
{0x0F12, 0x7000},
{0x0F12, 0x235C},
{0x0F12, 0x7000},
{0x0F12, 0xF4B0},
{0x0F12, 0x0000},
{0x0F12, 0x1554},
{0x0F12, 0x7000},
{0x0F12, 0x1AB8},
{0x0F12, 0x7000},
{0x0F12, 0x0080},
{0x0F12, 0x7000},
{0x0F12, 0x046C},
{0x0F12, 0x7000},
{0x0F12, 0x0468},
{0x0F12, 0x7000},
{0x0F12, 0x1100},
{0x0F12, 0xD000},
{0x0F12, 0x198C},
{0x0F12, 0x7000},
{0x0F12, 0x0AC4},
{0x0F12, 0x7000},
{0x0F12, 0xB0A0},
{0x0F12, 0xD000},
{0x0F12, 0xB0B4},
{0x0F12, 0x0000},
{0x0F12, 0x01B8},
{0x0F12, 0x7000},
{0x0F12, 0x044E},
{0x0F12, 0x7000},
{0x0F12, 0x0450},
{0x0F12, 0x7000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x9CE7},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xF004},
{0x0F12, 0xE51F},
{0x0F12, 0x9FB8},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x14C1},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x27E1},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x88DF},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x275D},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x1ED3},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x27C5},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xF004},
{0x0F12, 0xE51F},
{0x0F12, 0xA144},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x1F87},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x27A9},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x1ECB},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x28FF},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x26F9},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x4027},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x9F03},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xF004},
{0x0F12, 0xE51F},
{0x0F12, 0x9D9C},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x285F},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x6181},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x6663},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x85D9},
{0x0F12, 0x0000},
{0x0F12, 0x4778},
{0x0F12, 0x46C0},
{0x0F12, 0xC000},
{0x0F12, 0xE59F},
{0x0F12, 0xFF1C},
{0x0F12, 0xE12F},
{0x0F12, 0x2001},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0xE848},
{0x0F12, 0x0001},
{0x0F12, 0xE848},
{0x0F12, 0x0001},
{0x0F12, 0x0500},
{0x0F12, 0x0064},
{0x0F12, 0x0002},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
//Parameters Defined in T&P:
//REG_SF_USER_IsoVal                        2 700003EE SHORT
//REG_SF_USER_IsoChanged                    2 700003F0 SHORT
//AWBBTune_EVT4                            20 7000235C STRUCT
//AWBBTune_EVT4_uMinCoarse                  2 7000235C SHORT
//AWBBTune_EVT4_uMinFine                    2 7000235E SHORT
//AWBBTune_EVT4_uMaxExp3                    4 70002360 LONG
//AWBBTune_EVT4_uCapMaxExp3                 4 70002364 LONG
//AWBBTune_EVT4_uMaxAnGain3                 2 70002368 SHORT
//AWBBTune_EVT4_uMinFinalPt                 2 7000236A SHORT
//AWBBTune_EVT4_uInitPostToleranceCnt       2 7000236C SHORT
//AWBB_Mon_EVT3                             4 70002370 STRUCT
//AWBB_Mon_EVT3_uPostToleranceCnt           2 70002370 SHORT
//AWBB_Mon_EVT3_usIsoFixedDigitalGain88     2 70002372 SHORT
//End T&P part


//================================================================
// Analog Setting
//================================================================
{0x0004, 0x0000},	//Disable Auto Address Increment : 0
{0xF454, 0x0001},	//ADC sat = 750mV(50h), NTG = -0.8V(10h), Saturation margin low limit = 732LSB
{0xF418, 0x0050},	//aig_adc_sat[7:4]
{0xF43E, 0x0010},	//aig_reg_tune_ntg[7:0]
{0x0004, 0x0001},	//Disable Auto Address Increment : 1
{0x002A, 0x112A},	//senHal_SenRegsModes3_pSenModesRegsArray3[8]
{0x0F12, 0x0000},
{0x002A, 0x1132},	//senHal_SenRegsModes3_pSenModesRegsArray3[12]
{0x0F12, 0x0000},
{0x002A, 0x113E},	//senHal_SenRegsModes3_pSenModesRegsArray3[18]
{0x0F12, 0x0000},
{0x002A, 0x115C},	//senHal_SenRegsModes3_pSenModesRegsArray3[33]
{0x0F12, 0x0000},
{0x002A, 0x1164},	//senHal_SenRegsModes3_pSenModesRegsArray3[37]
{0x0F12, 0x0000},
{0x002A, 0x1174},	//senHal_SenRegsModes3_pSenModesRegsArray3[45]
{0x0F12, 0x0000},
{0x002A, 0x1178},	//senHal_SenRegsModes3_pSenModesRegsArray3[47]
{0x0F12, 0x0000},
{0x002A, 0x077A},	//msm_uOffsetNoBin
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x002A, 0x07A2},	//msm_sAnalogOffset
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x002A, 0x07B6},	//msm_NonLinearOfsOutput
{0x0F12, 0x0000},	//msm_NonLinearOfsOutput[0]
{0x0F12, 0x0002},	//msm_NonLinearOfsOutput[1]
{0x0F12, 0x0004},	//msm_NonLinearOfsOutput[2]
{0x0F12, 0x0004},	//msm_NonLinearOfsOutput[3]
{0x0F12, 0x0005},	//msm_NonLinearOfsOutput[4]
{0x0F12, 0x0005},	//msm_NonLinearOfsOutput[5]              


//================================================================
// ESD Check Code add_100505
//================================================================
{0x0028, 0x7000},
{0x002A, 0x0132},
{0x0F12, 0xAAAA},	//REG_FWpid


//================================================================
// AE & AE Weight
//================================================================
{0x002A, 0x1000},	//TVAR_ae_BrAve
{0x0F12, 0x0034},	//35

{0x002A, 0x0474},
{0x0F12, 0x0112},	//lt_uLimitHigh  //010F  //0114
{0x0F12, 0x00EF},	//lt_uLimitLow   //00F1  //00F9

{0x002A, 0x1006},
{0x0F12, 0x001F},	//ae_StatMode

{0x002A, 0x108E},	//SARR_IllumType
{0x0F12, 0x00C7},
{0x0F12, 0x00F7},
{0x0F12, 0x0107},
{0x0F12, 0x0142},
{0x0F12, 0x017A},
{0x0F12, 0x01A0},
{0x0F12, 0x01B6},

{0x0F12, 0x0100},	//SARR_IllumTypeF	// 0112
{0x0F12, 0x0100},	//0122
{0x0F12, 0x0100},	//0136
{0x0F12, 0x0100},	//00F6
{0x0F12, 0x0100},	//0100
{0x0F12, 0x0100},	//00FE
{0x0F12, 0x0100},	//0100
{0x002A, 0x0488},

{0x0F12, 0x410A},	//416E //33.3m  //lt_uMaxExp1
{0x0F12, 0x0000},
{0x0F12, 0xA316},	//lt_uMaxExp2
{0x0F12, 0x0000},
{0x002A, 0x2360},	//AWBBTune_EVT4_uMaxExp3
{0x0F12, 0xF424},
{0x0F12, 0x0000},

{0x002A, 0x0490},	//lt_uCapMaxExp1
{0x0F12, 0x410A},	//416E // 33.3m
{0x0F12, 0x0000},
{0x0F12, 0xA316},	//lt_uCapMaxExp2
{0x0F12, 0x0000},
{0x002A, 0x2364},	//AWBBTune_EVT4_uCapMaxExp3
{0x0F12, 0xF424},
{0x0F12, 0x0000},

{0x002A, 0x0498},
{0x0F12, 0x0210},	//01E8	//lt_uMaxAnGain1       700luxshutter
{0x0F12, 0x0330},	//lt_uMaxAnGain2
{0x002A, 0x2368},
{0x0F12, 0x06d0},	//700//800//900//990//A00	//AWBBTune_EVT4_uMaxAnGain3
{0x002A, 0x049C},
{0x0F12, 0x0100},	//lt_uMaxDigGain

{0x002A, 0x235C},
{0x0F12, 0x0002},	//0001	//AWBBTune_EVT4_uMinCoarse
{0x0F12, 0x0090},	//AWBBTune_EVT4_uMinFine


//================================================================
// AE WeightTable
//================================================================
{0x002A, 0x1C72},	//ae_WeightTbl_16
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0302},
{0x0F12, 0x0203},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0403},
{0x0F12, 0x0304},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0403},
{0x0F12, 0x0304},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0302},
{0x0F12, 0x0203},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0101},
{0x0F12, 0x0000},
{0x0F12, 0x0000},
{0x0F12, 0x0000},

{0x002A, 0x0F4C},	//brightness
{0x0F12, 0x02B0},	//180
{0x002A, 0x0F52},
{0x0F12, 0x02F0},	//180   
   

//================================================================
// GAS (Shading)
//================================================================
{0x002A, 0x0754},	//TVAR_ash_pGAS
{0x0F12, 0x247C},
{0x0F12, 0x7000},
{0x002A, 0x247C},	
{0x0F12, 0x01D2},	//TVAR_ash_pGAS[0]
{0x0F12, 0x0186},	//TVAR_ash_pGAS[1]
{0x0F12, 0x012A},	//TVAR_ash_pGAS[2]
{0x0F12, 0x00E6},	//TVAR_ash_pGAS[3]
{0x0F12, 0x00BC},	//TVAR_ash_pGAS[4]
{0x0F12, 0x00A2},	//TVAR_ash_pGAS[5]
{0x0F12, 0x009B},	//TVAR_ash_pGAS[6]
{0x0F12, 0x00A1},	//TVAR_ash_pGAS[7]
{0x0F12, 0x00B7},	//TVAR_ash_pGAS[8]
{0x0F12, 0x00E2},	//TVAR_ash_pGAS[9]
{0x0F12, 0x0122},	//TVAR_ash_pGAS[10]
{0x0F12, 0x017E},	//TVAR_ash_pGAS[11]
{0x0F12, 0x01BF},	//TVAR_ash_pGAS[12]
{0x0F12, 0x019C},	//TVAR_ash_pGAS[13]
{0x0F12, 0x013C},	//TVAR_ash_pGAS[14]
{0x0F12, 0x00E1},	//TVAR_ash_pGAS[15]
{0x0F12, 0x00A7},	//TVAR_ash_pGAS[16]
{0x0F12, 0x0082},	//TVAR_ash_pGAS[17]
{0x0F12, 0x006D},	//TVAR_ash_pGAS[18]
{0x0F12, 0x0065},	//TVAR_ash_pGAS[19]
{0x0F12, 0x006B},	//TVAR_ash_pGAS[20]
{0x0F12, 0x007E},	//TVAR_ash_pGAS[21]
{0x0F12, 0x00A1},	//TVAR_ash_pGAS[22]
{0x0F12, 0x00DB},	//TVAR_ash_pGAS[23]
{0x0F12, 0x0133},	//TVAR_ash_pGAS[24]
{0x0F12, 0x018B},	//TVAR_ash_pGAS[25]
{0x0F12, 0x015A},	//TVAR_ash_pGAS[26]
{0x0F12, 0x00FA},	//TVAR_ash_pGAS[27]
{0x0F12, 0x00A7},	//TVAR_ash_pGAS[28]
{0x0F12, 0x0075},	//TVAR_ash_pGAS[29]
{0x0F12, 0x0053},	//TVAR_ash_pGAS[30]
{0x0F12, 0x0040},	//TVAR_ash_pGAS[31]
{0x0F12, 0x0038},	//TVAR_ash_pGAS[32]
{0x0F12, 0x003D},	//TVAR_ash_pGAS[33]
{0x0F12, 0x004F},	//TVAR_ash_pGAS[34]
{0x0F12, 0x006D},	//TVAR_ash_pGAS[35]
{0x0F12, 0x009F},	//TVAR_ash_pGAS[36]
{0x0F12, 0x00EE},	//TVAR_ash_pGAS[37]
{0x0F12, 0x0147},	//TVAR_ash_pGAS[38]
{0x0F12, 0x0129},	//TVAR_ash_pGAS[39]
{0x0F12, 0x00CE},	//TVAR_ash_pGAS[40]
{0x0F12, 0x0085},	//TVAR_ash_pGAS[41]
{0x0F12, 0x0055},	//TVAR_ash_pGAS[42]
{0x0F12, 0x0035},	//TVAR_ash_pGAS[43]
{0x0F12, 0x0020},	//TVAR_ash_pGAS[44]
{0x0F12, 0x0018},	//TVAR_ash_pGAS[45]
{0x0F12, 0x001E},	//TVAR_ash_pGAS[46]
{0x0F12, 0x0030},	//TVAR_ash_pGAS[47]
{0x0F12, 0x004F},	//TVAR_ash_pGAS[48]
{0x0F12, 0x007B},	//TVAR_ash_pGAS[49]
{0x0F12, 0x00C3},	//TVAR_ash_pGAS[50]
{0x0F12, 0x0119},	//TVAR_ash_pGAS[51]
{0x0F12, 0x010E},	//TVAR_ash_pGAS[52]
{0x0F12, 0x00B8},	//TVAR_ash_pGAS[53]
{0x0F12, 0x0072},	//TVAR_ash_pGAS[54]
{0x0F12, 0x0044},	//TVAR_ash_pGAS[55]
{0x0F12, 0x0023},	//TVAR_ash_pGAS[56]
{0x0F12, 0x000E},	//TVAR_ash_pGAS[57]
{0x0F12, 0x0005},	//TVAR_ash_pGAS[58]
{0x0F12, 0x000C},	//TVAR_ash_pGAS[59]
{0x0F12, 0x001F},	//TVAR_ash_pGAS[60]
{0x0F12, 0x003E},	//TVAR_ash_pGAS[61]
{0x0F12, 0x0069},	//TVAR_ash_pGAS[62]
{0x0F12, 0x00AC},	//TVAR_ash_pGAS[63]
{0x0F12, 0x0101},	//TVAR_ash_pGAS[64]
{0x0F12, 0x0107},	//TVAR_ash_pGAS[65]
{0x0F12, 0x00B2},	//TVAR_ash_pGAS[66]
{0x0F12, 0x006F},	//TVAR_ash_pGAS[67]
{0x0F12, 0x0040},	//TVAR_ash_pGAS[68]
{0x0F12, 0x001E},	//TVAR_ash_pGAS[69]
{0x0F12, 0x0008},	//TVAR_ash_pGAS[70]
{0x0F12, 0x0000},	//TVAR_ash_pGAS[71]
{0x0F12, 0x0007},	//TVAR_ash_pGAS[72]
{0x0F12, 0x001B},	//TVAR_ash_pGAS[73]
{0x0F12, 0x003A},	//TVAR_ash_pGAS[74]
{0x0F12, 0x0065},	//TVAR_ash_pGAS[75]
{0x0F12, 0x00A8},	//TVAR_ash_pGAS[76]
{0x0F12, 0x00FB},	//TVAR_ash_pGAS[77]
{0x0F12, 0x0115},	//TVAR_ash_pGAS[78]
{0x0F12, 0x00BC},	//TVAR_ash_pGAS[79]
{0x0F12, 0x0077},	//TVAR_ash_pGAS[80]
{0x0F12, 0x0048},	//TVAR_ash_pGAS[81]
{0x0F12, 0x0027},	//TVAR_ash_pGAS[82]
{0x0F12, 0x0011},	//TVAR_ash_pGAS[83]
{0x0F12, 0x0009},	//TVAR_ash_pGAS[84]
{0x0F12, 0x000F},	//TVAR_ash_pGAS[85]
{0x0F12, 0x0023},	//TVAR_ash_pGAS[86]
{0x0F12, 0x0043},	//TVAR_ash_pGAS[87]
{0x0F12, 0x0070},	//TVAR_ash_pGAS[88]
{0x0F12, 0x00B5},	//TVAR_ash_pGAS[89]
{0x0F12, 0x010A},	//TVAR_ash_pGAS[90]
{0x0F12, 0x0138},	//TVAR_ash_pGAS[91]
{0x0F12, 0x00D9},	//TVAR_ash_pGAS[92]
{0x0F12, 0x008E},	//TVAR_ash_pGAS[93]
{0x0F12, 0x005E},	//TVAR_ash_pGAS[94]
{0x0F12, 0x003C},	//TVAR_ash_pGAS[95]
{0x0F12, 0x0027},	//TVAR_ash_pGAS[96]
{0x0F12, 0x001F},	//TVAR_ash_pGAS[97]
{0x0F12, 0x0024},	//TVAR_ash_pGAS[98]
{0x0F12, 0x0038},	//TVAR_ash_pGAS[99]
{0x0F12, 0x0058},	//TVAR_ash_pGAS[100]
{0x0F12, 0x0088},	//TVAR_ash_pGAS[101]
{0x0F12, 0x00D3},	//TVAR_ash_pGAS[102]
{0x0F12, 0x012D},	//TVAR_ash_pGAS[103]
{0x0F12, 0x0171},	//TVAR_ash_pGAS[104]
{0x0F12, 0x010C},	//TVAR_ash_pGAS[105]
{0x0F12, 0x00B5},	//TVAR_ash_pGAS[106]
{0x0F12, 0x007F},	//TVAR_ash_pGAS[107]
{0x0F12, 0x005D},	//TVAR_ash_pGAS[108]
{0x0F12, 0x0048},	//TVAR_ash_pGAS[109]
{0x0F12, 0x0040},	//TVAR_ash_pGAS[110]
{0x0F12, 0x0045},	//TVAR_ash_pGAS[111]
{0x0F12, 0x0059},	//TVAR_ash_pGAS[112]
{0x0F12, 0x007B},	//TVAR_ash_pGAS[113]
{0x0F12, 0x00B1},	//TVAR_ash_pGAS[114]
{0x0F12, 0x0104},	//TVAR_ash_pGAS[115]
{0x0F12, 0x0166},	//TVAR_ash_pGAS[116]
{0x0F12, 0x01B9},	//TVAR_ash_pGAS[117]
{0x0F12, 0x0152},	//TVAR_ash_pGAS[118]
{0x0F12, 0x00F3},	//TVAR_ash_pGAS[119]
{0x0F12, 0x00B5},	//TVAR_ash_pGAS[120]
{0x0F12, 0x008C},	//TVAR_ash_pGAS[121]
{0x0F12, 0x0075},	//TVAR_ash_pGAS[122]
{0x0F12, 0x006E},	//TVAR_ash_pGAS[123]
{0x0F12, 0x0073},	//TVAR_ash_pGAS[124]
{0x0F12, 0x0089},	//TVAR_ash_pGAS[125]
{0x0F12, 0x00B2},	//TVAR_ash_pGAS[126]
{0x0F12, 0x00F1},	//TVAR_ash_pGAS[127]
{0x0F12, 0x014E},	//TVAR_ash_pGAS[128]
{0x0F12, 0x01A8},	//TVAR_ash_pGAS[129]
{0x0F12, 0x01E4},	//TVAR_ash_pGAS[130]
{0x0F12, 0x019F},	//TVAR_ash_pGAS[131]
{0x0F12, 0x013E},	//TVAR_ash_pGAS[132]
{0x0F12, 0x00F5},	//TVAR_ash_pGAS[133]
{0x0F12, 0x00C6},	//TVAR_ash_pGAS[134]
{0x0F12, 0x00AC},	//TVAR_ash_pGAS[135]
{0x0F12, 0x00A2},	//TVAR_ash_pGAS[136]
{0x0F12, 0x00AA},	//TVAR_ash_pGAS[137]
{0x0F12, 0x00C3},	//TVAR_ash_pGAS[138]
{0x0F12, 0x00F3},	//TVAR_ash_pGAS[139]
{0x0F12, 0x0138},	//TVAR_ash_pGAS[140]
{0x0F12, 0x0193},	//TVAR_ash_pGAS[141]
{0x0F12, 0x01DE},	//TVAR_ash_pGAS[142]
{0x0F12, 0x017C},	//TVAR_ash_pGAS[143]
{0x0F12, 0x0139},	//TVAR_ash_pGAS[144]
{0x0F12, 0x00EC},	//TVAR_ash_pGAS[145]
{0x0F12, 0x00B5},	//TVAR_ash_pGAS[146]
{0x0F12, 0x0092},	//TVAR_ash_pGAS[147]
{0x0F12, 0x007E},	//TVAR_ash_pGAS[148]
{0x0F12, 0x0077},	//TVAR_ash_pGAS[149]
{0x0F12, 0x007B},	//TVAR_ash_pGAS[150]
{0x0F12, 0x008A},	//TVAR_ash_pGAS[151]
{0x0F12, 0x00A7},	//TVAR_ash_pGAS[152]
{0x0F12, 0x00D8},	//TVAR_ash_pGAS[153]
{0x0F12, 0x011F},	//TVAR_ash_pGAS[154]
{0x0F12, 0x015A},	//TVAR_ash_pGAS[155]
{0x0F12, 0x014B},	//TVAR_ash_pGAS[156]
{0x0F12, 0x00F9},	//TVAR_ash_pGAS[157]
{0x0F12, 0x00AF},	//TVAR_ash_pGAS[158]
{0x0F12, 0x0081},	//TVAR_ash_pGAS[159]
{0x0F12, 0x0064},	//TVAR_ash_pGAS[160]
{0x0F12, 0x0054},	//TVAR_ash_pGAS[161]
{0x0F12, 0x004E},	//TVAR_ash_pGAS[162]
{0x0F12, 0x0050},	//TVAR_ash_pGAS[163]
{0x0F12, 0x005D},	//TVAR_ash_pGAS[164]
{0x0F12, 0x0075},	//TVAR_ash_pGAS[165]
{0x0F12, 0x009F},	//TVAR_ash_pGAS[166]
{0x0F12, 0x00E1},	//TVAR_ash_pGAS[167]
{0x0F12, 0x012C},	//TVAR_ash_pGAS[168]
{0x0F12, 0x0116},	//TVAR_ash_pGAS[169]
{0x0F12, 0x00C4},	//TVAR_ash_pGAS[170]
{0x0F12, 0x0084},	//TVAR_ash_pGAS[171]
{0x0F12, 0x005B},	//TVAR_ash_pGAS[172]
{0x0F12, 0x0042},	//TVAR_ash_pGAS[173]
{0x0F12, 0x0032},	//TVAR_ash_pGAS[174]
{0x0F12, 0x002B},	//TVAR_ash_pGAS[175]
{0x0F12, 0x002E},	//TVAR_ash_pGAS[176]
{0x0F12, 0x003A},	//TVAR_ash_pGAS[177]
{0x0F12, 0x004F},	//TVAR_ash_pGAS[178]
{0x0F12, 0x0072},	//TVAR_ash_pGAS[179]
{0x0F12, 0x00AD},	//TVAR_ash_pGAS[180]
{0x0F12, 0x00F8},	//TVAR_ash_pGAS[181]
{0x0F12, 0x00EC},	//TVAR_ash_pGAS[182]
{0x0F12, 0x00A2},	//TVAR_ash_pGAS[183]
{0x0F12, 0x0069},	//TVAR_ash_pGAS[184]
{0x0F12, 0x0044},	//TVAR_ash_pGAS[185]
{0x0F12, 0x002A},	//TVAR_ash_pGAS[186]
{0x0F12, 0x0019},	//TVAR_ash_pGAS[187]
{0x0F12, 0x0012},	//TVAR_ash_pGAS[188]
{0x0F12, 0x0015},	//TVAR_ash_pGAS[189]
{0x0F12, 0x0022},	//TVAR_ash_pGAS[190]
{0x0F12, 0x0037},	//TVAR_ash_pGAS[191]
{0x0F12, 0x0057},	//TVAR_ash_pGAS[192]
{0x0F12, 0x008C},	//TVAR_ash_pGAS[193]
{0x0F12, 0x00D2},	//TVAR_ash_pGAS[194]
{0x0F12, 0x00D6},	//TVAR_ash_pGAS[195]
{0x0F12, 0x0090},	//TVAR_ash_pGAS[196]
{0x0F12, 0x005A},	//TVAR_ash_pGAS[197]
{0x0F12, 0x0036},	//TVAR_ash_pGAS[198]
{0x0F12, 0x001C},	//TVAR_ash_pGAS[199]
{0x0F12, 0x000A},	//TVAR_ash_pGAS[200]
{0x0F12, 0x0003},	//TVAR_ash_pGAS[201]
{0x0F12, 0x0007},	//TVAR_ash_pGAS[202]
{0x0F12, 0x0015},	//TVAR_ash_pGAS[203]
{0x0F12, 0x002B},	//TVAR_ash_pGAS[204]
{0x0F12, 0x004A},	//TVAR_ash_pGAS[205]
{0x0F12, 0x007C},	//TVAR_ash_pGAS[206]
{0x0F12, 0x00BF},	//TVAR_ash_pGAS[207]
{0x0F12, 0x00CF},	//TVAR_ash_pGAS[208]
{0x0F12, 0x008C},	//TVAR_ash_pGAS[209]
{0x0F12, 0x0057},	//TVAR_ash_pGAS[210]
{0x0F12, 0x0033},	//TVAR_ash_pGAS[211]
{0x0F12, 0x0019},	//TVAR_ash_pGAS[212]
{0x0F12, 0x0007},	//TVAR_ash_pGAS[213]
{0x0F12, 0x0000},	//TVAR_ash_pGAS[214]
{0x0F12, 0x0004},	//TVAR_ash_pGAS[215]
{0x0F12, 0x0012},	//TVAR_ash_pGAS[216]
{0x0F12, 0x0029},	//TVAR_ash_pGAS[217]
{0x0F12, 0x0048},	//TVAR_ash_pGAS[218]
{0x0F12, 0x007A},	//TVAR_ash_pGAS[219]
{0x0F12, 0x00BC},	//TVAR_ash_pGAS[220]
{0x0F12, 0x00DB},	//TVAR_ash_pGAS[221]
{0x0F12, 0x0093},	//TVAR_ash_pGAS[222]
{0x0F12, 0x005E},	//TVAR_ash_pGAS[223]
{0x0F12, 0x003B},	//TVAR_ash_pGAS[224]
{0x0F12, 0x0021},	//TVAR_ash_pGAS[225]
{0x0F12, 0x000F},	//TVAR_ash_pGAS[226]
{0x0F12, 0x0008},	//TVAR_ash_pGAS[227]
{0x0F12, 0x000C},	//TVAR_ash_pGAS[228]
{0x0F12, 0x001B},	//TVAR_ash_pGAS[229]
{0x0F12, 0x0031},	//TVAR_ash_pGAS[230]
{0x0F12, 0x0052},	//TVAR_ash_pGAS[231]
{0x0F12, 0x0084},	//TVAR_ash_pGAS[232]
{0x0F12, 0x00C8},	//TVAR_ash_pGAS[233]
{0x0F12, 0x00F8},	//TVAR_ash_pGAS[234]
{0x0F12, 0x00AB},	//TVAR_ash_pGAS[235]
{0x0F12, 0x0071},	//TVAR_ash_pGAS[236]
{0x0F12, 0x004D},	//TVAR_ash_pGAS[237]
{0x0F12, 0x0034},	//TVAR_ash_pGAS[238]
{0x0F12, 0x0023},	//TVAR_ash_pGAS[239]
{0x0F12, 0x001C},	//TVAR_ash_pGAS[240]
{0x0F12, 0x0020},	//TVAR_ash_pGAS[241]
{0x0F12, 0x002E},	//TVAR_ash_pGAS[242]
{0x0F12, 0x0044},	//TVAR_ash_pGAS[243]
{0x0F12, 0x0066},	//TVAR_ash_pGAS[244]
{0x0F12, 0x009D},	//TVAR_ash_pGAS[245]
{0x0F12, 0x00E7},	//TVAR_ash_pGAS[246]
{0x0F12, 0x0125},	//TVAR_ash_pGAS[247]
{0x0F12, 0x00D2},	//TVAR_ash_pGAS[248]
{0x0F12, 0x0090},	//TVAR_ash_pGAS[249]
{0x0F12, 0x0069},	//TVAR_ash_pGAS[250]
{0x0F12, 0x004F},	//TVAR_ash_pGAS[251]
{0x0F12, 0x003F},	//TVAR_ash_pGAS[252]
{0x0F12, 0x0038},	//TVAR_ash_pGAS[253]
{0x0F12, 0x003C},	//TVAR_ash_pGAS[254]
{0x0F12, 0x0049},	//TVAR_ash_pGAS[255]
{0x0F12, 0x0061},	//TVAR_ash_pGAS[256]
{0x0F12, 0x0087},	//TVAR_ash_pGAS[257]
{0x0F12, 0x00C4},	//TVAR_ash_pGAS[258]
{0x0F12, 0x0114},	//TVAR_ash_pGAS[259]
{0x0F12, 0x0162},	//TVAR_ash_pGAS[260]
{0x0F12, 0x010C},	//TVAR_ash_pGAS[261]
{0x0F12, 0x00C3},	//TVAR_ash_pGAS[262]
{0x0F12, 0x0093},	//TVAR_ash_pGAS[263]
{0x0F12, 0x0074},	//TVAR_ash_pGAS[264]
{0x0F12, 0x0064},	//TVAR_ash_pGAS[265]
{0x0F12, 0x005D},	//TVAR_ash_pGAS[266]
{0x0F12, 0x0061},	//TVAR_ash_pGAS[267]
{0x0F12, 0x0071},	//TVAR_ash_pGAS[268]
{0x0F12, 0x008C},	//TVAR_ash_pGAS[269]
{0x0F12, 0x00BB},	//TVAR_ash_pGAS[270]
{0x0F12, 0x0102},	//TVAR_ash_pGAS[271]
{0x0F12, 0x014D},	//TVAR_ash_pGAS[272]
{0x0F12, 0x018A},	//TVAR_ash_pGAS[273]
{0x0F12, 0x014F},	//TVAR_ash_pGAS[274]
{0x0F12, 0x00FE},	//TVAR_ash_pGAS[275]
{0x0F12, 0x00C5},	//TVAR_ash_pGAS[276]
{0x0F12, 0x00A2},	//TVAR_ash_pGAS[277]
{0x0F12, 0x008E},	//TVAR_ash_pGAS[278]
{0x0F12, 0x0087},	//TVAR_ash_pGAS[279]
{0x0F12, 0x008C},	//TVAR_ash_pGAS[280]
{0x0F12, 0x009E},	//TVAR_ash_pGAS[281]
{0x0F12, 0x00C0},	//TVAR_ash_pGAS[282]
{0x0F12, 0x00F5},	//TVAR_ash_pGAS[283]
{0x0F12, 0x013E},	//TVAR_ash_pGAS[284]
{0x0F12, 0x017D},	//TVAR_ash_pGAS[285]
{0x0F12, 0x0192},	//TVAR_ash_pGAS[286]
{0x0F12, 0x0148},	//TVAR_ash_pGAS[287]
{0x0F12, 0x00F5},	//TVAR_ash_pGAS[288]
{0x0F12, 0x00BA},	//TVAR_ash_pGAS[289]
{0x0F12, 0x0095},	//TVAR_ash_pGAS[290]
{0x0F12, 0x007F},	//TVAR_ash_pGAS[291]
{0x0F12, 0x0078},	//TVAR_ash_pGAS[292]
{0x0F12, 0x007C},	//TVAR_ash_pGAS[293]
{0x0F12, 0x008F},	//TVAR_ash_pGAS[294]
{0x0F12, 0x00B3},	//TVAR_ash_pGAS[295]
{0x0F12, 0x00E9},	//TVAR_ash_pGAS[296]
{0x0F12, 0x013B},	//TVAR_ash_pGAS[297]
{0x0F12, 0x017E},	//TVAR_ash_pGAS[298]
{0x0F12, 0x015F},	//TVAR_ash_pGAS[299]
{0x0F12, 0x0108},	//TVAR_ash_pGAS[300]
{0x0F12, 0x00B9},	//TVAR_ash_pGAS[301]
{0x0F12, 0x0087},	//TVAR_ash_pGAS[302]
{0x0F12, 0x0069},	//TVAR_ash_pGAS[303]
{0x0F12, 0x0056},	//TVAR_ash_pGAS[304]
{0x0F12, 0x004F},	//TVAR_ash_pGAS[305]
{0x0F12, 0x0053},	//TVAR_ash_pGAS[306]
{0x0F12, 0x0062},	//TVAR_ash_pGAS[307]
{0x0F12, 0x007F},	//TVAR_ash_pGAS[308]
{0x0F12, 0x00AE},	//TVAR_ash_pGAS[309]
{0x0F12, 0x00F8},	//TVAR_ash_pGAS[310]
{0x0F12, 0x014C},	//TVAR_ash_pGAS[311]
{0x0F12, 0x0129},	//TVAR_ash_pGAS[312]
{0x0F12, 0x00D4},	//TVAR_ash_pGAS[313]
{0x0F12, 0x008F},	//TVAR_ash_pGAS[314]
{0x0F12, 0x0063},	//TVAR_ash_pGAS[315]
{0x0F12, 0x0047},	//TVAR_ash_pGAS[316]
{0x0F12, 0x0034},	//TVAR_ash_pGAS[317]
{0x0F12, 0x002D},	//TVAR_ash_pGAS[318]
{0x0F12, 0x0030},	//TVAR_ash_pGAS[319]
{0x0F12, 0x003E},	//TVAR_ash_pGAS[320]
{0x0F12, 0x0057},	//TVAR_ash_pGAS[321]
{0x0F12, 0x007F},	//TVAR_ash_pGAS[322]
{0x0F12, 0x00BF},	//TVAR_ash_pGAS[323]
{0x0F12, 0x0111},	//TVAR_ash_pGAS[324]
{0x0F12, 0x0100},	//TVAR_ash_pGAS[325]
{0x0F12, 0x00B1},	//TVAR_ash_pGAS[326]
{0x0F12, 0x0074},	//TVAR_ash_pGAS[327]
{0x0F12, 0x004C},	//TVAR_ash_pGAS[328]
{0x0F12, 0x002F},	//TVAR_ash_pGAS[329]
{0x0F12, 0x001C},	//TVAR_ash_pGAS[330]
{0x0F12, 0x0013},	//TVAR_ash_pGAS[331]
{0x0F12, 0x0017},	//TVAR_ash_pGAS[332]
{0x0F12, 0x0026},	//TVAR_ash_pGAS[333]
{0x0F12, 0x003E},	//TVAR_ash_pGAS[334]
{0x0F12, 0x0062},	//TVAR_ash_pGAS[335]
{0x0F12, 0x0099},	//TVAR_ash_pGAS[336]
{0x0F12, 0x00E5},	//TVAR_ash_pGAS[337]
{0x0F12, 0x00E6},	//TVAR_ash_pGAS[338]
{0x0F12, 0x009F},	//TVAR_ash_pGAS[339]
{0x0F12, 0x0066},	//TVAR_ash_pGAS[340]
{0x0F12, 0x003F},	//TVAR_ash_pGAS[341]
{0x0F12, 0x0021},	//TVAR_ash_pGAS[342]
{0x0F12, 0x000D},	//TVAR_ash_pGAS[343]
{0x0F12, 0x0004},	//TVAR_ash_pGAS[344]
{0x0F12, 0x0008},	//TVAR_ash_pGAS[345]
{0x0F12, 0x0016},	//TVAR_ash_pGAS[346]
{0x0F12, 0x002F},	//TVAR_ash_pGAS[347]
{0x0F12, 0x0050},	//TVAR_ash_pGAS[348]
{0x0F12, 0x0085},	//TVAR_ash_pGAS[349]
{0x0F12, 0x00CA},	//TVAR_ash_pGAS[350]
{0x0F12, 0x00E2},	//TVAR_ash_pGAS[351]
{0x0F12, 0x009A},	//TVAR_ash_pGAS[352]
{0x0F12, 0x0062},	//TVAR_ash_pGAS[353]
{0x0F12, 0x003B},	//TVAR_ash_pGAS[354]
{0x0F12, 0x001D},	//TVAR_ash_pGAS[355]
{0x0F12, 0x0009},	//TVAR_ash_pGAS[356]
{0x0F12, 0x0000},	//TVAR_ash_pGAS[357]
{0x0F12, 0x0003},	//TVAR_ash_pGAS[358]
{0x0F12, 0x0011},	//TVAR_ash_pGAS[359]
{0x0F12, 0x0029},	//TVAR_ash_pGAS[360]
{0x0F12, 0x0049},	//TVAR_ash_pGAS[361]
{0x0F12, 0x007D},	//TVAR_ash_pGAS[362]
{0x0F12, 0x00C0},	//TVAR_ash_pGAS[363]
{0x0F12, 0x00EE},	//TVAR_ash_pGAS[364]
{0x0F12, 0x00A1},	//TVAR_ash_pGAS[365]
{0x0F12, 0x0069},	//TVAR_ash_pGAS[366]
{0x0F12, 0x0043},	//TVAR_ash_pGAS[367]
{0x0F12, 0x0025},	//TVAR_ash_pGAS[368]
{0x0F12, 0x0010},	//TVAR_ash_pGAS[369]
{0x0F12, 0x0007},	//TVAR_ash_pGAS[370]
{0x0F12, 0x000A},	//TVAR_ash_pGAS[371]
{0x0F12, 0x0017},	//TVAR_ash_pGAS[372]
{0x0F12, 0x002E},	//TVAR_ash_pGAS[373]
{0x0F12, 0x004F},	//TVAR_ash_pGAS[374]
{0x0F12, 0x0083},	//TVAR_ash_pGAS[375]
{0x0F12, 0x00C7},	//TVAR_ash_pGAS[376]
{0x0F12, 0x010B},	//TVAR_ash_pGAS[377]
{0x0F12, 0x00BA},	//TVAR_ash_pGAS[378]
{0x0F12, 0x007B},	//TVAR_ash_pGAS[379]
{0x0F12, 0x0054},	//TVAR_ash_pGAS[380]
{0x0F12, 0x0037},	//TVAR_ash_pGAS[381]
{0x0F12, 0x0023},	//TVAR_ash_pGAS[382]
{0x0F12, 0x001A},	//TVAR_ash_pGAS[383]
{0x0F12, 0x001B},	//TVAR_ash_pGAS[384]
{0x0F12, 0x0027},	//TVAR_ash_pGAS[385]
{0x0F12, 0x003D},	//TVAR_ash_pGAS[386]
{0x0F12, 0x005F},	//TVAR_ash_pGAS[387]
{0x0F12, 0x0095},	//TVAR_ash_pGAS[388]
{0x0F12, 0x00DE},	//TVAR_ash_pGAS[389]
{0x0F12, 0x013A},	//TVAR_ash_pGAS[390]
{0x0F12, 0x00E1},	//TVAR_ash_pGAS[391]
{0x0F12, 0x009C},	//TVAR_ash_pGAS[392]
{0x0F12, 0x006F},	//TVAR_ash_pGAS[393]
{0x0F12, 0x0053},	//TVAR_ash_pGAS[394]
{0x0F12, 0x003F},	//TVAR_ash_pGAS[395]
{0x0F12, 0x0035},	//TVAR_ash_pGAS[396]
{0x0F12, 0x0036},	//TVAR_ash_pGAS[397]
{0x0F12, 0x0042},	//TVAR_ash_pGAS[398]
{0x0F12, 0x0058},	//TVAR_ash_pGAS[399]
{0x0F12, 0x007D},	//TVAR_ash_pGAS[400]
{0x0F12, 0x00B9},	//TVAR_ash_pGAS[401]
{0x0F12, 0x0105},	//TVAR_ash_pGAS[402]
{0x0F12, 0x0177},	//TVAR_ash_pGAS[403]
{0x0F12, 0x011D},	//TVAR_ash_pGAS[404]
{0x0F12, 0x00CE},	//TVAR_ash_pGAS[405]
{0x0F12, 0x009A},	//TVAR_ash_pGAS[406]
{0x0F12, 0x0078},	//TVAR_ash_pGAS[407]
{0x0F12, 0x0064},	//TVAR_ash_pGAS[408]
{0x0F12, 0x005B},	//TVAR_ash_pGAS[409]
{0x0F12, 0x005C},	//TVAR_ash_pGAS[410]
{0x0F12, 0x0068},	//TVAR_ash_pGAS[411]
{0x0F12, 0x0082},	//TVAR_ash_pGAS[412]
{0x0F12, 0x00AD},	//TVAR_ash_pGAS[413]
{0x0F12, 0x00F1},	//TVAR_ash_pGAS[414]
{0x0F12, 0x0137},	//TVAR_ash_pGAS[415]
{0x0F12, 0x019F},	//TVAR_ash_pGAS[416]
{0x0F12, 0x0160},	//TVAR_ash_pGAS[417]
{0x0F12, 0x010D},	//TVAR_ash_pGAS[418]
{0x0F12, 0x00CF},	//TVAR_ash_pGAS[419]
{0x0F12, 0x00A7},	//TVAR_ash_pGAS[420]
{0x0F12, 0x0090},	//TVAR_ash_pGAS[421]
{0x0F12, 0x0085},	//TVAR_ash_pGAS[422]
{0x0F12, 0x0087},	//TVAR_ash_pGAS[423]
{0x0F12, 0x0097},	//TVAR_ash_pGAS[424]
{0x0F12, 0x00B6},	//TVAR_ash_pGAS[425]
{0x0F12, 0x00E7},	//TVAR_ash_pGAS[426]
{0x0F12, 0x0129},	//TVAR_ash_pGAS[427]
{0x0F12, 0x0166},	//TVAR_ash_pGAS[428]
{0x0F12, 0x013A},	//TVAR_ash_pGAS[429]
{0x0F12, 0x0102},	//TVAR_ash_pGAS[430]
{0x0F12, 0x00C1},	//TVAR_ash_pGAS[431]
{0x0F12, 0x0092},	//TVAR_ash_pGAS[432]
{0x0F12, 0x0075},	//TVAR_ash_pGAS[433]
{0x0F12, 0x0065},	//TVAR_ash_pGAS[434]
{0x0F12, 0x0061},	//TVAR_ash_pGAS[435]
{0x0F12, 0x0068},	//TVAR_ash_pGAS[436]
{0x0F12, 0x007A},	//TVAR_ash_pGAS[437]
{0x0F12, 0x009C},	//TVAR_ash_pGAS[438]
{0x0F12, 0x00CD},	//TVAR_ash_pGAS[439]
{0x0F12, 0x0116},	//TVAR_ash_pGAS[440]
{0x0F12, 0x014D},	//TVAR_ash_pGAS[441]
{0x0F12, 0x0116},	//TVAR_ash_pGAS[442]
{0x0F12, 0x00CE},	//TVAR_ash_pGAS[443]
{0x0F12, 0x008E},	//TVAR_ash_pGAS[444]
{0x0F12, 0x0068},	//TVAR_ash_pGAS[445]
{0x0F12, 0x0050},	//TVAR_ash_pGAS[446]
{0x0F12, 0x0044},	//TVAR_ash_pGAS[447]
{0x0F12, 0x0040},	//TVAR_ash_pGAS[448]
{0x0F12, 0x0045},	//TVAR_ash_pGAS[449]
{0x0F12, 0x0054},	//TVAR_ash_pGAS[450]
{0x0F12, 0x006F},	//TVAR_ash_pGAS[451]
{0x0F12, 0x009A},	//TVAR_ash_pGAS[452]
{0x0F12, 0x00DC},	//TVAR_ash_pGAS[453]
{0x0F12, 0x0127},	//TVAR_ash_pGAS[454]
{0x0F12, 0x00E5},	//TVAR_ash_pGAS[455]
{0x0F12, 0x00A0},	//TVAR_ash_pGAS[456]
{0x0F12, 0x0068},	//TVAR_ash_pGAS[457]
{0x0F12, 0x0047},	//TVAR_ash_pGAS[458]
{0x0F12, 0x0033},	//TVAR_ash_pGAS[459]
{0x0F12, 0x0027},	//TVAR_ash_pGAS[460]
{0x0F12, 0x0023},	//TVAR_ash_pGAS[461]
{0x0F12, 0x0028},	//TVAR_ash_pGAS[462]
{0x0F12, 0x0036},	//TVAR_ash_pGAS[463]
{0x0F12, 0x004B},	//TVAR_ash_pGAS[464]
{0x0F12, 0x006E},	//TVAR_ash_pGAS[465]
{0x0F12, 0x00A7},	//TVAR_ash_pGAS[466]
{0x0F12, 0x00EE},	//TVAR_ash_pGAS[467]
{0x0F12, 0x00C0},	//TVAR_ash_pGAS[468]
{0x0F12, 0x0081},	//TVAR_ash_pGAS[469]
{0x0F12, 0x0051},	//TVAR_ash_pGAS[470]
{0x0F12, 0x0034},	//TVAR_ash_pGAS[471]
{0x0F12, 0x0020},	//TVAR_ash_pGAS[472]
{0x0F12, 0x0014},	//TVAR_ash_pGAS[473]
{0x0F12, 0x000F},	//TVAR_ash_pGAS[474]
{0x0F12, 0x0014},	//TVAR_ash_pGAS[475]
{0x0F12, 0x0020},	//TVAR_ash_pGAS[476]
{0x0F12, 0x0034},	//TVAR_ash_pGAS[477]
{0x0F12, 0x0052},	//TVAR_ash_pGAS[478]
{0x0F12, 0x0083},	//TVAR_ash_pGAS[479]
{0x0F12, 0x00C3},	//TVAR_ash_pGAS[480]
{0x0F12, 0x00AD},	//TVAR_ash_pGAS[481]
{0x0F12, 0x0073},	//TVAR_ash_pGAS[482]
{0x0F12, 0x0046},	//TVAR_ash_pGAS[483]
{0x0F12, 0x002A},	//TVAR_ash_pGAS[484]
{0x0F12, 0x0016},	//TVAR_ash_pGAS[485]
{0x0F12, 0x0008},	//TVAR_ash_pGAS[486]
{0x0F12, 0x0004},	//TVAR_ash_pGAS[487]
{0x0F12, 0x0008},	//TVAR_ash_pGAS[488]
{0x0F12, 0x0014},	//TVAR_ash_pGAS[489]
{0x0F12, 0x0026},	//TVAR_ash_pGAS[490]
{0x0F12, 0x0040},	//TVAR_ash_pGAS[491]
{0x0F12, 0x006D},	//TVAR_ash_pGAS[492]
{0x0F12, 0x00AA},	//TVAR_ash_pGAS[493]
{0x0F12, 0x00A8},	//TVAR_ash_pGAS[494]
{0x0F12, 0x006F},	//TVAR_ash_pGAS[495]
{0x0F12, 0x0044},	//TVAR_ash_pGAS[496]
{0x0F12, 0x0027},	//TVAR_ash_pGAS[497]
{0x0F12, 0x0013},	//TVAR_ash_pGAS[498]
{0x0F12, 0x0005},	//TVAR_ash_pGAS[499]
{0x0F12, 0x0000},	//TVAR_ash_pGAS[500]
{0x0F12, 0x0004},	//TVAR_ash_pGAS[501]
{0x0F12, 0x000F},	//TVAR_ash_pGAS[502]
{0x0F12, 0x0020},	//TVAR_ash_pGAS[503]
{0x0F12, 0x003A},	//TVAR_ash_pGAS[504]
{0x0F12, 0x0065},	//TVAR_ash_pGAS[505]
{0x0F12, 0x009F},	//TVAR_ash_pGAS[506]
{0x0F12, 0x00B4},	//TVAR_ash_pGAS[507]
{0x0F12, 0x0076},	//TVAR_ash_pGAS[508]
{0x0F12, 0x004B},	//TVAR_ash_pGAS[509]
{0x0F12, 0x002E},	//TVAR_ash_pGAS[510]
{0x0F12, 0x001A},	//TVAR_ash_pGAS[511]
{0x0F12, 0x000D},	//TVAR_ash_pGAS[512]
{0x0F12, 0x0006},	//TVAR_ash_pGAS[513]
{0x0F12, 0x000A},	//TVAR_ash_pGAS[514]
{0x0F12, 0x0014},	//TVAR_ash_pGAS[515]
{0x0F12, 0x0024},	//TVAR_ash_pGAS[516]
{0x0F12, 0x003E},	//TVAR_ash_pGAS[517]
{0x0F12, 0x0069},	//TVAR_ash_pGAS[518]
{0x0F12, 0x00A3},	//TVAR_ash_pGAS[519]
{0x0F12, 0x00CE},	//TVAR_ash_pGAS[520]
{0x0F12, 0x008D},	//TVAR_ash_pGAS[521]
{0x0F12, 0x005D},	//TVAR_ash_pGAS[522]
{0x0F12, 0x003E},	//TVAR_ash_pGAS[523]
{0x0F12, 0x002C},	//TVAR_ash_pGAS[524]
{0x0F12, 0x001D},	//TVAR_ash_pGAS[525]
{0x0F12, 0x0017},	//TVAR_ash_pGAS[526]
{0x0F12, 0x0019},	//TVAR_ash_pGAS[527]
{0x0F12, 0x0022},	//TVAR_ash_pGAS[528]
{0x0F12, 0x0031},	//TVAR_ash_pGAS[529]
{0x0F12, 0x004C},	//TVAR_ash_pGAS[530]
{0x0F12, 0x007B},	//TVAR_ash_pGAS[531]
{0x0F12, 0x00B8},	//TVAR_ash_pGAS[532]
{0x0F12, 0x00F9},	//TVAR_ash_pGAS[533]
{0x0F12, 0x00B2},	//TVAR_ash_pGAS[534]
{0x0F12, 0x0079},	//TVAR_ash_pGAS[535]
{0x0F12, 0x0059},	//TVAR_ash_pGAS[536]
{0x0F12, 0x0044},	//TVAR_ash_pGAS[537]
{0x0F12, 0x0036},	//TVAR_ash_pGAS[538]
{0x0F12, 0x0030},	//TVAR_ash_pGAS[539]
{0x0F12, 0x0031},	//TVAR_ash_pGAS[540]
{0x0F12, 0x0039},	//TVAR_ash_pGAS[541]
{0x0F12, 0x004A},	//TVAR_ash_pGAS[542]
{0x0F12, 0x0068},	//TVAR_ash_pGAS[543]
{0x0F12, 0x009D},	//TVAR_ash_pGAS[544]
{0x0F12, 0x00DD},	//TVAR_ash_pGAS[545]
{0x0F12, 0x0131},	//TVAR_ash_pGAS[546]
{0x0F12, 0x00E9},	//TVAR_ash_pGAS[547]
{0x0F12, 0x00A9},	//TVAR_ash_pGAS[548]
{0x0F12, 0x0080},	//TVAR_ash_pGAS[549]
{0x0F12, 0x0066},	//TVAR_ash_pGAS[550]
{0x0F12, 0x0058},	//TVAR_ash_pGAS[551]
{0x0F12, 0x0052},	//TVAR_ash_pGAS[552]
{0x0F12, 0x0053},	//TVAR_ash_pGAS[553]
{0x0F12, 0x005E},	//TVAR_ash_pGAS[554]
{0x0F12, 0x0073},	//TVAR_ash_pGAS[555]
{0x0F12, 0x0097},	//TVAR_ash_pGAS[556]
{0x0F12, 0x00D2},	//TVAR_ash_pGAS[557]
{0x0F12, 0x0110},	//TVAR_ash_pGAS[558]
{0x0F12, 0x0150},	//TVAR_ash_pGAS[559]
{0x0F12, 0x0124},	//TVAR_ash_pGAS[560]
{0x0F12, 0x00E0},	//TVAR_ash_pGAS[561]
{0x0F12, 0x00B0},	//TVAR_ash_pGAS[562]
{0x0F12, 0x0090},	//TVAR_ash_pGAS[563]
{0x0F12, 0x007E},	//TVAR_ash_pGAS[564]
{0x0F12, 0x0077},	//TVAR_ash_pGAS[565]
{0x0F12, 0x0079},	//TVAR_ash_pGAS[566]
{0x0F12, 0x0087},	//TVAR_ash_pGAS[567]
{0x0F12, 0x00A2},	//TVAR_ash_pGAS[568]
{0x0F12, 0x00CD},	//TVAR_ash_pGAS[569]
{0x0F12, 0x0106},	//TVAR_ash_pGAS[570]
{0x0F12, 0x0137},	//TVAR_ash_pGAS[571]          


//================================================================
// Shading Alpha
//================================================================
{0x002A, 0x0704},
{0x0F12, 0x00ED},	//TVAR_ash_AwbAshCord[0]
{0x0F12, 0x0124},	//TVAR_ash_AwbAshCord[1]
{0x0F12, 0x012B},	//TVAR_ash_AwbAshCord[2]
{0x0F12, 0x014A},	//TVAR_ash_AwbAshCord[3]
{0x0F12, 0x0190},	//TVAR_ash_AwbAshCord[4]
{0x0F12, 0x01B2},	//TVAR_ash_AwbAshCord[5]
{0x0F12, 0x01C4},	//TVAR_ash_AwbAshCord[6]

{0x0F12, 0x012B},	//TVAR_ash_GASAlpha[0]
{0x0F12, 0x0106},	//TVAR_ash_GASAlpha[1]
{0x0F12, 0x010A},	//TVAR_ash_GASAlpha[2]
{0x0F12, 0x00EE},	//TVAR_ash_GASAlpha[3]
{0x0F12, 0x00C8},	//TVAR_ash_GASAlpha[4]
{0x0F12, 0x00FC},	//TVAR_ash_GASAlpha[5]
{0x0F12, 0x00FE},	//TVAR_ash_GASAlpha[6]
{0x0F12, 0x00A0},	//TVAR_ash_GASAlpha[7]
{0x0F12, 0x011B},	//TVAR_ash_GASAlpha[8]
{0x0F12, 0x0107},	//TVAR_ash_GASAlpha[9]
{0x0F12, 0x0109},	//TVAR_ash_GASAlpha[10]
{0x0F12, 0x00FF},	//TVAR_ash_GASAlpha[11]
{0x0F12, 0x00DB},	//TVAR_ash_GASAlpha[12]
{0x0F12, 0x00FF},	//TVAR_ash_GASAlpha[13]
{0x0F12, 0x0100},	//TVAR_ash_GASAlpha[14]
{0x0F12, 0x00FB},	//TVAR_ash_GASAlpha[15]
{0x0F12, 0x0100},	//TVAR_ash_GASAlpha[16]
{0x0F12, 0x0103},	//TVAR_ash_GASAlpha[17]
{0x0F12, 0x0101},	//TVAR_ash_GASAlpha[18]
{0x0F12, 0x0100},	//TVAR_ash_GASAlpha[19]
{0x0F12, 0x0110},	//TVAR_ash_GASAlpha[20]
{0x0F12, 0x00FD},	//TVAR_ash_GASAlpha[21]
{0x0F12, 0x00FD},	//TVAR_ash_GASAlpha[22]
{0x0F12, 0x0100},	//TVAR_ash_GASAlpha[23]
{0x0F12, 0x00D4},	//TVAR_ash_GASAlpha[24]
{0x0F12, 0x00FB},	//TVAR_ash_GASAlpha[25]
{0x0F12, 0x00F8},	//TVAR_ash_GASAlpha[26]
{0x0F12, 0x0100},	//TVAR_ash_GASAlpha[27]

{0x0F12, 0x00F5},	//TVAR_ash_GASOutdoorAlpha[0]
{0x0F12, 0x0103},	//TVAR_ash_GAsOutdoorAlpha[1]
{0x0F12, 0x0101},	//TVAR_ash_GAsOutdoorAlpha[2]
{0x0F12, 0x00E4},	//TVAR_ash_GAsOutdoorAlpha[3]

{0x002A, 0x075A},
{0x0F12, 0x0000},	//ash_bParabolicEstimation
{0x0F12, 0x0280},	//ash_uParabolicCenterX
{0x0F12, 0x0200},	//ash_uParabolicCenterY
{0x0F12, 0x000E},	//ash_uParabolicscalingA
{0x0F12, 0x000F},	//ash_uParabolicscalingB       

{0x002A, 0x04C8},
{0x0F12, 0x0000},	//0000 ////0000 ////0000 ////0000 ////0000 //	//0000  //0000 //0000                                                	//SARR_usGammaLutRGBIndoor_0__0_ 
{0x0F12, 0x0007},	//0007 ////0007 ////05 ////0005 ////0005 ////000a ////0D //	//0005  //0002 //0001                                	//SARR_usGammaLutRGBIndoor_0__1_ 
{0x0F12, 0x000D},	//000D ////000D ////0c ////000c ////000c ////10 ////0016 ////1B //	//000C  //0004 //0002                        	//SARR_usGammaLutRGBIndoor_0__2_ 
{0x0F12, 0x0013},	//0013 ////0012 ////0010 ////1f ////001f ////30 ////0035 ////3C //	//001F  //0009 //0004                        	//SARR_usGammaLutRGBIndoor_0__3_ 
{0x0F12, 0x0079},	//0079 ////0079 ////0079 ////75 ////81 ////0081 ////0081 //	//0056  //0020 //0070                                	//SARR_usGammaLutRGBIndoor_0__4_ 
{0x0F12, 0x00FE},	//00FE ////00FE ////00FE ////00FE ////00FE //	//00CD  //00B8 //00df                                                	//SARR_usGammaLutRGBIndoor_0__5_ 
{0x0F12, 0x0159},	//0159 ////0159 ////0159 ////0159 ////0159 //	//013A  //0139 //0139                                                	//SARR_usGammaLutRGBIndoor_0__6_ 
{0x0F12, 0x01A1},	//01A1 ////01A1 ////01A1 ////01A1 ////01A1 //	//018C  //0188 //017b                                                	//SARR_usGammaLutRGBIndoor_0__7_ 
{0x0F12, 0x0210},	//0210 ////0210 ////0210 ////0210 ////0210 //	//01FE  //01DF //01DF                                                	//SARR_usGammaLutRGBIndoor_0__8_ 
{0x0F12, 0x0263},	//0263 ////0263 ////0263 ////0263 ////0263 //	//0255  //022D //022D                                                	//SARR_usGammaLutRGBIndoor_0__9_ 
{0x0F12, 0x02D5},	//02D5 ////02D5 ////02D5 ////02D8 ////02D8 //	//02D1  //02B5 //02a2                                                	//SARR_usGammaLutRGBIndoor_0__10_
{0x0F12, 0x0330},	//0330 ////0330 ////0330 ////0338 ////0338 //	//0338  //0326 //0300                                                	//SARR_usGammaLutRGBIndoor_0__11_
{0x0F12, 0x0377},	//0377 ////0378 ////7A ////80 ////0380 ////0384 ////0384 //	//0384  //0376 //034c                                	//SARR_usGammaLutRGBIndoor_0__12_
{0x0F12, 0x03BE},	//00C0 ////03B9 ////03B4 ////03B5 ////03BD ////C0 ////3BC ////03BC //	//03BC  //03AC //038c                        	//SARR_usGammaLutRGBIndoor_0__13_
{0x0F12, 0x03F0},	//03ED ////03EC ////EB ////03EA ////E8 ////03EF ////3E8 ////03E8 //	//03E8  //03CA //03bc                        	//SARR_usGammaLutRGBIndoor_0__14_
{0x0F12, 0x0400},	//0400 ////0400 ////0400 ////0400 ////0400 //	//0400  //03DE //03e7                                                	//SARR_usGammaLutRGBIndoor_0__15_

{0x0F12, 0x0000},	//0000 ////0000 ////0000 ////0000 ////0000 ////0000 ////0000 ////0000 //	//0000 //	//0000  //0000 //0000	//SARR_usGammaLutRGBIndoor_1__0_ 
{0x0F12, 0x0007},	//0007 ////0007 ////0005 ////0005 ////0005 ////0005 ////0005 ////000a ////000D //	//0005  //0004 //0001        	//SARR_usGammaLutRGBIndoor_1__1_ 
{0x0F12, 0x000D},	//000D ////000D ////000c ////000c ////000c ////000c ////0010 ////0016 ////001B //	//000C  //0008 //0002        	//SARR_usGammaLutRGBIndoor_1__2_ 
{0x0F12, 0x0013},	//0013 ////0012 ////0012 ////0010 ////0010 ////1f ////001f ////0030 ////0035 ////003C //	//001F  //0010 //000a	//SARR_usGammaLutRGBIndoor_1__3_ 
{0x0F12, 0x0079},	//0079 ////0079 ////0079 ////0079 ////0075 ////81 ////0081 ////0081 //	//0056  //0035 //0085                        	//SARR_usGammaLutRGBIndoor_1__4_ 
{0x0F12, 0x00FE},	//00FE ////00FE ////00FE ////00FE ////00FE ////00FE ////00FE //	//00CD  //00C8 //00e1                                	//SARR_usGammaLutRGBIndoor_1__5_ 
{0x0F12, 0x0159},	//0159 ////0159 ////0159 ////0159 ////0159 ////0159 ////0159 //	//013A  //0139 //0139                                	//SARR_usGammaLutRGBIndoor_1__6_ 
{0x0F12, 0x01A1},	//01A1 ////01A1 ////01A1 ////01A1 ////01A1 ////01A1 ////01A1 //	//018C  //0188 //017b                                	//SARR_usGammaLutRGBIndoor_1__7_ 
{0x0F12, 0x0210},	//0210 ////0210 ////0210 ////0210 ////0210 ////0210 ////0210 //	//01FE  //01DF //01DF                                	//SARR_usGammaLutRGBIndoor_1__8_ 
{0x0F12, 0x0263},	//0263 ////0263 ////0263 ////0263 ////0263 ////0263 ////0263 //	//0255  //022D //022D                                	//SARR_usGammaLutRGBIndoor_1__9_ 
{0x0F12, 0x02D5},	//02D5 ////02D5 ////02D5 ////02D5 ////02D5 ////02D8 ////02D8 //	//02D1  //02B5 //02a2                                	//SARR_usGammaLutRGBIndoor_1__10_
{0x0F12, 0x0330},	//0330 ////0330 ////0330 ////0330 ////0330 ////0338 ////0338 //	//0338  //0326 //0300                                	//SARR_usGammaLutRGBIndoor_1__11_
{0x0F12, 0x0377},	//0377 ////0378 ////037A ////80 ////0380 ////0380 ////0384 ////0384 //	//0384  //0376 //034c                        	//SARR_usGammaLutRGBIndoor_1__12_
{0x0F12, 0x03BE},	//00C0 ////03B9 ////03B4 ////03B4 ////03B5 ////03BD ////C0 ////3BC ////03BC //	//03BC  //03AC //038c                	//SARR_usGammaLutRGBIndoor_1__13_
{0x0F12, 0x03F0},	//03ED ////03EC ////03EB ////03EA ////03E8 ////03EF ////3E8 ////03E8 //	//03E8  //03CA //03bc                        	//SARR_usGammaLutRGBIndoor_1__14_
{0x0F12, 0x0400},	//0400 ////0400 ////0400 ////0400 ////0400 ////0400 ////0400 //	//0400  //03DE //03e7                                	//SARR_usGammaLutRGBIndoor_1__15_

{0x0F12, 0x0000},	//0000 ////0000 ////0000 ////0000 ////0000 ////0000 ////0000 ////0000 //	//0000 //	//0000  //0000 //0000	//SARR_usGammaLutRGBIndoor_2__0_ 
{0x0F12, 0x0007},	//0007 ////0007 ////0005 ////0005 ////0005 ////0005 ////0005 ////000a ////000D //	//0005  //0004 //0001        	//SARR_usGammaLutRGBIndoor_2__1_ 
{0x0F12, 0x000D},	//000D ////000D ////000c ////000c ////000c ////000c ////0010 ////0016 ////001B //	//000C  //0008 //0002        	//SARR_usGammaLutRGBIndoor_2__2_ 
{0x0F12, 0x0013},	//0013 ////0012 ////0012 ////0010 ////0010 ////1f ////001f ////0030 ////0035 ////003C //	//001F  //0010 //000a	//SARR_usGammaLutRGBIndoor_2__3_ 
{0x0F12, 0x0079},	//0079 ////0079 ////0079 ////0079 ////0075 ////81 ////0081 ////0081 //	//0056  //0035 //0085                        	//SARR_usGammaLutRGBIndoor_2__4_ 
{0x0F12, 0x00FE},	//00FE ////00FE ////00FE ////00FE ////00FE ////00FE ////00FE //	//00CD  //00C8 //00e1                                	//SARR_usGammaLutRGBIndoor_2__5_ 
{0x0F12, 0x0159},	//0159 ////0159 ////0159 ////0159 ////0159 ////0159 ////0159 //	//013A  //0139 //0139                                	//SARR_usGammaLutRGBIndoor_2__6_ 
{0x0F12, 0x01A1},	//01A1 ////01A1 ////01A1 ////01A1 ////01A1 ////01A1 ////01A1 //	//018C  //0188 //017b                                	//SARR_usGammaLutRGBIndoor_2__7_ 
{0x0F12, 0x0210},	//0210 ////0210 ////0210 ////0210 ////0210 ////0210 ////0210 //	//01FE  //01DF //01DF                                	//SARR_usGammaLutRGBIndoor_2__8_ 
{0x0F12, 0x0263},	//0263 ////0263 ////0263 ////0263 ////0263 ////0263 ////0263 //	//0255  //022D //022D                                	//SARR_usGammaLutRGBIndoor_2__9_ 
{0x0F12, 0x02D5},	//02D5 ////02D5 ////02D5 ////02D5 ////02D5 ////02D8 ////02D8 //	//02D1  //02B5 //02a2                                	//SARR_usGammaLutRGBIndoor_2__10_
{0x0F12, 0x0330},	//0330 ////0330 ////0330 ////0330 ////0330 ////0338 ////0338 //	//0338  //0326 //0300                                	//SARR_usGammaLutRGBIndoor_2__11_
{0x0F12, 0x0377},	//0377 ////0378 ////037A ////80 ////0380 ////0380 ////0384 ////0384 //	//0384  //0376 //034c                        	//SARR_usGammaLutRGBIndoor_2__12_
{0x0F12, 0x03BE},	//00C0 ////03B9 ////03B4 ////03B4 ////03B5 ////03BD ////C0 ////3BC ////03BC //	//03BC  //03AC //038c                	//SARR_usGammaLutRGBIndoor_2__13_
{0x0F12, 0x03F0},	//03ED ////03EC ////03EB ////03EA ////03E8 ////03EF ////3E8 ////03E8 //	//03E8  //03CA //03bc                        	//SARR_usGammaLutRGBIndoor_2__14_
{0x0F12, 0x0400},	//0400 ////0400 ////0400 ////0400 ////0400 ////0400 ////0400 //	//0400  //03DE //03e7                                	//SARR_usGammaLutRGBIndoor_2__15_


//================================================================
// AWB
//================================================================
{0x002A, 0x0C50},	//awbb_IndoorGrZones_m_BGrid
{0x0F12, 0x03B8},	//03B8	//awbb_IndoorGrZones_m_Bgrid[0]	70000C50
{0x0F12, 0x03C8},	//03C8  //awbb_IndoorGrZones_m_Bgrid[0] 
{0x0F12, 0x0384},	//0384  //awbb_IndoorGrZones_m_Bgrid[1] 
{0x0F12, 0x03D0},	//03D0  //awbb_IndoorGrZones_m_Bgrid[1] 
{0x0F12, 0x035E},	//035E  //awbb_IndoorGrZones_m_Bgrid[2] 
{0x0F12, 0x03CC},	//03CC  //awbb_IndoorGrZones_m_Bgrid[2] 
{0x0F12, 0x033E},	//033E  //awbb_IndoorGrZones_m_Bgrid[3] 
{0x0F12, 0x03B2},	//03B2  //awbb_IndoorGrZones_m_Bgrid[3] 
{0x0F12, 0x0322},	//0322  //awbb_IndoorGrZones_m_Bgrid[4] 
{0x0F12, 0x0396},	//0396  //awbb_IndoorGrZones_m_Bgrid[4] 
{0x0F12, 0x030C},	//030C  //awbb_IndoorGrZones_m_Bgrid[5] 
{0x0F12, 0x0380},	//0380  //awbb_IndoorGrZones_m_Bgrid[5] 
{0x0F12, 0x02F8},	//02F8  //awbb_IndoorGrZones_m_Bgrid[6] 
{0x0F12, 0x0368},	//0368  //awbb_IndoorGrZones_m_Bgrid[6] 
{0x0F12, 0x02DC},	//02DC  //awbb_IndoorGrZones_m_Bgrid[7] 
{0x0F12, 0x0352},	//034A  //awbb_IndoorGrZones_m_Bgrid[7] 
{0x0F12, 0x02C2},	//02C2  //awbb_IndoorGrZones_m_Bgrid[8] 
{0x0F12, 0x033C},	//0336  //awbb_IndoorGrZones_m_Bgrid[8] 
{0x0F12, 0x02AE},	//02AE  //awbb_IndoorGrZones_m_Bgrid[9] 
{0x0F12, 0x032A},	//031E  //awbb_IndoorGrZones_m_Bgrid[9] 
{0x0F12, 0x029A},	//029A  //awbb_IndoorGrZones_m_Bgrid[10]
{0x0F12, 0x031C},	//0306  //awbb_IndoorGrZones_m_Bgrid[10]
{0x0F12, 0x028C},	//028C  //awbb_IndoorGrZones_m_Bgrid[11]
{0x0F12, 0x030A},	//02F4  //awbb_IndoorGrZones_m_Bgrid[11]
{0x0F12, 0x027C},	//027C  //awbb_IndoorGrZones_m_Bgrid[12]
{0x0F12, 0x02FC},	//02DC  //awbb_IndoorGrZones_m_Bgrid[12]
{0x0F12, 0x0264},	//0264  //awbb_IndoorGrZones_m_Bgrid[13]
{0x0F12, 0x02EC},	//02C2  //awbb_IndoorGrZones_m_Bgrid[13]
{0x0F12, 0x0252},	//0252  //awbb_IndoorGrZones_m_Bgrid[14]
{0x0F12, 0x02DE},	//02AE  //awbb_IndoorGrZones_m_Bgrid[14]
{0x0F12, 0x0246},	//0246  //awbb_IndoorGrZones_m_Bgrid[15]
{0x0F12, 0x02CC},	//029C  //awbb_IndoorGrZones_m_Bgrid[15]
{0x0F12, 0x023C},	//023C  //awbb_IndoorGrZones_m_Bgrid[16]
{0x0F12, 0x02C2},	//028A  //awbb_IndoorGrZones_m_Bgrid[16]
{0x0F12, 0x022E},	//022E  //awbb_IndoorGrZones_m_Bgrid[17]
{0x0F12, 0x02B4},	//027E  //awbb_IndoorGrZones_m_Bgrid[17]
{0x0F12, 0x0222},	//0222  //awbb_IndoorGrZones_m_Bgrid[18]
{0x0F12, 0x02A8},	//0272  //awbb_IndoorGrZones_m_Bgrid[18]
{0x0F12, 0x0212},	//0212  //awbb_IndoorGrZones_m_Bgrid[19]
{0x0F12, 0x029C},	//0266  //awbb_IndoorGrZones_m_Bgrid[19]
{0x0F12, 0x0202},	//0202  //awbb_IndoorGrZones_m_Bgrid[20]
{0x0F12, 0x0292},	//025A  //awbb_IndoorGrZones_m_Bgrid[20]
{0x0F12, 0x01FA},	//01FA  //awbb_IndoorGrZones_m_Bgrid[21]
{0x0F12, 0x0288},	//0250  //awbb_IndoorGrZones_m_Bgrid[21]
{0x0F12, 0x01EC},	//01EC  //awbb_IndoorGrZones_m_Bgrid[22]
{0x0F12, 0x027E},	//0246  //awbb_IndoorGrZones_m_Bgrid[22]
{0x0F12, 0x01E6},	//01E6  //awbb_IndoorGrZones_m_Bgrid[23]
{0x0F12, 0x0272},	//023C  //awbb_IndoorGrZones_m_Bgrid[23]
{0x0F12, 0x01DC},	//01DC  //awbb_IndoorGrZones_m_Bgrid[24]
{0x0F12, 0x0264},	//0234  //awbb_IndoorGrZones_m_Bgrid[24]
{0x0F12, 0x01D4},	//01D4  //awbb_IndoorGrZones_m_Bgrid[25]
{0x0F12, 0x0256},	//0228  //awbb_IndoorGrZones_m_Bgrid[25]
{0x0F12, 0x01CE},	//01CE  //awbb_IndoorGrZones_m_Bgrid[26]
{0x0F12, 0x0248},	//0220  //awbb_IndoorGrZones_m_Bgrid[26]
{0x0F12, 0x01C6},	//01C6  //awbb_IndoorGrZones_m_Bgrid[27]
{0x0F12, 0x023E},	//0216  //awbb_IndoorGrZones_m_Bgrid[27]
{0x0F12, 0x01C0},	//01C0  //awbb_IndoorGrZones_m_Bgrid[28]
{0x0F12, 0x022E},	//020C  //awbb_IndoorGrZones_m_Bgrid[28]
{0x0F12, 0x01BE},	//01BE  //awbb_IndoorGrZones_m_Bgrid[29]
{0x0F12, 0x0222},	//01FC  //awbb_IndoorGrZones_m_Bgrid[29]
{0x0F12, 0x01C4},	//01C4  //awbb_IndoorGrZones_m_Bgrid[30]
{0x0F12, 0x020E},	//01EE  //awbb_IndoorGrZones_m_Bgrid[30]
{0x0F12, 0x01D0},	//01D0  //awbb_IndoorGrZones_m_Bgrid[31]
{0x0F12, 0x01E0},	//01E0  //awbb_IndoorGrZones_m_Bgrid[31]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[32]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[32]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[33]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[33]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[34]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[34]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[35]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[35]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[36]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[36]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[37]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[37]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[38]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[38]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[39]
{0x0F12, 0x0000},               //awbb_IndoorGrZones_m_Bgrid[39]

{0x0F12, 0x0004},	//awbb_IndoorGrZones_m_GridStep
{0x0F12, 0x0000},	//awbb_IndoorGrZones_m_GridSz

{0x002A, 0x0CF8},
{0x0F12, 0x010F},	//awbb_IndoorGrZones_m_Boffs
{0x0F12, 0x0000},	//awbb_IndoorGrZones_m_Boffs

{0x002A, 0x0D84},	//awbb_LowBrGrZones_m_BGrid
{0x0F12, 0x0406},	//awbb_LowBrGrZones_m_BGrid[0] 
{0x0F12, 0x0467},       //awbb_LowBrGrZones_m_BGrid[0] 
{0x0F12, 0x0371},       //awbb_LowBrGrZones_m_BGrid[1] 
{0x0F12, 0x04B0},       //awbb_LowBrGrZones_m_BGrid[1] 
{0x0F12, 0x02E5},       //awbb_LowBrGrZones_m_BGrid[2] 
{0x0F12, 0x0481},       //awbb_LowBrGrZones_m_BGrid[2] 
{0x0F12, 0x0298},       //awbb_LowBrGrZones_m_BGrid[3] 
{0x0F12, 0x042E},       //awbb_LowBrGrZones_m_BGrid[3] 
{0x0F12, 0x0260},       //awbb_LowBrGrZones_m_BGrid[4] 
{0x0F12, 0x03DE},       //awbb_LowBrGrZones_m_BGrid[4] 
{0x0F12, 0x022F},       //awbb_LowBrGrZones_m_BGrid[5] 
{0x0F12, 0x0391},       //awbb_LowBrGrZones_m_BGrid[5] 
{0x0F12, 0x0201},       //awbb_LowBrGrZones_m_BGrid[6] 
{0x0F12, 0x034D},       //awbb_LowBrGrZones_m_BGrid[6] 
{0x0F12, 0x01DA},       //awbb_LowBrGrZones_m_BGrid[7] 
{0x0F12, 0x0310},       //awbb_LowBrGrZones_m_BGrid[7] 
{0x0F12, 0x01B3},       //awbb_LowBrGrZones_m_BGrid[8] 
{0x0F12, 0x02D4},       //awbb_LowBrGrZones_m_BGrid[8] 
{0x0F12, 0x018F},       //awbb_LowBrGrZones_m_BGrid[9] 
{0x0F12, 0x0297},       //awbb_LowBrGrZones_m_BGrid[9] 
{0x0F12, 0x0181},       //awbb_LowBrGrZones_m_BGrid[10]
{0x0F12, 0x0271},       //awbb_LowBrGrZones_m_BGrid[10]
{0x0F12, 0x0181},       //awbb_LowBrGrZones_m_BGrid[11]
{0x0F12, 0x022A},       //awbb_LowBrGrZones_m_BGrid[11]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[12]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[12]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[13]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[13]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[14]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[14]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[15]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[15]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[16]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[16]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[17]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[17]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[18]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[18]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[19]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[19]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[20]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[20]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[21]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[21]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[22]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[22]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[23]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[23]
{0x0F12, 0x0000},       //awbb_LowBrGrZones_m_BGrid[24]
{0x0F12, 0x0000},	//awbb_LowBrGrZones_m_BGrid[24]
                
{0x0F12, 0x0006},	//awbb_LowBrGrZones_m_GridStep
{0x0F12, 0x0000},	//awbb_LowBrGrZones_m_GridSz

{0x002A, 0x0DF0},
{0x0F12, 0x0081},	//awbb_LowBrGrZones_m_Boffs
{0x0F12, 0x0000},	//awbb_LowBrGrZones_m_Boffs

{0x002A, 0x0D08},
{0x0F12, 0x026A},	//0264 //0250	//awbb_OutdoorGrZones_m_BGrid[0]
{0x0F12, 0x0280},	//0274 //0260	//awbb_OutdoorGrZones_m_BGrid[1]
{0x0F12, 0x025E},	//0259 //0245	//awbb_OutdoorGrZones_m_BGrid[2]
{0x0F12, 0x028C},	//027F //026B	//awbb_OutdoorGrZones_m_BGrid[3]
{0x0F12, 0x0250},	//024E //023A	//awbb_OutdoorGrZones_m_BGrid[4]
{0x0F12, 0x0290},	//0281 //026D	//awbb_OutdoorGrZones_m_BGrid[5]
{0x0F12, 0x0246},	//0244 //0230	//awbb_OutdoorGrZones_m_BGrid[6]
{0x0F12, 0x0290},	//0283 //026F	//awbb_OutdoorGrZones_m_BGrid[7]
{0x0F12, 0x023C},	//023A //0226	//awbb_OutdoorGrZones_m_BGrid[8]
{0x0F12, 0x0290},	//0283 //026F	//awbb_OutdoorGrZones_m_BGrid[9]
{0x0F12, 0x0234},	//0235 //0221	//awbb_OutdoorGrZones_m_BGrid[10]
{0x0F12, 0x028E},	//027E //026A	//awbb_OutdoorGrZones_m_BGrid[11]
{0x0F12, 0x022A},	//0231 //021D	//awbb_OutdoorGrZones_m_BGrid[12]
{0x0F12, 0x028C},	//0278 //0264	//awbb_OutdoorGrZones_m_BGrid[13]
{0x0F12, 0x021E},	//022B //0217	//awbb_OutdoorGrZones_m_BGrid[14]
{0x0F12, 0x0286},	//0274 //0260	//awbb_OutdoorGrZones_m_BGrid[15]
{0x0F12, 0x0214},	//0224 //0210	//awbb_OutdoorGrZones_m_BGrid[16]
{0x0F12, 0x0280},	//026D //0259	//awbb_OutdoorGrZones_m_BGrid[17]
{0x0F12, 0x020E},	//021E //020A	//awbb_OutdoorGrZones_m_BGrid[18]
{0x0F12, 0x0278},	//0265 //0251	//awbb_OutdoorGrZones_m_BGrid[19]
{0x0F12, 0x020A},	//0218 //0204	//awbb_OutdoorGrZones_m_BGrid[20]
{0x0F12, 0x0272},	//025F //024B	//awbb_OutdoorGrZones_m_BGrid[21]
{0x0F12, 0x0204},	//0211 //01FD	//awbb_OutdoorGrZones_m_BGrid[22]
{0x0F12, 0x026C},	//0259 //0245	//awbb_OutdoorGrZones_m_BGrid[23]
{0x0F12, 0x0200},	//020A //01F6	//awbb_OutdoorGrZones_m_BGrid[24]
{0x0F12, 0x0264},	//0252 //023E	//awbb_OutdoorGrZones_m_BGrid[25]
{0x0F12, 0x01FE},	//0207 //01F1	//awbb_OutdoorGrZones_m_BGrid[26]
{0x0F12, 0x025E},	//0239 //0238	//awbb_OutdoorGrZones_m_BGrid[27]
{0x0F12, 0x01FE},	//0204 //01EC	//awbb_OutdoorGrZones_m_BGrid[28]
{0x0F12, 0x0258},	//0224 //022F	//awbb_OutdoorGrZones_m_BGrid[29]
{0x0F12, 0x01FE},	//0000 //01E9	//awbb_OutdoorGrZones_m_BGrid[30]
{0x0F12, 0x0252},	//0000 //0219	//awbb_OutdoorGrZones_m_BGrid[31]
{0x0F12, 0x0200},	//0000 //01EF	//awbb_OutdoorGrZones_m_BGrid[32]
{0x0F12, 0x0238},	//0000 //01F8	//awbb_OutdoorGrZones_m_BGrid[33]
{0x0F12, 0x0204},	//0000	//awbb_OutdoorGrZones_m_BGrid[34]
{0x0F12, 0x0224},	//0000	//awbb_OutdoorGrZones_m_BGrid[35]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[36]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[37]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[38]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[39]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[40]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[41]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[42]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[43]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[44]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[45]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[46]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[47]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[48]
{0x0F12, 0x0000},	//0000	//awbb_OutdoorGrZones_m_BGrid[49]
{0x0F12, 0x0003},	//0003	/awbb_OutdoorGrZones_m_Gridstep
{0x0F12, 0x0000},	//0000

{0x002A, 0x0D70},
{0x0F12, 0x0011},	//awbb_OutdoorGrZones_m_GridSz
{0x0F12, 0x0000},

{0x002A, 0x0D74},
{0x0F12, 0x020E},	//021F	//2F	//awbb_OutdoorGrZones_m_Boffs   Don't modify
{0x0F12, 0x0000},
{0x002A, 0x0E00},
{0x0F12, 0x034A},	//awbb_CrclLowT_R_c
{0x0F12, 0x0000},
{0x0F12, 0x0176},	//awbb_CrclLowT_B_c
{0x0F12, 0x0000},
{0x0F12, 0x71B8},	//awbb_CrclLowT_Rad_c
{0x0F12, 0x0000},
{0x002A, 0x0E1A},
{0x0F12, 0x012F},
{0x0F12, 0x0120},
{0x002A, 0x0E68},
{0x0F12, 0x04F2},	// awbb_LowTempRB
{0x002A, 0x0D78},
{0x0F12, 0x0020},	//AWB min.
{0x002A, 0x0D80},
{0x0F12, 0x00E0},	//AWB Max.
{0x002A, 0x0E40},	//awbb_Use_Filters
{0x0F12, 0x0061},	//AWB option
{0x002A, 0x0EE4},
{0x0F12, 0x0003},	//awbb_OutdoorFltrsz
{0x002A, 0x0E3C},
{0x0F12, 0x0001},	//awbb_Use_InvalidOutDoor
{0x002A, 0x0F3A},
{0x0F12, 0x024C},	//awbb_OutdoorWP_r	//0240  //0218  // 0226
{0x0F12, 0x0290},	//awbb_OutdoorWP_b	//2B2  // 0280

{0x002A, 0x0E46},
{0x0F12, 0x0BB7},	//FA0	//awbb_sunnyBr
{0x0F12, 0x0096},	//awbb_sunny_NBzone
{0x0F12, 0x0BB8},	//awbb_CloudyBr

{0x002A, 0x0E5E},
{0x0F12, 0x071A},	//awbb_GamutWidthThr1
{0x0F12, 0x03A4},

{0x002A, 0x0E50},
{0x0F12, 0x001B},	//awbb_MacbethGamut_WidthZone
{0x0F12, 0x000E},
{0x0F12, 0x0008},
{0x0F12, 0x0004},

{0x002A, 0x0E36},
{0x0F12, 0x0001},	//awbb_ByPass_LowTempMode

{0x002A, 0x0E18},
{0x0F12, 0x0000},	// 32 awbb_dark

{0x002A, 0x0E3A},
{0x0F12, 0x02C2},	//awbb_Alpha_Comp_Mode
{0x002A, 0x0F12},
{0x0F12, 0x02C9},	//awbb_GLocusR
{0x0F12, 0x033F},	//awbb_GLocusB
{0x002A, 0x0E1A},
{0x0F12, 0x0138},	//awbb_IntcR

{0x002A, 0x236C},
{0x0F12, 0x0000},	//AWBBTune_EVT4_uInitPostToleranceCnt


{0x002A, 0x0C48},	//awbb_GainsInit
{0x0F12, 0x053C},	//R Gain
{0x0F12, 0x03B0},	//400
{0x0F12, 0x055C},	//B Gain


//================================================================
// Grid Correction
//================================================================
{0x002A, 0x0E42},
{0x0F12, 0x0002},
{0x002A, 0x0EE0},
{0x0F12, 0x00B5},	//awbb_GridCoeff_R_2
{0x0F12, 0x00B5},	//awbb_GridCoeff_B_2
{0x002A, 0x0ED0},
{0x0F12, 0x0EC8},	//awbb_GridConst_2[0]
{0x0F12, 0x1022},	//awbb_GridConst_2[1]
{0x0F12, 0x10BB},	//awbb_GridConst_2[2]
{0x0F12, 0x10C9},	//awbb_GridConst_2[3]
{0x0F12, 0x1149},	//awbb_GridConst_2[4]
{0x0F12, 0x11FD},	//awbb_GridConst_2[5]
{0x0F12, 0x00B8},	//awbb_GridCoeff_R_1
{0x0F12, 0x00B2},	//awbb_GridCoeff_B_1
{0x002A, 0x0ECA},
{0x0F12, 0x029A},	//awbb_GridConst_1[0]
{0x0F12, 0x0344},	//awbb_GridConst_1[1]
{0x0F12, 0x03FB},	//awbb_GridConst_1[2]
{0x002A, 0x0E82}, 
{0x0F12, 0xFFE0},	//awbb_GridCorr_R[0][0]	//d65
{0x0F12, 0x0000},	//awbb_GridCorr_R[0][1] //d50
{0x0F12, 0x0004},	//awbb_GridCorr_R[0][2] //cw
{0x0F12, 0x0004},	//awbb_GridCorr_R[0][3] //ww
{0x0F12, 0x0010},	//awbb_GridCorr_R[0][4] //a
{0x0F12, 0x007D},	//awbb_GridCorr_R[0][5] //h
{0x0F12, 0xFFE0},	//awbb_GridCorr_R[1][0]	//d65
{0x0F12, 0x0000},	//awbb_GridCorr_R[1][1] //d50
{0x0F12, 0x0004},	//awbb_GridCorr_R[1][2] //cw
{0x0F12, 0x0004},	//awbb_GridCorr_R[1][3] //ww
{0x0F12, 0x0010},	//awbb_GridCorr_R[1][4] //a
{0x0F12, 0x007D},	//awbb_GridCorr_R[1][5] //h
{0x0F12, 0xFFE0},	//awbb_GridCorr_R[2][0]	//d65
{0x0F12, 0x0000},	//awbb_GridCorr_R[2][1] //d50
{0x0F12, 0x0004},	//awbb_GridCorr_R[2][2] //cw
{0x0F12, 0x0004},	//awbb_GridCorr_R[2][3] //ww
{0x0F12, 0x0010},	//awbb_GridCorr_R[2][4] //a
{0x0F12, 0x007D},	//awbb_GridCorr_R[2][5] //h

{0x0F12, 0x0010},	//awbb_GridCorr_B[0][0]	//d65
{0x0F12, 0x0010},	//awbb_GridCorr_B[0][1] //d50
{0x0F12, 0x0010},	//awbb_GridCorr_B[0][2] //cw
{0x0F12, 0x0000},	//awbb_GridCorr_B[0][3] //ww
{0x0F12, 0xFF7D},	//awbb_GridCorr_B[0][4] //a
{0x0F12, 0xFDE7},	//awbb_GridCorr_B[0][5] //h
{0x0F12, 0x0010},	//awbb_GridCorr_B[1][0]	//d65
{0x0F12, 0x0010},	//awbb_GridCorr_B[1][1] //d50                         
{0x0F12, 0x0010},	//awbb_GridCorr_B[1][2] //cw                         
{0x0F12, 0x0000},	//awbb_GridCorr_B[1][3] //ww                           
{0x0F12, 0xFF7D},	//awbb_GridCorr_B[1][4] //a                       
{0x0F12, 0xFDE7},	//awbb_GridCorr_B[0][5] //h              
{0x0F12, 0x0010},	//awbb_GridCorr_B[2][0]	//d65
{0x0F12, 0x0010},	//awbb_GridCorr_B[2][1] //d50                         
{0x0F12, 0x0010},	//awbb_GridCorr_B[2][2] //cw                         
{0x0F12, 0x0000},	//awbb_GridCorr_B[2][3] //ww                           
{0x0F12, 0xFF7D},	//awbb_GridCorr_B[2][4] //a                       
{0x0F12, 0xFDE7},	//awbb_GridCorr_B[0][5] //h


//================================================================
// CCM                                                            
//================================================================                                        
{0x002A, 0x06D4},
{0x0F12, 0x2380},	//TVAR_wbt_pOutdoorCcm         
{0x0F12, 0x7000},                               
{0x002A, 0x06CC},                               
{0x0F12, 0x23A4},	//TVAR_wbt_pBaseCcms           
{0x0F12, 0x7000},                               
{0x002A, 0x06E8},
{0x0F12, 0x23A4},
{0x0F12, 0x7000},
{0x0F12, 0x23C8},
{0x0F12, 0x7000},
{0x0F12, 0x23EC},
{0x0F12, 0x7000},
{0x0F12, 0x2410},
{0x0F12, 0x7000},
{0x0F12, 0x2434},
{0x0F12, 0x7000},
{0x0F12, 0x2458},
{0x0F12, 0x7000},
{0x002A, 0x06DA},
{0x0F12, 0x00BF},	//SARR_AwbCcmCord[0]  2   700006DA
{0x0F12, 0x00E6},	//SARR_AwbCcmCord[1]  2   700006DC
{0x0F12, 0x00F2},	//SARR_AwbCcmCord[2]  2   700006DE
{0x0F12, 0x0143},	//SARR_AwbCcmCord[3]  2   700006E0
{0x0F12, 0x0178},	//SARR_AwbCcmCord[4]  2   700006E2
{0x0F12, 0x01A3},	//SARR_AwbCcmCord[5]
{0x002A, 0x23A4},
{0x0F12, 0x01FA},	//01FA	  	//H//23A4
{0x0F12, 0xFFB9},	//FFB9
{0x0F12, 0xFFF8},	//FFF8
{0x0F12, 0x0116},	//00EC
{0x0F12, 0x00BD},	//00EB
{0x0F12, 0xFF38},	//FF33
{0x0F12, 0xFF23},	//FF23
{0x0F12, 0x01AB},	//01AB
{0x0F12, 0xFF81},	//FF81
{0x0F12, 0xFF0D},	//FF0D
{0x0F12, 0x0169},	//0169
{0x0F12, 0x00DE},	//00DE
{0x0F12, 0xFFEF},	//FFEF
{0x0F12, 0xFFCA},	//FFCA
{0x0F12, 0x014D},	//014D
{0x0F12, 0x01C3},	//01C3
{0x0F12, 0xFF7E},	//FF7E
{0x0F12, 0x016F},	//016F

{0x0F12, 0x01FA},	//01FA    //A//23C8
{0x0F12, 0xFFB9},	//FFB9
{0x0F12, 0xFFF8},	//FFF8
{0x0F12, 0x0116},	//00EC
{0x0F12, 0x00BD},	//00EB
{0x0F12, 0xFF38},	//FF33
{0x0F12, 0xFF23},	//FF23
{0x0F12, 0x01AB},	//01AB
{0x0F12, 0xFF81},	//FF81
{0x0F12, 0xFF0D},	//FF0D
{0x0F12, 0x0169},	//0169
{0x0F12, 0x00DE},	//00DE
{0x0F12, 0xFFEF},	//FFEF
{0x0F12, 0xFFCA},	//FFCA
{0x0F12, 0x014D},	//014D
{0x0F12, 0x01C3},	//01C3
{0x0F12, 0xFF7E},	//FF7E
{0x0F12, 0x016F},	//016F

{0x0F12, 0x01FA},	//01FA	//WW//23EC
{0x0F12, 0xFFB9},	//FFB9
{0x0F12, 0xFFF8},	//FFF8
{0x0F12, 0x0116},	//00EC
{0x0F12, 0x00BD},	//00EB
{0x0F12, 0xFF38},	//FF33
{0x0F12, 0xFF23},	//FF23
{0x0F12, 0x01AB},	//01AB
{0x0F12, 0xFF81},	//FF81
{0x0F12, 0xFF0D},	//FF0D
{0x0F12, 0x0169},	//0169
{0x0F12, 0x00DE},	//00DE
{0x0F12, 0xFFEF},	//FFEF
{0x0F12, 0xFFCA},	//FFCA
{0x0F12, 0x014D},	//014D
{0x0F12, 0x01C3},	//01C3
{0x0F12, 0xFF7E},	//FF7E
{0x0F12, 0x016F},	//016F

{0x0F12, 0x01B1},	//01B6 ////0198 ////01D6 ////01E0 ////01B7	//CW//2410
{0x0F12, 0xFFCD},	//FFDE ////FFF1 ////FFBD ////FFB9 ////FFD0
{0x0F12, 0x0016},	//0003 ////0012 ////000D ////000C ////0019
{0x0F12, 0x00E6},	//00C5
{0x0F12, 0x00D6},	//00E3
{0x0F12, 0xFF4A},	//FF5D
{0x0F12, 0xFF33},	//FF3E
{0x0F12, 0x019C},	//0186
{0x0F12, 0xFF85},	//FF8C
{0x0F12, 0xFF0E},	//FF0E
{0x0F12, 0x016A},	//016A
{0x0F12, 0x00DF},	//00DF
{0x0F12, 0xFFEE},	//FFEE
{0x0F12, 0xFFC9},	//FFC9
{0x0F12, 0x014C},	//014C
{0x0F12, 0x01C1},	//01C1
{0x0F12, 0xFF7C},	//FF7C
{0x0F12, 0x016D},	//016D

{0x0F12, 0x01B4},	//01B6 ////01AC ////0198 ////01D6 ////01E2 ////01E0 ////01B7	//D50//2434
{0x0F12, 0xFFD4},	//FFDE ////FFE4 ////FFF1 ////FFBD ////FFB8 ////FFB9 ////FFD0
{0x0F12, 0x000C},	//0003 ////0008 ////0012 ////000D ////000B ////000C ////0019
{0x0F12, 0x00CB},	//00CB ////00C5
{0x0F12, 0x00F3},	//00F3 ////00E3
{0x0F12, 0xFF4C},	//FF4C ////FF5D
{0x0F12, 0xFF66},	//ff20 ////FF33 ////FF3E
{0x0F12, 0x01AB},	//01bb ////019C ////0186
{0x0F12, 0xFF43},	//ff79 ////FF85 ////FF8C
{0x0F12, 0xFF0E},	//FF0E ////FF0E
{0x0F12, 0x016A},	//016A ////016A
{0x0F12, 0x00DF},	//00DF ////00DF
{0x0F12, 0xFFE8},	//FFEE ////FFEE
{0x0F12, 0xFFC1},	//FFC9 ////FFC9
{0x0F12, 0x015A},	//014C ////014C
{0x0F12, 0x01C1},	//01C1 ////01C1
{0x0F12, 0xFF7C},	//FF7C ////FF7C
{0x0F12, 0x016D},	//016D ////016D

{0x0F12, 0x01E6},	//01E6	
{0x0F12, 0xFFB7},	//FFB7	
{0x0F12, 0xFFE2},	//FFE2

{0x0F12, 0x00CC},	//00CC	
{0x0F12, 0x00F4},	//00F4	
{0x0F12, 0xFF4D},	//FF4D

{0x0F12, 0xFF1E},	//FF46	
{0x0F12, 0x01E6},	//01D2	
{0x0F12, 0xFF2C},	//FF1C

{0x0F12, 0xFF09},	//FF09	
{0x0F12, 0x0164},	//0164	
{0x0F12, 0x00DA},	//00DA	

{0x0F12, 0xFFD8},	//FFD8	
{0x0F12, 0xFFB2},	//FFB2	
{0x0F12, 0x014B},	//014B

{0x0F12, 0x01B4},	//01B4	
{0x0F12, 0xFF6E},	//FF6E	
{0x0F12, 0x0160},	//0160	

{0x002A, 0x2380},	//TVAR_wbt_pOutdoorCcm
{0x0F12, 0x01AE},	//01AA //01B9 //01DD	//TVAR_wbt_pOutdoorCcm[0]  ¾ß¿Ü¾ó±¼»ö°¨       ÇÏ´Ã
{0x0F12, 0xFFB9},	//FFB9 //FFBC //FFA8	//TVAR_wbt_pOutdoorCcm[1]
{0x0F12, 0x0017},	//001E //0012 //0008	//TVAR_wbt_pOutdoorCcm[2]
{0x0F12, 0x00FC},	//00FB //00FD	//TVAR_wbt_pOutdoorCcm[3]
{0x0F12, 0x013B},	//013B //0141	//TVAR_wbt_pOutdoorCcm[4]
{0x0F12, 0xFF84},	//FF82 //FF74	//TVAR_wbt_pOutdoorCcm[5]
{0x0F12, 0xFED9},	//FED6 //FEC6	//TVAR_wbt_pOutdoorCcm[6]
{0x0F12, 0x01E6},	//01E6 //01F6	//TVAR_wbt_pOutdoorCcm[7]
{0x0F12, 0xFF16},	//FF13 //FF05	//TVAR_wbt_pOutdoorCcm[8]
{0x0F12, 0xFF4A},	//FF48 //FF38	//TVAR_wbt_pOutdoorCcm[9]
{0x0F12, 0x0179},	//0179 //017F	//TVAR_wbt_pOutdoorCcm[10]
{0x0F12, 0x014F},	//014F //0153	//TVAR_wbt_pOutdoorCcm[11]
{0x0F12, 0xFFC2},	//FFCB //FFCC	//TVAR_wbt_pOutdoorCcm[12]
{0x0F12, 0xFF99},	//FFA4 //FFA6	//TVAR_wbt_pOutdoorCcm[13]
{0x0F12, 0x0219},	//0207 //020A	//TVAR_wbt_pOutdoorCcm[14]
{0x0F12, 0x0172},	//0174 //017E	//TVAR_wbt_pOutdoorCcm[15]
{0x0F12, 0xFF51},	//FF51 //FF45	//TVAR_wbt_pOutdoorCcm[16]
{0x0F12, 0x019B},	//019E //01AA	//TVAR_wbt_pOutdoorCcm[17]

//================================================================
// AFIT
//================================================================
{0x002A, 0x07E8},	//SARR_uNormBrInDoor
{0x0F12, 0x0016},	//000A	//SARR_uNormBrInDoor[0]
{0x0F12, 0x0028},	//0019	//SARR_uNormBrInDoor[1]
{0x0F12, 0x0096},	//0096	//SARR_uNormBrInDoor[2]
{0x0F12, 0x01F4},	//01F4	//SARR_uNormBrInDoor[3]
{0x0F12, 0x07D0},	//07D0	//SARR_uNormBrInDoor[4]
{0x002A, 0x07D0},	//afit_uNoiseIndInDoor
{0x0F12, 0x0030},	//afit_uNoiseIndInDoor[0]
{0x0F12, 0x0046},	//afit_uNoiseIndInDoor[1]
{0x0F12, 0x0088},	//afit_uNoiseIndInDoor[2]
{0x0F12, 0x0205},	//afit_uNoiseIndInDoor[3]
{0x0F12, 0x02BC},	//afit_uNoiseIndInDoor[4]
{0x002A, 0x07E6},	  
{0x0F12, 0x0000},	//afit_bUseNoiseInd
{0x002A, 0x0828},	             
{0x0F12, 0x0008},	//10	  //TVAR_afit_pBaseVals[0]     70000828  //BRIGHTNESS                                                                
{0x0F12, 0x0014},	//TVAR_afit_pBaseVals[1]     7000082A  //CONTRAST                                                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[2]     7000082C  //SATURATION                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[3]     7000082E  //SHARP_BLUR                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[4]     70000830  //GLAMOUR
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[5]     70000832  //Disparity_iSatSat                                                         
{0x0F12, 0x0021},	//TVAR_afit_pBaseVals[6]     70000834  //Denoise1_iYDenThreshLow
{0x0F12, 0x0028},	//TVAR_afit_pBaseVals[7]     70000836  //Denoise1_iYDenThreshLow_Bin
{0x0F12, 0x0050},	//TVAR_afit_pBaseVals[8]     70000838  //Denoise1_iYDenThreshHigh
{0x0F12, 0x00FF},	//TVAR_afit_pBaseVals[9]     7000083A  //Denoise1_iYDenThreshHigh_Bin
{0x0F12, 0x0129},	//TVAR_afit_pBaseVals[10]    7000083C  //Denoise1_iLowWWideThresh
{0x0F12, 0x000A},	//TVAR_afit_pBaseVals[11]    7000083E  //Denoise1_iHighWWideThresh
{0x0F12, 0x0028},	//TVAR_afit_pBaseVals[12]    70000840  //Denoise1_iLowWideThresh
{0x0F12, 0x0028},	//TVAR_afit_pBaseVals[13]    70000842  //Denoise1_iHighWideThresh
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[14]    70000844  //Denoise1_iSatSat                                                          
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[15]    70000846  //Demosaic4_iHystGrayLow
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[16]    70000848  //Demosaic4_iHystGrayHigh
{0x0F12, 0x0344},	//TVAR_afit_pBaseVals[17]    7000084A  //UVDenoise_iYLowThresh
{0x0F12, 0x033A},	//TVAR_afit_pBaseVals[18]    7000084C  //UVDenoise_iYHighThresh
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[19]    7000084E  //UVDenoise_iUVLowThresh
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[20]    70000850  //UVDenoise_iUVHighThresh
{0x0F12, 0x000A},	//TVAR_afit_pBaseVals[21]    70000852  //DSMix1_iLowLimit_Wide                                                     
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[22]    70000854  //DSMix1_iLowLimit_Wide_Bin                                                 
{0x0F12, 0x001E},	//TVAR_afit_pBaseVals[23]    70000856  //DSMix1_iHighLimit_Wide                                                    
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[24]    70000858  //DSMix1_iHighLimit_Wide_Bin                                                
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[25]    7000085A  //DSMix1_iLowLimit_Fine                                                     
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[26]    7000085C  //DSMix1_iLowLimit_Fine_Bin                                                 
{0x0F12, 0x0010},	//TVAR_afit_pBaseVals[27]    7000085E  //DSMix1_iHighLimit_Fine                                                    
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[28]    70000860  //DSMix1_iHighLimit_Fine_Bin                                                
{0x0F12, 0x0106},	//TVAR_afit_pBaseVals[29]    70000862  //DSMix1_iRGBOffset                                                         
{0x0F12, 0x006F},	//TVAR_afit_pBaseVals[30]    70000864  //DSMix1_iDemClamp                                                          
{0x0F12, 0x0C0F},	//TVAR_afit_pBaseVals[31]    70000866  //"Disparity_iDispTH_LowDisparity_iDispTH_Low_Bin"
{0x0F12, 0x0C0F},	//TVAR_afit_pBaseVals[32]    70000868  //"Disparity_iDispTH_High Disparity_iDispTH_High_Bin"
{0x0F12, 0x0303},	//TVAR_afit_pBaseVals[33]    7000086A  //"Despeckle_iCorrectionLevelColdDespeckle_iCorrectionLevelCold_Bin"
{0x0F12, 0x0303},	//TVAR_afit_pBaseVals[34]    7000086C  //Despeckle_iCorrectionLevelHotDespeckle_iCorrectionLevelHot_Bin
{0x0F12, 0x140A},	//TVAR_afit_pBaseVals[35]    7000086E  //"Despeckle_iColdThreshLowDespeckle_iColdThreshHigh"
{0x0F12, 0x140A},	//TVAR_afit_pBaseVals[36]    70000870  //"Despeckle_iHotThreshLowDespeckle_iHotThreshHigh"
{0x0F12, 0x2828},	//TVAR_afit_pBaseVals[37]    70000872  //"Denoise1_iLowMaxSlopeAllowedDenoise1_iHighMaxSlopeAllowed"               
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[38]    70000874  //"Denoise1_iLowSlopeThreshDenoise1_iHighSlopeThresh"                       
{0x0F12, 0x020A},	//TVAR_afit_pBaseVals[39]    70000876  //"Denoise1_iRadialPowerDenoise1_iRadialDivideShift"                        
{0x0F12, 0x0480},	//TVAR_afit_pBaseVals[40]    70000878  //"Denoise1_iRadialLimitDenoise1_iLWBNoise"
{0x0F12, 0x0E08},	//TVAR_afit_pBaseVals[41]    7000087A  //"Denoise1_iWideDenoise1_iWideWide"
{0x0F12, 0x030A},	//TVAR_afit_pBaseVals[42]    7000087C  //"Demosaic4_iHystGrayRangeUVDenoise_iYSupport"                             
{0x0F12, 0x0A03},	//TVAR_afit_pBaseVals[43]    7000087E  //"UVDenoise_iUVSupportDSMix1_iLowPower_Wide"                               
{0x0F12, 0x0A11},	//TVAR_afit_pBaseVals[44]    70000880  //"DSMix1_iLowPower_Wide_BinDSMix1_iHighPower_Wide"                         
{0x0F12, 0x000F},	//TVAR_afit_pBaseVals[45]    70000882  //"DSMix1_iHighPower_Wide_BinDSMix1_iLowThresh_Wide"                        
{0x0F12, 0x0500},	//TVAR_afit_pBaseVals[46]    70000884  //"DSMix1_iHighThresh_WideDSMix1_iReduceNegativeWide"                       
{0x0F12, 0x0914},	//TVAR_afit_pBaseVals[47]    70000886  //"DSMix1_iLowPower_FineDSMix1_iLowPower_Fine_Bin"                          
{0x0F12, 0x0012},	//TVAR_afit_pBaseVals[48]    70000888  //"DSMix1_iHighPower_FineDSMix1_iHighPower_Fine_Bin"                        
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[49]    7000088A  //"DSMix1_iLowThresh_FineDSMix1_iHighThresh_Fine"                           
{0x0F12, 0x0005},	//TVAR_afit_pBaseVals[50]    7000088C  //"DSMix1_iReduceNegativeFineDSMix1_iRGBMultiplier"                         
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[51]    7000088E  //"Mixer1_iNLowNoisePowerMixer1_iNLowNoisePower_Bin"
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[52]    70000890  //"Mixer1_iNVeryLowNoisePowerMixer1_iNVeryLowNoisePower_Bin"
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[53]    70000892  //"Mixer1_iNHighNoisePowerMixer1_iNHighNoisePower_Bin"
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[54]    70000894  //"Mixer1_iWLowNoisePowerMixer1_iWVeryLowNoisePower"
{0x0F12, 0x0A00},	//TVAR_afit_pBaseVals[55]    70000896  //"Mixer1_iWHighNoisePowerMixer1_iWLowNoiseCeilGain"
{0x0F12, 0x000A},	//TVAR_afit_pBaseVals[56]    70000898  //"Mixer1_iWHighNoiseCeilGainMixer1_iWNoiseCeilGain"
{0x0F12, 0x0180},	//014C	//TVAR_afit_pBaseVals[57]    7000089A  //"CCM_Oscar_iSaturationCCM_Oscar_bSaturation"                              
{0x0F12, 0x014D},	//TVAR_afit_pBaseVals[58]    7000089C  //"RGBGamma2_iLinearityRGBGamma2_bLinearity"
{0x0F12, 0x0100},	//TVAR_afit_pBaseVals[59]    7000089E  //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"
{0x0F12, 0x8020},	//TVAR_afit_pBaseVals[60]    700008A0  //"byr_gas2_iShadingPowerRGB2YUV_iRGBGain"                                  
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[61]    700008A2  //"RGB2YUV_iSaturationRGB2YUV_bGainOffset"                                  
{0x0F12, 0x0013},	//15 //18 //001a //05 //A	//TVAR_afit_pBaseVals[62]    700008A4  //RGB2YUV_iYOffset
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[63]    700008A6            //BRIGHTNESS                                                                
{0x0F12, 0x0028},	//TVAR_afit_pBaseVals[64]    700008A8            //CONTRAST                                                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[65]    700008AA            //SATURATION                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[66]    700008AC            //SHARP_BLUR                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[67]    700008AE            //GLAMOUR
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[68]    700008B0    //Disparity_iSatSat                                                         
{0x0F12, 0x000C},	//0E //0C	//0020	//TVAR_afit_pBaseVals[69]    700008B2    //Denoise1_iYDenThreshLow
{0x0F12, 0x000E},	//000E	//TVAR_afit_pBaseVals[70]    700008B4    //Denoise1_iYDenThreshLow_Bin
{0x0F12, 0x0050},	//0080	//TVAR_afit_pBaseVals[71]    700008B6    //Denoise1_iYDenThreshHigh
{0x0F12, 0x00FF},	//00FF	//TVAR_afit_pBaseVals[72]    700008B8    //Denoise1_iYDenThreshHigh_Bin
{0x0F12, 0x0129},	//0129	//TVAR_afit_pBaseVals[73]    700008BA    //Denoise1_iLowWWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[74]    700008BC    //Denoise1_iHighWWideThresh
{0x0F12, 0x0028},	//0028	//TVAR_afit_pBaseVals[75]    700008BE    //Denoise1_iLowWideThresh
{0x0F12, 0x0028},	//0028	//TVAR_afit_pBaseVals[76]    700008C0    //Denoise1_iHighWideThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[77]    700008C2    //Denoise1_iSatSat                                                          
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[78]    700008C4    //Demosaic4_iHystGrayLow
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[79]    700008C6    //Demosaic4_iHystGrayHigh
{0x0F12, 0x0114},	//0014	//TVAR_afit_pBaseVals[80]    700008C8    //UVDenoise_iYLowThresh
{0x0F12, 0x020A},	//000A	//TVAR_afit_pBaseVals[81]    700008CA    //UVDenoise_iYHighThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[82]    700008CC    //UVDenoise_iUVLowThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[83]    700008CE    //UVDenoise_iUVHighThresh
{0x0F12, 0x0018},	//000a	//TVAR_afit_pBaseVals[84]    700008D0    //DSMix1_iLowLimit_Wide                                                     
{0x0F12, 0x0032},	//0000	//TVAR_afit_pBaseVals[85]    700008D2    //DSMix1_iLowLimit_Wide_Bin                                                 
{0x0F12, 0x000A},	//0014	//TVAR_afit_pBaseVals[86]    700008D4    //DSMix1_iHighLimit_Wide                                                    
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[87]    700008D6    //DSMix1_iHighLimit_Wide_Bin                                                
{0x0F12, 0x0028},	//0000	//TVAR_afit_pBaseVals[88]    700008D8    //DSMix1_iLowLimit_Fine                                                     
{0x0F12, 0x0032},	//0000	//TVAR_afit_pBaseVals[89]    700008DA    //DSMix1_iLowLimit_Fine_Bin                                                 
{0x0F12, 0x0010},	//00A0	//TVAR_afit_pBaseVals[90]    700008DC    //DSMix1_iHighLimit_Fine                                                    
{0x0F12, 0x0032},	//0000	//TVAR_afit_pBaseVals[91]    700008DE    //DSMix1_iHighLimit_Fine_Bin                                                
{0x0F12, 0x0106},	//0106	//TVAR_afit_pBaseVals[92]    700008E0    //DSMix1_iRGBOffset                                                         
{0x0F12, 0x006F},	//006F	//TVAR_afit_pBaseVals[93]    700008E2    //DSMix1_iDemClamp                                                          
{0x0F12, 0x050F},	//050F	//TVAR_afit_pBaseVals[94]    700008E4    //"Disparity_iDispTH_LowDisparity_iDispTH_Low_Bin"
{0x0F12, 0x0A0F},	//0A0F	//TVAR_afit_pBaseVals[95]    700008E6    //"Disparity_iDispTH_High Disparity_iDispTH_High_Bin"
{0x0F12, 0x0203},	//0203	//TVAR_afit_pBaseVals[96]    700008E8    //"Despeckle_iCorrectionLevelColdDespeckle_iCorrectionLevelCold_Bin"
{0x0F12, 0x0303},	//0203	//TVAR_afit_pBaseVals[97]    700008EA    //Despeckle_iCorrectionLevelHotDespeckle_iCorrectionLevelHot_Bin
{0x0F12, 0x140A},	//140A	//TVAR_afit_pBaseVals[98]    700008EC    //"Despeckle_iColdThreshLowDespeckle_iColdThreshHigh"
{0x0F12, 0x140A},	//140A	//TVAR_afit_pBaseVals[99]    700008EE    //"Despeckle_iHotThreshLowDespeckle_iHotThreshHigh"
{0x0F12, 0x2828},	//2828	//TVAR_afit_pBaseVals[100]   700008F0    //"Denoise1_iLowMaxSlopeAllowedDenoise1_iHighMaxSlopeAllowed"               
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[101]   700008F2    //"Denoise1_iLowSlopeThreshDenoise1_iHighSlopeThresh"                       
{0x0F12, 0x020A},	//020A	//TVAR_afit_pBaseVals[102]   700008F4    //"Denoise1_iRadialPowerDenoise1_iRadialDivideShift"                        
{0x0F12, 0x0480},	//0480	//TVAR_afit_pBaseVals[103]   700008F6    //"Denoise1_iRadialLimitDenoise1_iLWBNoise"
{0x0F12, 0x0E08},	//0E08	//TVAR_afit_pBaseVals[104]   700008F8    //"Denoise1_iWideDenoise1_iWideWide"
{0x0F12, 0x030A},	//020A	//TVAR_afit_pBaseVals[105]   700008FA    //"Demosaic4_iHystGrayRangeUVDenoise_iYSupport"                             
{0x0F12, 0x1403},	//0A03	//TVAR_afit_pBaseVals[106]   700008FC    //"UVDenoise_iUVSupportDSMix1_iLowPower_Wide"                               
{0x0F12, 0x0A11},	//0A11	//TVAR_afit_pBaseVals[107]   700008FE    //"DSMix1_iLowPower_Wide_BinDSMix1_iHighPower_Wide"                         
{0x0F12, 0x0A0F},	//0A0F	//TVAR_afit_pBaseVals[108]   70000900    //"DSMix1_iHighPower_Wide_BinDSMix1_iLowThresh_Wide"                        
{0x0F12, 0x050A},	//050A	//TVAR_afit_pBaseVals[109]   70000902    //"DSMix1_iHighThresh_WideDSMix1_iReduceNegativeWide"                       
{0x0F12, 0x101E},	//14 //1E //101E	//TVAR_afit_pBaseVals[110]   70000904    //"DSMix1_iLowPower_FineDSMix1_iLowPower_Fine_Bin"                          
{0x0F12, 0x101E},	//101E	//TVAR_afit_pBaseVals[111]   70000906    //"DSMix1_iHighPower_FineDSMix1_iHighPower_Fine_Bin"                        
{0x0F12, 0x0A08},	//3030	//TVAR_afit_pBaseVals[112]   70000908    //"DSMix1_iLowThresh_FineDSMix1_iHighThresh_Fine"                           
{0x0F12, 0x0005},	//0005	//TVAR_afit_pBaseVals[113]   7000090A    //"DSMix1_iReduceNegativeFineDSMix1_iRGBMultiplier"                         
{0x0F12, 0x0400},	//0400	//TVAR_afit_pBaseVals[114]   7000090C    //"Mixer1_iNLowNoisePowerMixer1_iNLowNoisePower_Bin"
{0x0F12, 0x0400},	//0400	//TVAR_afit_pBaseVals[115]   7000090E    //"Mixer1_iNVeryLowNoisePowerMixer1_iNVeryLowNoisePower_Bin"
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[116]   70000910    //"Mixer1_iNHighNoisePowerMixer1_iNHighNoisePower_Bin"
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[117]   70000912    //"Mixer1_iWLowNoisePowerMixer1_iWVeryLowNoisePower"
{0x0F12, 0x0A00},	//0A00	//TVAR_afit_pBaseVals[118]   70000914    //"Mixer1_iWHighNoisePowerMixer1_iWLowNoiseCeilGain"
{0x0F12, 0x000A},	//100A	//TVAR_afit_pBaseVals[119]   70000916    //"Mixer1_iWHighNoiseCeilGainMixer1_iWNoiseCeilGain"
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[120]   70000918            //"CCM_Oscar_iSaturationCCM_Oscar_bSaturation"                              
{0x0F12, 0x0154},	//TVAR_afit_pBaseVals[121]   7000091A            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"
{0x0F12, 0x0100},	//TVAR_afit_pBaseVals[122]   7000091C            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"
{0x0F12, 0x8020},	//TVAR_afit_pBaseVals[123]   7000091E            //"byr_gas2_iShadingPowerRGB2YUV_iRGBGain"                                  
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[124]   70000920            //"RGB2YUV_iSaturationRGB2YUV_bGainOffset"                                  
{0x0F12, 0x000A},	//07 //0 	//TVAR_afit_pBaseVals[125]   70000922            //RGB2YUV_iYOffset
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[126]   70000924            //BRIGHTNESS                                                                
{0x0F12, 0x0024},	//TVAR_afit_pBaseVals[127]   70000926            //CONTRAST                                                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[128]   70000928            //SATURATION                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[129]   7000092A            //SHARP_BLUR                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[130]   7000092C            //GLAMOUR
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[131]   7000092E    //Disparity_iSatSat                                                         
{0x0F12, 0x000A},	//0e //08 //000E	//TVAR_afit_pBaseVals[132]   70000930    //Denoise1_iYDenThreshLow
{0x0F12, 0x0006},	//0006	//TVAR_afit_pBaseVals[133]   70000932    //Denoise1_iYDenThreshLow_Bin
{0x0F12, 0x0040},	//50 //0064	//TVAR_afit_pBaseVals[134]   70000934    //Denoise1_iYDenThreshHigh
{0x0F12, 0x0050},	//0050	//TVAR_afit_pBaseVals[135]   70000936    //Denoise1_iYDenThreshHigh_Bin
{0x0F12, 0x0002},	//0002	//TVAR_afit_pBaseVals[136]   70000938    //Denoise1_iLowWWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[137]   7000093A    //Denoise1_iHighWWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[138]   7000093C    //Denoise1_iLowWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[139]   7000093E    //Denoise1_iHighWideThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[140]   70000940    //Denoise1_iSatSat                                                          
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[141]   70000942    //Demosaic4_iHystGrayLow
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[142]   70000944    //Demosaic4_iHystGrayHigh
{0x0F12, 0x0014},	//0014	//TVAR_afit_pBaseVals[143]   70000946    //UVDenoise_iYLowThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[144]   70000948    //UVDenoise_iYHighThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[145]   7000094A    //UVDenoise_iUVLowThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[146]   7000094C    //UVDenoise_iUVHighThresh
{0x0F12, 0x001C},	//000a	//TVAR_afit_pBaseVals[147]   7000094E    //DSMix1_iLowLimit_Wide                                                     
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[148]   70000950    //DSMix1_iLowLimit_Wide_Bin                                                 
{0x0F12, 0x000A},	//0014	//TVAR_afit_pBaseVals[149]   70000952    //DSMix1_iHighLimit_Wide                                                    
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[150]   70000954    //DSMix1_iHighLimit_Wide_Bin                                                
{0x0F12, 0x0028},	//0050	//TVAR_afit_pBaseVals[151]   70000956    //DSMix1_iLowLimit_Fine                                                     
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[152]   70000958    //DSMix1_iLowLimit_Fine_Bin                                                 
{0x0F12, 0x0010},	//0010	//TVAR_afit_pBaseVals[153]   7000095A    //DSMix1_iHighLimit_Fine                                                    
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[154]   7000095C    //DSMix1_iHighLimit_Fine_Bin                                                
{0x0F12, 0x0106},	//0106	//TVAR_afit_pBaseVals[155]   7000095E    //DSMix1_iRGBOffset                                                         
{0x0F12, 0x006F},	//006F	//TVAR_afit_pBaseVals[156]   70000960    //DSMix1_iDemClamp                                                          
{0x0F12, 0x0205},	//020A	//TVAR_afit_pBaseVals[157]   70000962    //"Disparity_iDispTH_LowDisparity_iDispTH_Low_Bin"
{0x0F12, 0x0505},	//050A	//TVAR_afit_pBaseVals[158]   70000964    //"Disparity_iDispTH_High Disparity_iDispTH_High_Bin"
{0x0F12, 0x0101},	//0101	//TVAR_afit_pBaseVals[159]   70000966    //"Despeckle_iCorrectionLevelColdDespeckle_iCorrectionLevelCold_Bin"
{0x0F12, 0x0202},	//0102	//TVAR_afit_pBaseVals[160]   70000968    //Despeckle_iCorrectionLevelHotDespeckle_iCorrectionLevelHot_Bin
{0x0F12, 0x140A},	//140A	//TVAR_afit_pBaseVals[161]   7000096A    //"Despeckle_iColdThreshLowDespeckle_iColdThreshHigh"
{0x0F12, 0x140A},	//140A	//TVAR_afit_pBaseVals[162]   7000096C    //"Despeckle_iHotThreshLowDespeckle_iHotThreshHigh"
{0x0F12, 0x2828},	//2828	//TVAR_afit_pBaseVals[163]   7000096E    //"Denoise1_iLowMaxSlopeAllowedDenoise1_iHighMaxSlopeAllowed"               
{0x0F12, 0x0606},	//0606	//TVAR_afit_pBaseVals[164]   70000970    //"Denoise1_iLowSlopeThreshDenoise1_iHighSlopeThresh"                       
{0x0F12, 0x0205},	//0205	//TVAR_afit_pBaseVals[165]   70000972    //"Denoise1_iRadialPowerDenoise1_iRadialDivideShift"                        
{0x0F12, 0x0480},	//0480	//TVAR_afit_pBaseVals[166]   70000974    //"Denoise1_iRadialLimitDenoise1_iLWBNoise"
{0x0F12, 0x000A},	//000F	//TVAR_afit_pBaseVals[167]   70000976    //"Denoise1_iWideDenoise1_iWideWide"
{0x0F12, 0x0005},	//0005	//TVAR_afit_pBaseVals[168]   70000978    //"Demosaic4_iHystGrayRangeUVDenoise_iYSupport"                             
{0x0F12, 0x1903},	//1903	//TVAR_afit_pBaseVals[169]   7000097A    //"UVDenoise_iUVSupportDSMix1_iLowPower_Wide"                               
{0x0F12, 0x1611},	//0f11 //1911	//1911	//TVAR_afit_pBaseVals[170]   7000097C    //"DSMix1_iLowPower_Wide_BinDSMix1_iHighPower_Wide"                         
{0x0F12, 0x0A0F},	//0A0F	//TVAR_afit_pBaseVals[171]   7000097E    //"DSMix1_iHighPower_Wide_BinDSMix1_iLowThresh_Wide"                        
{0x0F12, 0x050A},	//050A	//TVAR_afit_pBaseVals[172]   70000980    //"DSMix1_iHighThresh_WideDSMix1_iReduceNegativeWide"                       
{0x0F12, 0x2025},	//14 //28	//2028	//TVAR_afit_pBaseVals[173]   70000982    //"DSMix1_iLowPower_FineDSMix1_iLowPower_Fine_Bin"                          
{0x0F12, 0x2025},	//1e //28	//2028	//TVAR_afit_pBaseVals[174]   70000984    //"DSMix1_iHighPower_FineDSMix1_iHighPower_Fine_Bin"                        
{0x0F12, 0x0A08},	//2000	//TVAR_afit_pBaseVals[175]   70000986    //"DSMix1_iLowThresh_FineDSMix1_iHighThresh_Fine"                           
{0x0F12, 0x0007},	//0007	//TVAR_afit_pBaseVals[176]   70000988    //"DSMix1_iReduceNegativeFineDSMix1_iRGBMultiplier"                         
{0x0F12, 0x0403},	//0403	//TVAR_afit_pBaseVals[177]   7000098A    //"Mixer1_iNLowNoisePowerMixer1_iNLowNoisePower_Bin"
{0x0F12, 0x0402},	//0402	//TVAR_afit_pBaseVals[178]   7000098C    //"Mixer1_iNVeryLowNoisePowerMixer1_iNVeryLowNoisePower_Bin"
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[179]   7000098E    //"Mixer1_iNHighNoisePowerMixer1_iNHighNoisePower_Bin"
{0x0F12, 0x0203},	//0203	//TVAR_afit_pBaseVals[180]   70000990    //"Mixer1_iWLowNoisePowerMixer1_iWVeryLowNoisePower"
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[181]   70000992    //"Mixer1_iWHighNoisePowerMixer1_iWLowNoiseCeilGain"
{0x0F12, 0x0006},	//1006	//TVAR_afit_pBaseVals[182]   70000994    //"Mixer1_iWHighNoiseCeilGainMixer1_iWNoiseCeilGain"
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[183]   70000996            //"CCM_Oscar_iSaturationCCM_Oscar_bSaturation"                              
{0x0F12, 0x0173},	//TVAR_afit_pBaseVals[184]   70000998            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"
{0x0F12, 0x0100},	//TVAR_afit_pBaseVals[185]   7000099A            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"
{0x0F12, 0x8032},	//TVAR_afit_pBaseVals[186]   7000099C            //"byr_gas2_iShadingPowerRGB2YUV_iRGBGain"                                  
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[187]   7000099E            //"RGB2YUV_iSaturationRGB2YUV_bGainOffset"                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[188]   700009A0            //RGB2YUV_iYOffset
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[189]   700009A2            //BRIGHTNESS                                                                
{0x0F12, 0x0014},	//TVAR_afit_pBaseVals[190]   700009A4            //CONTRAST                                                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[191]   700009A6            //SATURATION                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[192]   700009A8            //SHARP_BLUR                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[193]   700009AA            //GLAMOUR
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[194]   700009AC    //Disparity_iSatSat                                                         
{0x0F12, 0x000A},	//0e //08	//000E	//TVAR_afit_pBaseVals[195]   700009AE    //Denoise1_iYDenThreshLow                                                   
{0x0F12, 0x0006},	//0006	//TVAR_afit_pBaseVals[196]   700009B0    //Denoise1_iYDenThreshLow_Bin
{0x0F12, 0x0040},	//50	//0064	//TVAR_afit_pBaseVals[197]   700009B2    //Denoise1_iYDenThreshHigh                                                  
{0x0F12, 0x0050},	//0050	//TVAR_afit_pBaseVals[198]   700009B4    //Denoise1_iYDenThreshHigh_Bin
{0x0F12, 0x0002},	//0002	//TVAR_afit_pBaseVals[199]   700009B6    //Denoise1_iLowWWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[200]   700009B8    //Denoise1_iHighWWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[201]   700009BA    //Denoise1_iLowWideThresh
{0x0F12, 0x000A},	//000A	//TVAR_afit_pBaseVals[202]   700009BC    //Denoise1_iHighWideThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[203]   700009BE    //Denoise1_iSatSat                                                          
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[204]   700009C0    //Demosaic4_iHystGrayLow
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[205]   700009C2    //Demosaic4_iHystGrayHigh
{0x0F12, 0x0014},	//0014	//TVAR_afit_pBaseVals[206]   700009C4    //UVDenoise_iYLowThresh
{0x0F12, 0x0032},	//000A	//TVAR_afit_pBaseVals[207]   700009C6    //UVDenoise_iYHighThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[208]   700009C8    //UVDenoise_iUVLowThresh
{0x0F12, 0x03FF},	//03FF	//TVAR_afit_pBaseVals[209]   700009CA    //UVDenoise_iUVHighThresh
{0x0F12, 0x001C},	//000a	//TVAR_afit_pBaseVals[210]   700009CC    //DSMix1_iLowLimit_Wide                                                     
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[211]   700009CE    //DSMix1_iLowLimit_Wide_Bin                                                 
{0x0F12, 0x000A},	//0014	//TVAR_afit_pBaseVals[212]   700009D0    //DSMix1_iHighLimit_Wide                                                    
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[213]   700009D2    //DSMix1_iHighLimit_Wide_Bin                                                
{0x0F12, 0x0028},	//0050	//TVAR_afit_pBaseVals[214]   700009D4    //DSMix1_iLowLimit_Fine                                                     
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[215]   700009D6    //DSMix1_iLowLimit_Fine_Bin                                                 
{0x0F12, 0x0010},	//0010	//TVAR_afit_pBaseVals[216]   700009D8    //DSMix1_iHighLimit_Fine                                                    
{0x0F12, 0x0032},	//0032	//TVAR_afit_pBaseVals[217]   700009DA    //DSMix1_iHighLimit_Fine_Bin                                                
{0x0F12, 0x0106},	//0106	//TVAR_afit_pBaseVals[218]   700009DC    //DSMix1_iRGBOffset                                                         
{0x0F12, 0x006F},	//006F	//TVAR_afit_pBaseVals[219]   700009DE    //DSMix1_iDemClamp                                                          
{0x0F12, 0x0205},	//0205	//TVAR_afit_pBaseVals[220]   700009E0    //"Disparity_iDispTH_LowDisparity_iDispTH_Low_Bin"
{0x0F12, 0x0505},	//0505	//TVAR_afit_pBaseVals[221]   700009E2    //"Disparity_iDispTH_High Disparity_iDispTH_High_Bin"
{0x0F12, 0x0101},	//0101	//TVAR_afit_pBaseVals[222]   700009E4    //"Despeckle_iCorrectionLevelColdDespeckle_iCorrectionLevelCold_Bin"
{0x0F12, 0x0202},	//0102	//TVAR_afit_pBaseVals[223]   700009E6    //Despeckle_iCorrectionLevelHotDespeckle_iCorrectionLevelHot_Bin
{0x0F12, 0x140A},	//140A	//TVAR_afit_pBaseVals[224]   700009E8    //"Despeckle_iColdThreshLowDespeckle_iColdThreshHigh"
{0x0F12, 0x140A},	//140A	//TVAR_afit_pBaseVals[225]   700009EA    //"Despeckle_iHotThreshLowDespeckle_iHotThreshHigh"
{0x0F12, 0x2828},	//2828	//TVAR_afit_pBaseVals[226]   700009EC    //"Denoise1_iLowMaxSlopeAllowedDenoise1_iHighMaxSlopeAllowed"               
{0x0F12, 0x0606},	//0606	//TVAR_afit_pBaseVals[227]   700009EE    //"Denoise1_iLowSlopeThreshDenoise1_iHighSlopeThresh"                       
{0x0F12, 0x0205},	//0205	//TVAR_afit_pBaseVals[228]   700009F0    //"Denoise1_iRadialPowerDenoise1_iRadialDivideShift"                        
{0x0F12, 0x0480},	//0480	//TVAR_afit_pBaseVals[229]   700009F2    //"Denoise1_iRadialLimitDenoise1_iLWBNoise"
{0x0F12, 0x000A},	//000F	//TVAR_afit_pBaseVals[230]   700009F4    //"Denoise1_iWideDenoise1_iWideWide"
{0x0F12, 0x0005},	//0005	//TVAR_afit_pBaseVals[231]   700009F6    //"Demosaic4_iHystGrayRangeUVDenoise_iYSupport"                             
{0x0F12, 0x1903},	//1903	//TVAR_afit_pBaseVals[232]   700009F8    //"UVDenoise_iUVSupportDSMix1_iLowPower_Wide"                               
{0x0F12, 0x1611},	//0f11 //1911	//1911	//TVAR_afit_pBaseVals[233]   700009FA    //"DSMix1_iLowPower_Wide_BinDSMix1_iHighPower_Wide"                         
{0x0F12, 0x0A0F},	//0A0F	//TVAR_afit_pBaseVals[234]   700009FC    //"DSMix1_iHighPower_Wide_BinDSMix1_iLowThresh_Wide"                        
{0x0F12, 0x050A},	//050A	//TVAR_afit_pBaseVals[235]   700009FE    //"DSMix1_iHighThresh_WideDSMix1_iReduceNegativeWide"                       
{0x0F12, 0x2025},	//14 //28	//2028	//TVAR_afit_pBaseVals[236]   70000A00    //"DSMix1_iLowPower_FineDSMix1_iLowPower_Fine_Bin"                          
{0x0F12, 0x2025},	//1E //28	//2028	//TVAR_afit_pBaseVals[237]   70000A02    //"DSMix1_iHighPower_FineDSMix1_iHighPower_Fine_Bin"                        
{0x0F12, 0x0A08},	//2000	//TVAR_afit_pBaseVals[238]   70000A04    //"DSMix1_iLowThresh_FineDSMix1_iHighThresh_Fine"                           
{0x0F12, 0x0007},	//0007	//TVAR_afit_pBaseVals[239]   70000A06    //"DSMix1_iReduceNegativeFineDSMix1_iRGBMultiplier"                         
{0x0F12, 0x0403},	//0403	//TVAR_afit_pBaseVals[240]   70000A08    //"Mixer1_iNLowNoisePowerMixer1_iNLowNoisePower_Bin"
{0x0F12, 0x0402},	//0402	//TVAR_afit_pBaseVals[241]   70000A0A    //"Mixer1_iNVeryLowNoisePowerMixer1_iNVeryLowNoisePower_Bin"
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[242]   70000A0C    //"Mixer1_iNHighNoisePowerMixer1_iNHighNoisePower_Bin"
{0x0F12, 0x0203},	//0203	//TVAR_afit_pBaseVals[243]   70000A0E    //"Mixer1_iWLowNoisePowerMixer1_iWVeryLowNoisePower"
{0x0F12, 0x0000},	//0000	//TVAR_afit_pBaseVals[244]   70000A10    //"Mixer1_iWHighNoisePowerMixer1_iWLowNoiseCeilGain"
{0x0F12, 0x0006},	//1006	//TVAR_afit_pBaseVals[245]   70000A12    //"Mixer1_iWHighNoiseCeilGainMixer1_iWNoiseCeilGain"
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[246]   70000A14            //"CCM_Oscar_iSaturationCCM_Oscar_bSaturation"                              
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[247]   70000A16            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"
{0x0F12, 0x0100},	//TVAR_afit_pBaseVals[248]   70000A18            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"
{0x0F12, 0x803C},	//TVAR_afit_pBaseVals[249]   70000A1A            //"byr_gas2_iShadingPowerRGB2YUV_iRGBGain"                                  
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[250]   70000A1C            //"RGB2YUV_iSaturationRGB2YUV_bGainOffset"                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[251]   70000A1E            //RGB2YUV_iYOffset
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[252]   70000A20                //BRIGHTNESS                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[253]   70000A22                //CONTRAST                                                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[254]   70000A24                //SATURATION                                                                
{0x0F12, 0x0014},	//TVAR_afit_pBaseVals[255]   70000A26                //SHARP_BLUR                                                                
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[256]   70000A28                //GLAMOUR
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[257]   70000A2A                //Disparity_iSatSat                                                         
{0x0F12, 0x000E},	//TVAR_afit_pBaseVals[258]   70000A2C                //Denoise1_iYDenThreshLow
{0x0F12, 0x0006},	//TVAR_afit_pBaseVals[259]   70000A2E                //Denoise1_iYDenThreshLow_Bin
{0x0F12, 0x0020},	//TVAR_afit_pBaseVals[260]   70000A30                //Denoise1_iYDenThreshHigh
{0x0F12, 0x0050},	//TVAR_afit_pBaseVals[261]   70000A32                //Denoise1_iYDenThreshHigh_Bin
{0x0F12, 0x0002},	//TVAR_afit_pBaseVals[262]   70000A34                //Denoise1_iLowWWideThresh
{0x0F12, 0x000A},	//TVAR_afit_pBaseVals[263]   70000A36                //Denoise1_iHighWWideThresh
{0x0F12, 0x000A},	//TVAR_afit_pBaseVals[264]   70000A38                //Denoise1_iLowWideThresh
{0x0F12, 0x000A},	//TVAR_afit_pBaseVals[265]   70000A3A                //Denoise1_iHighWideThresh
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[266]   70000A3C                //Denoise1_iSatSat                                                          
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[267]   70000A3E                //Demosaic4_iHystGrayLow
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[268]   70000A40                //Demosaic4_iHystGrayHigh
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[269]   70000A42                //UVDenoise_iYLowThresh
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[270]   70000A44                //UVDenoise_iYHighThresh
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[271]   70000A46                //UVDenoise_iUVLowThresh
{0x0F12, 0x03FF},	//TVAR_afit_pBaseVals[272]   70000A48                //UVDenoise_iUVHighThresh
{0x0F12, 0x0014},	//TVAR_afit_pBaseVals[273]   70000A4A                //DSMix1_iLowLimit_Wide                                                     
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[274]   70000A4C                //DSMix1_iLowLimit_Wide_Bin                                                 
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[275]   70000A4E                //DSMix1_iHighLimit_Wide                                                    
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[276]   70000A50                //DSMix1_iHighLimit_Wide_Bin                                                
{0x0F12, 0x0020},	//TVAR_afit_pBaseVals[277]   70000A52                //DSMix1_iLowLimit_Fine                                                     
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[278]   70000A54                //DSMix1_iLowLimit_Fine_Bin                                                 
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[279]   70000A56                //DSMix1_iHighLimit_Fine                                                    
{0x0F12, 0x0032},	//TVAR_afit_pBaseVals[280]   70000A58                //DSMix1_iHighLimit_Fine_Bin                                                
{0x0F12, 0x0106},	//TVAR_afit_pBaseVals[281]   70000A5A                //DSMix1_iRGBOffset                                                         
{0x0F12, 0x006F},	//TVAR_afit_pBaseVals[282]   70000A5C                //DSMix1_iDemClamp                                                          
{0x0F12, 0x0202},	//TVAR_afit_pBaseVals[283]   70000A5E                //"Disparity_iDispTH_LowDisparity_iDispTH_Low_Bin"
{0x0F12, 0x0502},	//TVAR_afit_pBaseVals[284]   70000A60                //"Disparity_iDispTH_High Disparity_iDispTH_High_Bin"
{0x0F12, 0x0101},	//TVAR_afit_pBaseVals[285]   70000A62                //"Despeckle_iCorrectionLevelColdDespeckle_iCorrectionLevelCold_Bin"
{0x0F12, 0x0202},	//TVAR_afit_pBaseVals[286]   70000A64                //Despeckle_iCorrectionLevelHotDespeckle_iCorrectionLevelHot_Bin
{0x0F12, 0x140A},	//TVAR_afit_pBaseVals[287]   70000A66                //"Despeckle_iColdThreshLowDespeckle_iColdThreshHigh"
{0x0F12, 0x140A},	//TVAR_afit_pBaseVals[288]   70000A68                //"Despeckle_iHotThreshLowDespeckle_iHotThreshHigh"
{0x0F12, 0x2828},	//TVAR_afit_pBaseVals[289]   70000A6A                //"Denoise1_iLowMaxSlopeAllowedDenoise1_iHighMaxSlopeAllowed"               
{0x0F12, 0x0606},	//TVAR_afit_pBaseVals[290]   70000A6C                //"Denoise1_iLowSlopeThreshDenoise1_iHighSlopeThresh"                       
{0x0F12, 0x0205},	//TVAR_afit_pBaseVals[291]   70000A6E                //"Denoise1_iRadialPowerDenoise1_iRadialDivideShift"                        
{0x0F12, 0x0880},	//TVAR_afit_pBaseVals[292]   70000A70                //"Denoise1_iRadialLimitDenoise1_iLWBNoise"
{0x0F12, 0x000F},	//TVAR_afit_pBaseVals[293]   70000A72                //"Denoise1_iWideDenoise1_iWideWide"
{0x0F12, 0x0005},	//TVAR_afit_pBaseVals[294]   70000A74                //"Demosaic4_iHystGrayRangeUVDenoise_iYSupport"                             
{0x0F12, 0x1903},	//TVAR_afit_pBaseVals[295]   70000A76                //"UVDenoise_iUVSupportDSMix1_iLowPower_Wide"                               
{0x0F12, 0x1611},	//0f11 //1911	//TVAR_afit_pBaseVals[296]   70000A78                //"DSMix1_iLowPower_Wide_BinDSMix1_iHighPower_Wide"                         
{0x0F12, 0x0A0F},	//TVAR_afit_pBaseVals[297]   70000A7A                //"DSMix1_iHighPower_Wide_BinDSMix1_iLowThresh_Wide"                        
{0x0F12, 0x050A},	//TVAR_afit_pBaseVals[298]   70000A7C                //"DSMix1_iHighThresh_WideDSMix1_iReduceNegativeWide"                       
{0x0F12, 0x2020},	//14 //20	//TVAR_afit_pBaseVals[299]   70000A7E                //"DSMix1_iLowPower_FineDSMix1_iLowPower_Fine_Bin"                          
{0x0F12, 0x2020},	//1e //20	//TVAR_afit_pBaseVals[300]   70000A80                //"DSMix1_iHighPower_FineDSMix1_iHighPower_Fine_Bin"                        
{0x0F12, 0x0A08},	//TVAR_afit_pBaseVals[301]   70000A82                //"DSMix1_iLowThresh_FineDSMix1_iHighThresh_Fine"                           
{0x0F12, 0x0007},	//TVAR_afit_pBaseVals[302]   70000A84                //"DSMix1_iReduceNegativeFineDSMix1_iRGBMultiplier"                         
{0x0F12, 0x0408},	//TVAR_afit_pBaseVals[303]   70000A86                //"Mixer1_iNLowNoisePowerMixer1_iNLowNoisePower_Bin"
{0x0F12, 0x0406},	//TVAR_afit_pBaseVals[304]   70000A88                //"Mixer1_iNVeryLowNoisePowerMixer1_iNVeryLowNoisePower_Bin"
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[305]   70000A8A                //"Mixer1_iNHighNoisePowerMixer1_iNHighNoisePower_Bin"
{0x0F12, 0x0608},	//TVAR_afit_pBaseVals[306]   70000A8C                //"Mixer1_iWLowNoisePowerMixer1_iWVeryLowNoisePower"
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[307]   70000A8E                //"Mixer1_iWHighNoisePowerMixer1_iWLowNoiseCeilGain"
{0x0F12, 0x0006},	//TVAR_afit_pBaseVals[308]   70000A90                //"Mixer1_iWHighNoiseCeilGainMixer1_iWNoiseCeilGain"
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[309]   70000A92 //180 173 164  //"CCM_Oscar_iSaturationCCM_Oscar_bSaturation"                              
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[310]   70000A94 //Linearity    //"RGBGamma2_iLinearityRGBGamma2_bLinearity"
{0x0F12, 0x0100},	//TVAR_afit_pBaseVals[311]   70000A96                //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"
{0x0F12, 0x804A},	//TVAR_afit_pBaseVals[312]   70000A98                //"byr_gas2_iShadingPowerRGB2YUV_iRGBGain"                                  
{0x0F12, 0x0180},	//TVAR_afit_pBaseVals[313]   70000A9A                //"RGB2YUV_iSaturationRGB2YUV_bGainOffset"                                  
{0x0F12, 0x0000},	//TVAR_afit_pBaseVals[314]   70000A9C                //RGB2YUV_iYOffset
{0x0F12, 0x00FF},	//afit_pConstBaseVals[0]                            //Denoise1_iUVDenThreshLow
{0x0F12, 0x00FF},	//afit_pConstBaseVals[1]                            //Denoise1_iUVDenThreshHigh
{0x0F12, 0x0800},	//afit_pConstBaseVals[2]                            //Denoise1_sensor_width
{0x0F12, 0x0600},	//afit_pConstBaseVals[3]                            //Denoise1_sensor_height
{0x0F12, 0x0000},	//afit_pConstBaseVals[4]                            //Denoise1_start_x
{0x0F12, 0x0000},	//afit_pConstBaseVals[5]                            //Denoise1_start_y
{0x0F12, 0x0000},	//afit_pConstBaseVals[6]                            //"Denoise1_iYDenSmoothDenoise1_iWSharp  "             
{0x0F12, 0x0300},	//afit_pConstBaseVals[7]                            //"Denoise1_iWWSharp Denoise1_iRadialTune  "           
{0x0F12, 0x0002},	//afit_pConstBaseVals[8]                            //"Denoise1_iOutputBrightnessDenoise1_binning_x  "
{0x0F12, 0x0400},	//afit_pConstBaseVals[9]                            //"Denoise1_binning_yDemosaic4_iFDeriv  "
{0x0F12, 0x0106},	//afit_pConstBaseVals[10]                           //"Demosaic4_iFDerivNeiDemosaic4_iSDeriv  "            
{0x0F12, 0x0005},	//afit_pConstBaseVals[11]                           //"Demosaic4_iSDerivNeiDemosaic4_iEnhancerG  "         
{0x0F12, 0x0000},	//afit_pConstBaseVals[12]                           //"Demosaic4_iEnhancerRBDemosaic4_iEnhancerV  "
{0x0F12, 0x0703},	//afit_pConstBaseVals[13]                           //"Demosaic4_iDecisionThreshDemosaic4_iDesatThresh"
{0x0F12, 0x0000},	//afit_pConstBaseVals[14]                           //Demosaic4_iBypassSelect                              
{0x0F12, 0xFFD6},	//afit_pConstBaseVals[15]
{0x0F12, 0x53C1},	//afit_pConstBaseVals[16]//hys off : 4341
{0x0F12, 0xE1FE},	//afit_pConstBaseVals[17]//mixer on :E0FA
{0x0F12, 0x0001},	//afit_pConstBaseVals[18]


//================================================================
// Flicker
//================================================================
{0x1000, 0x0001},	//Set host interrupt so main start run
{0xFFFE, 0x000a},		//Wait 10mSec

{0x002A, 0x0400},
{0x0F12, 0x005F},	//REG_TC_DBG_AutoAlgEnBits
{0x002A, 0x03DC},
{0x0F12, 0x0001},	//REG_SF_USER_FlickerQuant	1:50hz, 2:60hz 
{0x0F12, 0x0001},	//REG_SF_USER_FlickerQuantChanged


//================================================================
// Clock Settings
//================================================================
// External Clock : 24Mhz
{0x002A, 0x01B8},
{0x0F12, 0x5DC0},	//REG_TC_IPRM_InClockLSBs		//700001B8	//5DC0:24Mhz
{0x0F12, 0x0000},	//REG_TC_IPRM_InClockMSBs		//700001BA

// Pararrel & MIPI Clock Setting
{0x002A, 0x01C6},
{0x0F12, 0x0001},	//REG_TC_IPRM_UseNPviClocks		//700001C6	//1 Parallel Configurations
{0x0F12, 0x0001},	//REG_TC_IPRM_UseNMipiClocks		//700001C8	//1 MIPI Configurations

// System Clock0 : 28Mhz, PCLK : 48Mhz
{0x002A, 0x01CC},
{0x0F12, 0x1B58},	//REG_TC_IPRM_OpClk4KHz_0	//700001CC
{0x0F12, 0x2EE0},	//REG_TC_IPRM_MinOutRate4KHz_0	//700001CE
{0x0F12, 0x2EE0},	//REG_TC_IPRM_MaxOutRate4KHz_0	//700001D0

 //System clock1 : 28Mhz, PCLK : 48Mhz
{0x002A, 0x01D2},
{0x0F12, 0x1B58},	//REG_TC_IPRM_OpClk4KHz_1	//700001D2
{0x0F12, 0x2EE0},	//REG_TC_IPRM_MinOutRate4KHz_1	//700001D4
{0x0F12, 0x2EE0},	//REG_TC_IPRM_MaxOutRate4KHz_1	//700001D6

// System clock2 : 24Mhz, PCLK : 24Mhz                                      
//s002A01D8                                                                 
//s0F121770	//REG_TC_IPRM_OpClk4KHz_2	//700001D8	//1770:24Mhz
//s0F121770  	//REG_TC_IPRM_MinOutRate4KHz_2	//700001DA	//1770:24Mhz
//s0F121770	//REG_TC_IPRM_MaxOutRate4KHz_2	//700001DC	//1770:24Mhz

{0x002A, 0x01E0},
{0x0F12, 0x0001},	//REG_TC_IPRM_InitParamsUpdated	//700001E0
{0xFFFE, 0x0064},		//Delay 100ms


//================================================================
// Preview Configuration
//================================================================
// Preview 0 for 640x480, 8-15fps
{0x002A, 0x0242},
{0x0F12, 0x0280},	//REG_0TC_PCFG_usWidth
{0x0F12, 0x01E0},	//REG_0TC_PCFG_usHeight
{0x0F12, 0x0005},	//REG_0TC_PCFG_Format
{0x002A, 0x024E},
{0x0F12, 0x0001},	//REG_0TC_PCFG_uClockInd
{0x002A, 0x0248},
{0x0F12, 0x2EE0},	//REG_0TC_PCFG_usMaxOut4KHzRate
{0x0F12, 0x2EE0},	//REG_0TC_PCFG_usMinOut4KHzRate
{0x0F12, 0x0052},	//REG_0TC_PCFG_PVIMask
{0x002A, 0x0252},
{0x0F12, 0x0001},	//REG_0TC_PCFG_FrRateQualityType
{0x002A, 0x0250},
{0x0F12, 0x0000},	//REG_0TC_PCFG_usFrTimeType
{0x002A, 0x0262},
{0x0F12, 0x0000},	//REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy
{0x0F12, 0x0000},	//REG_0TC_PCFG_uCaptureMirror
{0x002A, 0x0254},
{0x0F12, 0x0535},	//REG_0TC_PCFG_usMaxFrTimeMsecMult10
{0x0F12, 0x029A},	//REG_0TC_PCFG_usMinFrTimeMsecMult10

// Preview 1 Not USE.. But Don't Delete this configure
{0x002A, 0x0268},
{0x0F12, 0x0210},	//REG_1TC_PCFG_usWidth
{0x0F12, 0x01B0},	//REG_1TC_PCFG_usHeight
{0x0F12, 0x0005},	//REG_1TC_PCFG_Format
{0x002A, 0x0274},
{0x0F12, 0x0001},	//REG_1TC_PCFG_uClockInd
{0x002A, 0x026E},
{0x0F12, 0x2EE0},	//REG_1TC_PCFG_usMaxOut4KHzRate
{0x0F12, 0x2EE0},	//REG_1TC_PCFG_usMinOut4KHzRate
{0x0F12, 0x0052},	//REG_1TC_PCFG_PVIMask
{0x002A, 0x0278},
{0x0F12, 0x0001},	//REG_1TC_PCFG_FrRateQualityType
{0x002A, 0x0276},
{0x0F12, 0x0002},	//REG_1TC_PCFG_usFrTimeType
{0x002A, 0x0288},
{0x0F12, 0x0000},	//REG_1TC_PCFG_uPrevMirror
{0x0F12, 0x0000},	//REG_1TC_PCFG_uCaptureMirror
{0x002A, 0x027A},
{0x0F12, 0x0535},	//REG_1TC_PCFG_usMaxFrTimeMsecMult10
{0x0F12, 0x029A},	//REG_1TC_PCFG_usMinFrTimeMsecMult10


//================================================================ 
// Capture Configuration                                           
//================================================================ 
// Capture 0 for 1280x960                                         
//s002A030C
//s0F120001	//AE ON
{0x002A, 0x030E},
{0x0F12, 0x0500},	//REG_0TC_CCFG_usWidth
{0x0F12, 0x03C0},	//REG_0TC_CCFG_usHeight
{0x0F12, 0x0005},	//REG_0TC_CCFG_Format
{0x002A, 0x031A},
{0x0F12, 0x0001},	//REG_0TC_CCFG_uClockInd
{0x002A, 0x0314},
{0x0F12, 0x2EE0},	//REG_0TC_CCFG_usMaxOut4KHzRate
{0x0F12, 0x2EE0},	//REG_0TC_CCFG_usMinOut4KHzRate
{0x0F12, 0x0052},	//REG_0TC_CCFG_PVIMask
{0x002A, 0x031E},
{0x0F12, 0x0002},	//REG_0TC_CCFG_FrRateQualityType
{0x002A, 0x031C},
{0x0F12, 0x0002},	//REG_0TC_CCFG_usFrTimeType
{0x002A, 0x0320},
{0x0F12, 0x0535},	//REG_0TC_CCFG_usMaxFrTimeMsecMult10
{0x0F12, 0x0000},	//REG_0TC_CCFG_usMinFrTimeMsecMult10

{0x002A, 0x0226},
{0x0F12, 0x0001},	//REG_TC_GP_CapConfigChanged


//================================================================
// Factory Only No Delete                                         
//================================================================
{0x002A, 0x10EE},
{0x0F12, 0x097A},	// senHal_uMinColsNoBin *FACTORY ONLY *No Delete//


//================================================================
// REG TC FLS
//================================================================
{0x002A, 0x03B6},
{0x0F12, 0x0000},	//REG_TC_FLS


//================================================================
// MIPI Lane
//================================================================
{0x002A, 0x03FA},
{0x0F12, 0x0001},	//REG_TC_OIF_EnMipiLanes		//700003FA
{0x0F12, 0x00C3},	//REG_TC_OIF_EnPackets			//700003FC
{0x0F12, 0x0001},	//REG_TC_OIF_CfgChanged			//700003FE


//================================================================
//PREVIEW Command                                                 
//================================================================
{0x002A, 0x021C},
{0x0F12, 0x0000},	//REG_TC_GP_ActivePrevConfig		//7000021C
{0x002A, 0x0220},
{0x0F12, 0x0001},	//REG_TC_GP_PrevOpenAfterChange		//70000220
{0x002A, 0x01F8},
{0x0F12, 0x0001},	//REG_TC_GP_NewConfigSync		//700001F8
{0x002A, 0x021E},
{0x0F12, 0x0001},	//REG_TC_GP_PrevConfigChanged		//7000021E
{0x002A, 0x01F0},
{0x0F12, 0x0001},	//REG_TC_GP_EnablePreview		//700001F0
{0x0F12, 0x0001},	//REG_TC_GP_EnablePreviewChanged	//700001F2
{0xFFFE, 0x0096},		// Wait150ms//


//================================================================
//Input size Change (CROP)
//================================================================
{0x002A, 0x020A},
{0x0F12, 0x0500},	// REG_TC_GP_PrevZoomReqInputWidth
{0x0F12, 0x03C0},	// REG_TC_GP_PrevZoomReqInputHeight
{0x0F12, 0x0000},	// REG_TC_GP_PrevZoomReqInputWidthOfs
{0x0F12, 0x0020},	// REG_TC_GP_PrevZoomReqInputHeightOfs

{0x0F12, 0x0500},	// REG_TC_GP_CapZoomReqInputWidth
{0x0F12, 0x03C0},	// REG_TC_GP_CapZoomReqInputHeight
{0x0F12, 0x0000},	// REG_TC_GP_CapZoomReqInputWidthOfs
{0x0F12, 0x0020},	// REG_TC_GP_CapZoomReqInputHeightOfs
{0x0F12, 0x0001},	// REG_TC_GP_InputsChangeRequest//

{0xFFFE, 0x0064},		// Wait100ms//

//================================================================
// Non Continuous Mode
//================================================================
{0x0028, 0xD000},
{0x002A, 0xB0CC},
{0x0F12, 0x000B},	//Non Continuous mode
};

s5k6aa_short_t mode_preview_640x480[] =
{
{0xFCFC, 0xD000},
{0x0028, 0x7000},

{0x002A, 0x10EE},
{0x0F12, 0x097A},	// senHal_uMinColsNoBin *FACTORY ONLY *No Delete//

{0x002A, 0x021C},
{0x0F12, 0x0000},	// REG_TC_GP_ActivePrevConfig

{0x002A, 0x0220},
{0x0F12, 0x0001},	// REG_TC_GP_PrevOpenAfterChange

{0x002A, 0x01F8},
{0x0F12, 0x0001},	// REG_TC_GP_NewConfigSync

{0x002A, 0x021E},
{0x0F12, 0x0001},	// REG_TC_GP_PrevConfigChanged
{0x002A, 0x01F0},
{0x0F12, 0x0001},	// REG_TC_GP_EnablePreview
{0x0F12, 0x0001},	// REG_TC_GP_EnablePreviewChanged
{0xFFFE, 0x0096},		// Wait150ms//


{0x0028, 0xD000},	// MIPI
{0x002A, 0xB0CC},
{0x0F12, 0x000B},
};

s5k6aa_short_t mode_capture_1280x960[] =
{
{0xFCFC, 0xD000},
{0x0028, 0x7000},

{0x002A, 0x10EE},
{0x0F12, 0x0D7A},	// senHal_uMinColsNoBin *FACTORY ONLY *No Delete//

{0x002A, 0x0224},
{0x0F12, 0x0000},	// REG_TC_GP_ActiveCapConfig

{0x002A, 0x01F8},
{0x0F12, 0x0001},	// REG_TC_GP_NewConfigSync

{0x002A, 0x0226},
{0x0F12, 0x0001},	// REG_TC_GP_CapConfigChanged

{0x002A, 0x01F4},
{0x0F12, 0x0001},	// REG_TC_GP_EnableCapture
{0x0F12, 0x0001},	// REG_TC_GP_EnableCaptureChanged
{0xFFFE, 0x0096},		// Wait150ms//


{0x0028, 0xD000},	// MIPI
{0x002A, 0xB0CC},
{0x0F12, 0x000B},
};

s5k6aa_short_t mode_recording_640x480[] =
{
{0xFCFC, 0xD000},
{0x0028, 0x7000},

{0x002A, 0x10EE},
{0x0F12, 0x03CE},	//097A	//senHal_uMinColsNoBin *FACTORY ONLY *No Delete

{0x002A, 0x021C},
{0x0F12, 0x0000},	// REG_TC_GP_ActivePrevConfig

{0x002A, 0x0220},
{0x0F12, 0x0001},	// REG_TC_GP_PrevOpenAfterChange

{0x002A, 0x01F8},
{0x0F12, 0x0001},	// REG_TC_GP_NewConfigSync

{0x002A, 0x021E},
{0x0F12, 0x0001},	// REG_TC_GP_PrevConfigChanged
{0x002A, 0x01F0},
{0x0F12, 0x0001},	// REG_TC_GP_EnablePreview
{0x0F12, 0x0001},	// REG_TC_GP_EnablePreviewChanged
{0xFFFE, 0x0096},		// Wait150ms//

{0x0028, 0xD000},	// MIPI
{0x002A, 0xB0CC},
{0x0F12, 0x000B},
};

#endif
