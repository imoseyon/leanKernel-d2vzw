/*
 * t-project-gpio.h
 *
 * header file supporting gpio functions for Samsung device
 *
 * COPYRIGHT(C) Samsung Electronics Co., Ltd. 2006-2011 All Right Reserved.
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

/* MSM8960 GPIO */
#define GPIO_MHL_EN					0
#define GPIO_HDMI_EN1				0
#define GPIO_MHL_RST				1
#define GPIO_HDMI_EN2				1
#define GPIO_VFE_CAMIF_TIMER1		2
#define GPIO_VFE_CAMIF_TIMER2		3
#define GPIO_VFE_CAMIF_TIMER3_INT	4
#define	CAM_CORE_EN					6
#define GPIO_MHL_SEL				9

#define GPIO_MXT_TS_IRQ				11
#define GPIO_CYTTSP_TS_IRQ			11
#define GPIO_TOUCH_KEY_I2C_SDA		12
#define GPIO_TOUCH_KEY_I2C_SCL		13
#define GPIO_FPGA_CS				14
#define GPIO_VFE_CAMIF_TIMER_9A     18
#define GPIO_MDP_VSYNC				19

#define GPIO_FUELGAUGE_I2C_SDA		24
#define GPIO_FUEKGAUGE_I2C_SCL		25

#define GPIO_NFC_SDA				32
#define GPIO_NFC_SCL				33

#define GPIO_SENSOR_SNS_SDA			44
#define GPIO_SENSOR_SNS_SCL			45
#define GPIO_CAM_A_EN				46

#define GPIO_MXT_TS_LDO_EN			50
#define GPIO_MXT_TS_RESET			52
#define GPIO_PS_EN					53
#define GPIO_TOUCH_KEY_INT			54
#define GPIO_MSENSE_RST				58

#define GPIO_SENSOR_ALS_SDA			63
#define GPIO_SENSOR_ALS_SCL			64
#define GPIO_FUEL_INT				67
#define GPIO_MPU3050_INT			69

#define GPIO_USB_I2C_SDA			73
#define GPIO_USB_I2C_SCL			74
#define CAM2_RST_N					76
#define GPIO_KEY_LED_EN				79

#define GPIO_KS8851_RST				89

#define GPIO_KS8851_IRQ				90
#define GPIO_NFC_FIRMWARE			92
#define GPIO_MHL_SCL				96
#define GPIO_MHL_SDA				97
#define GPIO_MHL_INT				99

#define GPIO_NFC_IRQ				106
#define CAM1_RST_N					107

/* PMIC8921 GPIO */
#define PMIC_GPIO_NFC_EN			21
#define PMIC_GPIO_OTG_EN			22
#define PMIC_GPIO_ALS_INT			42

/* gpio for changed list */
#define GPIO_REV_MAX 3
#define BOARD_REV00 0
#define BOARD_REV01 1
#define BOARD_REV02 2

enum {
	VOLUME_UP,
	VOLUME_DOWN,
	TOUCH_KEY_I2C_SDA,
	TOUCH_KEY_I2C_SCL,
};
