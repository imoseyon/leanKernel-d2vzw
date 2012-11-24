/*
 * jaguar-gpio.h
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
#define GPIO_MDP_VSYNC			0
#define GPIO_MHL_RST			1
#define GPIO_MSM_FLASH_CNTL_EN	2
#define GPIO_MSM_FLASH_NOW		3
#define GPIO_MAIN_CAM_STBY		4
#define GPIO_CAM_MCLK			5
#define GPIO_CAM_CORE_EN		6
#define GPIO_CODEC_I2C_SDA		8
#define GPIO_CODEC_I2C_SCL		9
#define GPIO_MXT_TS_IRQ			11
#define GPIO_CYTTSP_TS_IRQ		11

#define GPIO_FPGA_CS			14
#define GPIO_MHL_WAKE_UP		15
#define GPIO_VT_STBY			18

#define GPIO_MHL_EN				19
#define GPIO_I2C_DATA_CAM		20
#define GPIO_I2C_CLK_CAM		21
#define GPIO_FUELGAUGE_I2C_SDA	24
#define GPIO_FUEKGAUGE_I2C_SCL	25

#define GPIO_BT_UART_TXD		26
#define GPIO_BT_UART_RXD		27
#define GPIO_BT_UART_CTS		28
#define GPIO_BT_UART_RTS		29

#define GPIO_NFC_SDA			32
#define GPIO_NFC_SCL			33

#define GPIO_TDMB_SPI_MOSI		34
#define GPIO_TDMB_SPI_MISO		35
#define GPIO_TDMB_SPI_CS		36
#define GPIO_TDMB_SPI_CLK		37

#define GPIO_SENSOR_SNS_SDA		44
#define GPIO_SENSOR_SNS_SCL		45
#define GPIO_CAM_A_EN			46
#define GPIO_HAPTIC_PWR_EN		47 /* < BOARD_REV15 */

#ifdef CONFIG_KEYBOARD_CYPRESS_TOUCH_236
#define GPIO_TOUCHKEY_INT		42
#define GPIO_TOUCHKEY_SCL		47
#define GPIO_TOUCHKEY_SDA		48
#endif

#define GPIO_HOME_KEY			49

#define GPIO_MXT_TS_LDO_EN		50
#define GPIO_CAM_SENSOR_IO_EN	51
#define GPIO_MXT_TS_RESET		52

#if defined(CONFIG_BCM4334) || defined(CONFIG_BCM4334_MODULE)
#define GPIO_WL_REG_ON			43
#define GPIO_WL_HOST_WAKE		54
#endif

#define GPIO_SENSOR_ALS_SDA		63
#define GPIO_SENSOR_ALS_SCL		64
#define GPIO_FUEL_INT			67
#define GPIO_MSENSE_RST			68
#define GPIO_MPU3050_INT		69
#define GPIO_VIB_PWM			70

#define GPIO_USB_I2C_SDA		73
#define GPIO_USB_I2C_SCL		74
#define GPIO_CAM2_RST_N			76
#define GPIO_VIB_ON				77

#define GPIO_MHL_SEL			82
#define GPIO_KS8851_RST			89

#define GPIO_KS8851_IRQ			90
#define GPIO_NFC_FIRMWARE		92
#define GPIO_MHL_SDA			95
#define GPIO_MHL_SCL			96
#define GPIO_MHL_INT			99

#define GPIO_NFC_IRQ			106
#define GPIO_CAM1_RST_N			107

/* PMIC8921 GPIO */
#define PMIC_GPIO_EAR_OUT_SEL	2
#define PMIC_GPIO_ALS_INT		6
#define PMIC_GPIO_ECOMPASS_RST	9
#define PMIC_GPIO_SPK_EN		18
#define PMIC_GPIO_FLASH_LED_UNLOCK	19
#define PMIC_GPIO_VPS_EN		20
#define PMIC_GPIO_NFC_EN		21
#define PMIC_GPIO_OTG_EN		-1
#define PMIC_GPIO_HAPTIC_PWR_EN	22
#if defined(CONFIG_CHARGER_SMB347)
#define PMIC_GPIO_CHG_EN		PMIC_GPIO_OTG_EN
#define PMIC_GPIO_CHG_STAT		17
#endif
#define PMIC_GPIO_BATT_INT		37

#define PMIC_GPIO_CODEC_RST		38
#define PMIC_GPIO_LCD_RST		43

#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
#define PMIC_GPIO_VGH_ESD_DET		44
#endif

/* gpio for changed list */
enum {
	BOARD_REV00,
	BOARD_REV01,
	BOARD_REV02,
	BOARD_REV03,
	BOARD_REV04,
	BOARD_REV05,
	BOARD_REV06,
	BOARD_REV07,
	BOARD_REV08,
	BOARD_REV09,
	BOARD_REV10,
	BOARD_REV11,
	BOARD_REV12,
	BOARD_REV13,
	BOARD_REV14,
	BOARD_REV15,
	BOARD_REV16,
	BOARD_REV17,
	BOARD_REV18,
	BOARD_REV19,
	BOARD_REV20,
	BOARD_REV21,
	BOARD_REV22,
	BOARD_REV23,
	BOARD_REV24,
	BOARD_REV25,
	GPIO_REV_MAX,
};

enum {
	VOLUME_UP,
	VOLUME_DOWN,
#ifdef CONFIG_KEYBOARD_CYPRESS_TOUCH
	TOUCH_KEY_I2C_SDA,
	TOUCH_KEY_I2C_SCL,
#endif
	MHL_EN,
	MHL_SEL,
	MDP_VSYNC,
	MHL_SDA,
	PS_EN,
	CODEC_MAD_INT_N,
	TDMB_RST,
	TDMB_EN,
	TDMB_INT,
	GPIO_MAG_RST,
	ALS_INT,
#if defined(CONFIG_OPTICAL_GP2A) || defined(CONFIG_OPTICAL_GP2AP020A00F)
	ALS_SDA,
	ALS_SCL,
#endif
	BT_HOST_WAKE,
	BT_WAKE,
	BT_EN,
};
