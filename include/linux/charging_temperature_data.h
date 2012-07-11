/*
 * charging_temperature_data.h
 *
 * header file supporting temperature functions for Samsung device
 *
 * COPYRIGHT(C) Samsung Electronics Co., Ltd. 2012-2017 All Right Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CHARGING_TEMPERATURE_DATA_H
#define __CHARGING_TEMPERATURE_DATA_H __FILE__

#define DEFAULT_HIGH_BLOCK_TEMP	650
#define DEFAULT_HIGH_RECOVER_TEMP	430
#define DEFAULT_LOW_BLOCK_TEMP	-50
#define DEFAULT_LOW_RECOVER_TEMP	0

#if defined(CONFIG_MACH_JAGUAR)
static const int temp_table[][2] = {
	{26250,	 800},
	{26583,	 750},
	{26979,	 700},
	{27429,	 650},
	{27941,	 600},
	{28523,	 550},
	{29170,	 500},
	{29875,	 450},
	{30684,	 400},
	{31705,	 350},
	{32726,	 300},
	{33747,	 250},
	{34768,	 200},
	{35789,	 150},
	{36810,	 100},
	{37837,	  50},
	{38710,	   0},
	{39539,	 -50},
	{40269,	-100},
	{41099,	-150},
	{41859,	-200},
};

#elif defined(CONFIG_MACH_M2_ATT)
static const int temp_table[][2] = {
	{26465,	 800},
	{26749,	 750},
	{27017,	 700},
	{27414,	 650},
	{27870,	 600},
	{28424,	 550},
	{29350,	 500},
	{29831,	 450},
	{30595,	 400},
	{31561,	 350},
	{32603,	 300},
	{33647,	 250},
	{34655,	 200},
	{35741,	 150},
	{36747,	 100},
	{37755,	  50},
	{38605,	   0},
	{39412,	 -50},
	{40294,	-100},
	{40845,	-150},
	{41353,	-200},
};

#elif defined(CONFIG_MACH_M2_SPR)
static const int temp_table[][2] = {
	{26385,	 800},
	{26787,	 750},
	{27136,	 700},
	{27540,	 650},
	{28031,	 600},
	{28601,	 550},
	{29255,	 500},
	{29568,	 450},
	{30967,	 400},
	{31880,	 350},
	{32846,	 300},
	{33694,	 250},
	{34771,	 200},
	{35890,	 150},
	{37045,	 100},
	{38144,	  50},
	{39097,	   0},
	{39885,	 -50},
	{40595,	-100},
	{41190,	-150},
	{41954,	-200},
};

#elif defined(CONFIG_MACH_M2_VZW)
static const int temp_table[][2] = {
	{26537,	 800},
	{26849,	 750},
	{27211,	 700},
	{27627,	 650},
	{28117,	 600},
	{28713,	 550},
	{29403,	 500},
	{30205,	 450},
	{31075,	 400},
	{32026,	 350},
	{33014,	 300},
	{34117,	 250},
	{35115,	 200},
	{36121,	 150},
	{37212,	 100},
	{38190,	  50},
	{39006,	   0},
	{39813,	 -50},
	{40490,	-100},
	{41084,	-150},
	{41537,	-200},
};

#elif defined(CONFIG_MACH_GOGH)
static const int temp_table[][2] = {
	{24641,	 800},
	{26757,	 750},
	{27111,	 700},
	{27516,	 650},
	{27963,	 600},
	{28494,	 550},
	{29147,	 500},
	{29735,	 450},
	{30623,	 400},
	{31484,	 350},
	{32398,	 300},
	{33407,	 250},
	{34503,	 200},
	{35478,	 150},
	{36663,	 100},
	{37711,	  50},
	{38667,	   0},
	{39556,	 -50},
	{40274,	-100},
};

#else
static const int temp_table[][2] = {
	{26250,	 800},
	{26583,	 750},
	{26979,	 700},
	{27429,	 650},
	{27941,	 600},
	{28523,	 550},
	{29170,	 500},
	{29875,	 450},
	{30684,	 400},
	{31705,	 350},
	{32726,	 300},
	{33747,	 250},
	{34768,	 200},
	{35789,	 150},
	{36810,	 100},
	{37837,	  50},
	{38710,	   0},
	{39539,	 -50},
	{40269,	-100},
	{41099,	-150},
	{41859,	-200},
};
#endif

#endif /* __CHARGING_TEMPERATURE_DATA_H */
