/*
 * kona_att-gpio.h
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

#define GPIO_NC_3			3
#define GPIO_NC_47			47
#define GPIO_NC_62			62

#ifdef CONFIG_VIBETONZ
#define GPIO_VIB_ON			2
#endif

#define GPIO_MSM_FLASH_CNTL_EN		-1
#define GPIO_MSM_FLASH_NOW		-1
#define GPIO_MAIN_CAM_STBY		4
#define GPIO_CAM_MCLK			5
#define GPIO_CAM_CORE_EN		6
#define GPIO_CODEC_I2C_SDA              8
#define GPIO_CODEC_I2C_SCL              9

#ifdef CONFIG_ADC_STMPE811
#define GPIO_ADC_INT			10
#endif

#define GPIO_MXT_TS_IRQ			11
#define GPIO_TOUCH_IRQ			11

#ifdef CONFIG_VP_A2220
#define GPIO_A2220_I2C_SDA		12
#define GPIO_A2220_I2C_SCL		13
#endif
#define GPIO_SENSOR_ALS_SDA		12
#define GPIO_SENSOR_ALS_SCL		13
#ifdef CONFIG_30PIN_CONN
/*This is Dock interrupt according to the 30Pin driver flow */
#define GPIO_DOCK_INT			14
#else
#define GPIO_FPGA_CS			14
#endif

#define GPIO_TOUCH_SDA			16
#define GPIO_TOUCH_SCL			17

#define GPIO_VT_STBY			18

#define GPIO_I2C_DATA_CAM		20
#define GPIO_I2C_CLK_CAM		21
#define GPIO_FUELGAUGE_I2C_SDA		24
#define GPIO_FUELGAUGE_I2C_SCL		25
#ifdef CONFIG_ADC_STMPE811
#define GPIO_ADC_SDA			32
#define GPIO_ADC_SCL			33
#endif
#define GPIO_CHARGER_I2C_SDA            36
#define GPIO_CHARGER_I2C_SCL            37

#ifdef CONFIG_WACOM_W9001
#define GPIO_PEN_DETECT			39
#define GPIO_PEN_IRQ			42
#define GPIO_PEN_FWE1			43
#endif

#define GPIO_SENSOR_SNS_SDA		44
#define GPIO_SENSOR_SNS_SCL		45
#define GPIO_CAM_A_EN			46
#define GPIO_MAG_RST			48

#ifdef CONFIG_30PIN_CONN
/*This is Accessory interrupt according to the 30Pin driver flow */
#define GPIO_ACCESSORY_INT		49
#endif

#define GPIO_VOLUME_UP			50

#define GPIO_CAM_VTCORE_EN		51
#define GPIO_MXT_TS_LDO_EN		-1
#define GPIO_MXT_TS_RESET		-1
#define GPIO_MAG_RDY			52
#define GPIO_USB_SEL			53
#define GPIO_IRDA_EN			55
#define GPIO_CODEC_MAD_INTR		58

#ifdef CONFIG_WACOM_W9001
#define GPIO_PEN_LDO_EN			63
#define GPIO_PEN_PDCT			65
#endif

#define GPIO_ALS_RESET			66
#define GPIO_FUEL_INT			67

#define GPIO_ACCESSORY_EN		68
#define GPIO_ACC_INT_N			69

#ifdef CONFIG_VIBETONZ
#define GPIO_VIB_PWM			70
#endif

#ifdef CONFIG_IRDA_MC96
#define GPIO_IRDA_I2C_SDA		71
#define GPIO_IRDA_I2C_SCL		72
#endif

#ifdef CONFIG_SAMSUNG_CMC624
#define GPIO_IMA_I2C_SDA		73
#define GPIO_IMA_I2C_SCL		74
#endif

#define GPIO_USB_I2C_SDA		0
#define GPIO_USB_I2C_SCL		0

#define GPIO_CAM2_RST_N			76
#ifdef CONFIG_IRDA_MC96
#define GPIO_IRDA_WAKE			80
#endif

#define GPIO_VOLUME_DOWN		81

#define GPIO_KS8851_RST			89

#define GPIO_CRADLE_SW_EN		92

#define GPIO_PS_EN			97

#define GPIO_CAM1_RST_N			107

/* ES305B GPIO */
#define MSM_AUD_A2220_WAKEUP		79
#define MSM_AUD_A2220_RESET		75

/* PMIC8921 GPIO */
#ifdef CONFIG_SAMSUNG_CMC624
#define PMIC_GPIO_IMA_PWR_EN		11
#define PMIC_GPIO_IMA_CMC_EN		12
#define PMIC_GPIO_IMA_nRST		13
#define PMIC_GPIO_IMA_SLEEP		14
#define PMIC_GPIO_CMC_ESD_DET		15
#endif
#define PMIC_GPIO_TA_nCONNECTED		40 /* 16 is NC try 40  */
#define PMIC_GPIO_CHG_STAT		17
#define PMIC_GPIO_SPK_EN		18
#define PMIC_GPIO_VPS_EN		19
#define PMIC_GPIO_USEURO_SWITCH		24 /* NC */
#define PMIC_GPIO_IF_CON_SENSE		28
#define PMIC_GPIO_OTG_EN		35
#define PMIC_GPIO_USB_INT		36
#define PMIC_GPIO_USB_INT_REV03		16 /* 40 used by TA_nconnected */
#define PMIC_GPIO_CODEC_RST		38
#define PMIC_GPIO_V_ACCESSORY_OUT_5V	42
#define PMIC_GPIO_LCD_RST		43
#define PMIC_GPIO_LED_BACKLIGHT_RESET	43

#define GPIO_MHL_EN			19
#define GPIO_MHL_SDA			95
#define GPIO_MHL_SCL			96
#define GPIO_MHL_INT			99

#ifdef CONFIG_BACKLIGHT_LP8556

#define MSM_GPIO_BL_I2C_SDA 40
#define MSM_GPIO_BL_I2C_SCL 41

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
	GPIO_REV_MAX,
};

enum {
	MDP_VSYNC,
};
