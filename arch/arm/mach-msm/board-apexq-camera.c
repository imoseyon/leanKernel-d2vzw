/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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

#include <asm/mach-types.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <mach/board.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "board-8960.h"
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/ctype.h>
#include <linux/regulator/consumer.h>
#include <mach/board-msm8960-camera.h>
#include <mach/gpio.h>
#include <mach/msm8960-gpio.h>
#include <mach/msm_iomap.h>
#include <mach/camera.h>
#include <linux/spi/spi.h>

extern unsigned int system_rev;

static struct i2c_board_info cam_expander_i2c_info[] = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data = &msm8960_sx150x_data[SX150X_CAM]
	},
};
static struct msm_cam_expander_info cam_expander_info[] = {
	{
		cam_expander_i2c_info,
		MSM_8960_GSBI4_QUP_I2C_BUS_ID,
	},
};

static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 1*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 2*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 3*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_5, /*active 4*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_6, /*active 5*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 6*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_3, /*active 7*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*i2c suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_KEEPER,
	},
	{
		.func = GPIOMUX_FUNC_1, /* drive strength for D2*/
		.drv = GPIOMUX_DRV_4MA,
		.pull = GPIOMUX_PULL_NONE,
	},
	{
		.func = GPIOMUX_FUNC_4, /* sub camera of D2 */
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},
	{
		.func = GPIOMUX_FUNC_4, /*active 11 : sub camera of ApexQ*/
		.drv = GPIOMUX_DRV_4MA,
		.pull = GPIOMUX_PULL_NONE,
	},
};

static struct msm_gpiomux_config msm8960_cam_common_configs[] = {
	{
		.gpio = GPIO_MSM_FLASH_NOW,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = GPIO_CAM_MCLK,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = GPIO_CAM2_RST_N,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = GPIO_CAM1_RST_N,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};

#ifdef CONFIG_MSM_CAMERA

static struct msm_gpiomux_config msm8960_cam_2d_configs[] = {
	{
		.gpio = GPIO_I2C_DATA_CAM,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = GPIO_I2C_CLK_CAM,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};

static struct msm_gpiomux_config msm8960_cam_2d_configs_v2[] = {
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[10],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = GPIO_I2C_DATA_CAM,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = GPIO_I2C_CLK_CAM,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};

static struct msm_camera_sensor_strobe_flash_data strobe_flash_xenon = {
	.flash_trigger = GPIO_MSM_FLASH_NOW,
	.flash_charge = GPIO_MSM_FLASH_CNTL_EN,
	.flash_charge_done = GPIO_MAIN_CAM_STBY,
	.flash_recharge_duration = 50000,
	.irq = MSM_GPIO_TO_INT(GPIO_MAIN_CAM_STBY),
};

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = GPIO_MSM_FLASH_CNTL_EN,
	._fsrc.ext_driver_src.led_flash_en = GPIO_MSM_FLASH_NOW,
};
#endif

static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 27648000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
};


static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 43200000,
		.ib  = 69120000,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 43200000,
		.ib  = 69120000,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 43200000,
		.ib  = 69120000,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 43200000,
		.ib  = 69120000,
	},
};

static struct msm_bus_vectors cam_video_ls_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 348192000,
		.ib  = 617103360,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_dual_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 43200000,
		.ib  = 69120000,
	},
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_MM_IMEM,
		.ab  = 43200000,
		.ib  = 69120000,
	},
};

static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_video_ls_vectors),
		cam_video_ls_vectors,
	},
	{
		ARRAY_SIZE(cam_dual_vectors),
		cam_dual_vectors,
	},
};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

static struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
		.csid_core = 0,
		.is_csiphy = 1,
		.is_csid   = 1,
		.is_ispif  = 1,
		.is_csic   = 0,
		.is_vpe    = 1,
        .is_csic = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
	{
		.csid_core = 1,
		.is_csiphy = 1,
		.is_csid   = 1,
		.is_ispif  = 1,
		.is_csic   = 0,
		.is_vpe    = 1,
        .is_csic = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
};

static struct camera_vreg_t msm_8960_back_cam_vreg[] = {
	{"cam_vdig", REG_LDO, 1200000, 1200000, 105000},
	{"cam_vio", REG_VS, 0, 0, 0},
	{"cam_vana", REG_LDO, 2800000, 2850000, 85600},
	{"cam_vio", REG_VS, 0, 0, 0},
	{"cam_vdig", REG_LDO, 1200000, 1200000, 105000},
	{"cam_vaf", REG_LDO, 2800000, 2800000, 300000},
};

static struct camera_vreg_t msm_8960_front_cam_vreg[] = {
	{"cam_vio", REG_VS, 0, 0, 0},
	{"cam_vana", REG_LDO, 2800000, 2850000, 85600},
	{"cam_vio", REG_VS, 0, 0, 0},
	{"cam_vdig", REG_LDO, 1200000, 1200000, 105000},
};

static struct gpio msm8960_common_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{20, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{21, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};

static struct gpio msm8960_common_cam_gpio_v2[] = {
	{2, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{20, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{21, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};

static struct gpio msm8960_front_cam_gpio[] = {
	/*{76, GPIOF_DIR_OUT, "CAM_RESET"},*/
};

static struct gpio msm8960_back_cam_gpio[] = {
	/*{107, GPIOF_DIR_OUT, "CAM_RESET"},*/
};

static struct msm_gpio_set_tbl msm8960_front_cam_gpio_set_tbl[] = {
	/*{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},*/
};

static struct msm_gpio_set_tbl msm8960_back_cam_gpio_set_tbl[] = {
	/*{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},*/
};

static struct msm_camera_gpio_conf msm_8960_front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8960_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8960_cam_2d_configs),
	.cam_gpio_common_tbl = msm8960_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8960_common_cam_gpio),
	.cam_gpio_req_tbl = msm8960_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8960_front_cam_gpio),
	.cam_gpio_set_tbl = msm8960_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8960_front_cam_gpio_set_tbl),
};

static struct msm_camera_gpio_conf msm_8960_back_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8960_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8960_cam_2d_configs),
	.cam_gpio_common_tbl = msm8960_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8960_common_cam_gpio),
	.cam_gpio_req_tbl = msm8960_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8960_back_cam_gpio),
	.cam_gpio_set_tbl = msm8960_back_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8960_back_cam_gpio_set_tbl),
};

static struct regulator *l8,*l11, *l12,*l16, *l18, *l29,*l30, *l28, *isp_core;
/* CAM power
	CAM_SENSOR_A_2.8		:  GPIO_CAM_A_EN(GPIO 46)
	CAM_SENSOR_IO_1.8		: VREG_L29		: l29
	CAM_AF_2.8				: VREG_L11		: l11
	CAM_SENSOR_CORE1.2		: VREG_L12		: l12
	CAM_ISP_CORE_1.2		: CAM_CORE_EN(GPIO 6)

	CAM_DVDD_1.5		: VREG_L18		: l18
*/

u8 torchonoff;
static void cam_ldo_power_on(int mode)
{
	int ret = 0;

	printk(KERN_DEBUG "[%s : %d] %s CAMERA POWER ON!!\n",
	       __func__, __LINE__, mode ? "FRONT" : "REAR");

/* FLASH_LED_UNLOCK*/
	if ((system_rev >= BOARD_REV03) && !mode && torchonoff == 0) {
		gpio_set_value_cansleep(PM8921_MPP_PM_TO_SYS
			(PMIC_MPP_FLASH_LED_UNLOCK), 1);
		ret = gpio_get_value_cansleep(PM8921_MPP_PM_TO_SYS
			(PMIC_MPP_FLASH_LED_UNLOCK));
		printk(KERN_DEBUG "check FLASH_LED_UNLOCK : %d\n", ret);
	}

/*5M Core 1.2V - CAM_ISP_CORE_1P2*/
	gpio_set_value_cansleep(GPIO_CAM_CORE_EN, 1);
	ret = gpio_get_value(GPIO_CAM_CORE_EN);
	printk(KERN_DEBUG "check CAM_CORE_EN : %d\n", ret);
	usleep(1000);

/*Sensor IO 1.8V -CAM_SENSOR_IO_1P8  */
#ifndef CONFIG_MACH_EXPRESS
	if (system_rev >= BOARD_REV02) {
		gpio_set_value_cansleep(GPIO_CAM_SENSOR_IO_EN, 1);
		ret = gpio_get_value(GPIO_CAM_SENSOR_IO_EN);
		printk(KERN_DEBUG "check CAM_SENSOR_IO_EN : %d\n", ret);
	} else {
#endif
		l29 = regulator_get(NULL, "8921_lvs5");
		ret = regulator_enable(l29);
		if (ret)
			cam_err("error enabling regulator\n");
#ifndef CONFIG_MACH_EXPRESS
	}
	usleep(1000);
#endif

/*Sensor AVDD 2.8V - CAM_SENSOR_A2P8 */
	gpio_set_value_cansleep(GPIO_CAM_A_EN, 1);
	ret = gpio_get_value(GPIO_CAM_A_EN);
	printk(KERN_DEBUG "check GPIO_CAM_A_EN : %d\n", ret);
	usleep(1000);

/*VT core 1.2V - CAM_DVDD_1P2V*/
	l18 = regulator_get(NULL, "8921_l18");
	ret = regulator_set_voltage(l18, 1500000, 1500000);
	if (ret)
		cam_err("error setting voltage\n");
	ret = regulator_enable(l18);
	if (ret)
		cam_err("error enabling regulator\n");


/*Sensor AF 2.8V -CAM_AF_2P8  */
	if (!mode) {
		l11 = regulator_get(NULL, "8921_l11");
		ret = regulator_set_voltage(l11, 2800000, 2800000);
		if (ret)
			cam_err("error setting voltage\n");
		ret = regulator_enable(l11);
		if (ret)
			cam_err("error enabling regulator\n");
	}
}

static void cam_ldo_power_off(int mode)
{
	int ret = 0;

	printk(KERN_DEBUG "[%s : %d] %s CAMERA POWER OFF!!\n",
	       __func__, __LINE__, mode ? "FRONT" : "REAR");

/* FLASH_LED_LOCK*/
	if ((system_rev >= BOARD_REV03) && !mode && torchonoff == 0) {
		gpio_set_value_cansleep(PM8921_MPP_PM_TO_SYS
			(PMIC_MPP_FLASH_LED_UNLOCK), 0);
	}

/*Sensor AF 2.8V -CAM_AF_2P8  */
	if (!mode) {
		if (l11) {
			ret = regulator_disable(l11);
			if (ret)
				cam_err("error disabling regulator\n");
		}
		usleep(1000);
	}

/*VT core 1.2 - CAM_DVDD_1P2V*/
	if (l18) {
		ret = regulator_disable(l18);
		if (ret)
			cam_err("error disabling regulator\n");
	}
	usleep(1000);

/*Sensor AVDD 2.8V - CAM_SENSOR_A2P8 */
	gpio_set_value_cansleep(GPIO_CAM_A_EN, 0);
	usleep(1000);

/*Sensor IO 1.8V -CAM_SENSOR_IO_1P8  */
	if (system_rev >= BOARD_REV02)
		gpio_set_value_cansleep(GPIO_CAM_SENSOR_IO_EN, 0);
	else {
		if (l29) {
			ret = regulator_disable(l29);
			if (ret)
				cam_err("error disabling regulator\n");
		}
	}
	usleep(1000);

/*5M Core 1.2V - CAM_ISP_CORE_1P2*/
	gpio_set_value_cansleep(GPIO_CAM_CORE_EN, 0);

}

static struct msm_camera_sensor_flash_data flash_isx012 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
};

static struct msm_camera_sensor_platform_info sensor_board_info_isx012 = {
    .mount_angle	= 90,
	.sensor_reset	= GPIO_CAM1_RST_N,
	.sensor_stby	= GPIO_MAIN_CAM_STBY,
	.vt_sensor_stby	= GPIO_VT_STBY,
	.vt_sensor_reset	= GPIO_CAM2_RST_N,
	.flash_en	= GPIO_MSM_FLASH_CNTL_EN,
	.flash_set	= GPIO_MSM_FLASH_NOW,
	.mclk	= GPIO_CAM_MCLK,
	.sensor_pwd	= GPIO_CAM_CORE_EN,
	.vcm_pwd	= 0,
	.vcm_enable	= 1,
	.sensor_power_on = cam_ldo_power_on,
	.sensor_power_off = cam_ldo_power_off,
	.cam_vreg = msm_8960_back_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8960_back_cam_vreg),
	.gpio_conf = &msm_8960_back_cam_gpio_conf,
};

static struct msm_camera_sensor_info msm_camera_sensor_isx012_data = {
	.sensor_name	= "isx012",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_isx012,
	.sensor_platform_info = &sensor_board_info_isx012,
    .gpio_conf = &msm_8960_back_cam_gpio_conf,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
};

static struct msm_camera_sensor_flash_data flash_db8131m = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_platform_info sensor_board_info_db8131m = {
	.mount_angle    = 270,
	.sensor_reset   = GPIO_CAM1_RST_N,
	.sensor_pwd     = GPIO_CAM_CORE_EN,
	.sensor_stby    = GPIO_MAIN_CAM_STBY,
	.vt_sensor_stby	= GPIO_VT_STBY,
	.vt_sensor_reset        = GPIO_CAM2_RST_N,
	.mclk   = GPIO_CAM_MCLK,
	.vcm_pwd        = 0,
	.vcm_enable     = 1,
	.sensor_power_on =  cam_ldo_power_on,
	.sensor_power_off = cam_ldo_power_off,
	.cam_vreg = msm_8960_front_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8960_front_cam_vreg),
	.gpio_conf = &msm_8960_front_cam_gpio_conf,
};

static struct msm_camera_sensor_info msm_camera_sensor_db8131m_data = {
	.sensor_name    = "db8131m",
	.pdata  = &msm_camera_csi_device_data[1],
	.flash_data     = &flash_db8131m,
	.sensor_platform_info = &sensor_board_info_db8131m,
	.gpio_conf = &msm_8960_front_cam_gpio_conf,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
};

static struct platform_device *cam_dev[] = {
};

static ssize_t back_camera_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_type[] = "SONY_ISX012\n";
	return snprintf(buf, sizeof(cam_type), "%s", cam_type);
}

static ssize_t front_camera_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_type[] = "DUB_DB8131M\n";
	return snprintf(buf, sizeof(cam_type), "%s", cam_type);
}

static DEVICE_ATTR(rear_camtype, S_IRUGO, back_camera_type_show, NULL);
static DEVICE_ATTR(front_camtype, S_IRUGO, front_camera_type_show, NULL);

static ssize_t back_camera_firmware_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_fw[] = "ISX012\n";
	return snprintf(buf, sizeof(cam_fw), "%s", cam_fw);
}

static ssize_t front_camera_firmware_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_fw[] = "DB8131M\n";
	return snprintf(buf, sizeof(cam_fw), "%s", cam_fw);
}

static DEVICE_ATTR(rear_camfw, 0664, back_camera_firmware_show, NULL);
static DEVICE_ATTR(front_camfw, 0664, front_camera_firmware_show, NULL);
u8 torchonoff;
static u8 gpio_flash_en;
static u8 gpio_flash_set;
static u8 pmic_gpio_msm_flash_cntl_en;
static bool isFlashCntlEn;

static int get_flash_led_unlock_rev(void)
{
	return ((system_rev >= BOARD_REV03) ? 1 : 0);
}

static ssize_t cameraflash_file_cmd_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t size)
{
	int value;
	int err = 1;
	int flash_rev = 0;

	flash_rev = get_flash_led_unlock_rev();

	if (strlen(buf) > 2)
		return -err;

	if (isdigit(*buf)) {
		err = kstrtoint(buf, 10, &value);
		if (err < 0)
			pr_err("%s, kstrtoint failed.", __func__);
	} else
		return -err;

	if (flash_rev) {
		gpio_set_value_cansleep(PM8921_MPP_PM_TO_SYS
			(PMIC_MPP_FLASH_LED_UNLOCK), value ? 1 : 0);
	}

	if (value == 0) {
		pr_err("[Torch flash]OFF\n");
		gpio_set_value_cansleep(gpio_flash_en, 0);
#ifndef CONFIG_MACH_EXPRESS
		gpio_set_value_cansleep(gpio_flash_set, 0);
#endif
		torchonoff = 0;
	}
	else {
		pr_err("[Torch flash]ON\n");
		gpio_set_value_cansleep(gpio_flash_en, 0);
#ifdef CONFIG_MACH_EXPRESS
		udelay(0);
		gpio_set_value_cansleep(gpio_flash_en, 1);
		udelay(1);
#else
		int i = 0;
		for (i = 5; i > 1; i--) {
			gpio_set_value_cansleep(
				gpio_flash_set, 1);
			udelay(1);
			gpio_set_value_cansleep(
				gpio_flash_set, 0);
			udelay(1);
		}
		gpio_set_value_cansleep(gpio_flash_set, 1);
		usleep(2*1000);
#endif
		torchonoff = 1;
	}
	if (value > 1) {
		pr_err("[Torch flash]HIGH\n");
#ifdef CONFIG_MACH_EXPRESS
		/* expressatt has multiple steps for brightness, so
		   we incrementally turn it up */
		int i = 0;
		for (i = 15; i > 1; i--) {
			gpio_set_value_cansleep(
				gpio_flash_en, 0);
			udelay(0);
			gpio_set_value_cansleep(
				gpio_flash_en, 1);
			udelay(1);
		}
#else
		gpio_set_value_cansleep(gpio_flash_en, 1);
		gpio_set_value_cansleep(gpio_flash_set, 0);
#endif
	}
	return size;
}

static DEVICE_ATTR(rear_flash, S_IRUGO | S_IWUSR | S_IWGRP,
		NULL, cameraflash_file_cmd_store);

void msm8960_cam_create_node(void)
{
	struct device *cam_dev_back;
	struct device *cam_dev_front;
	struct class *camera_class;

	camera_class = class_create(THIS_MODULE, "camera");

	if (IS_ERR(camera_class)) {
		pr_err("Failed to create class(camera)!\n");
		return;
	}
	cam_dev_back = device_create(camera_class, NULL,
		0, NULL, "rear");

	if (IS_ERR(cam_dev_back)) {
		pr_err("Failed to create cam_dev_back device!\n");
		goto OUT7;
	}

	if (device_create_file(cam_dev_back, &dev_attr_rear_flash) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_flash.attr.name);
		goto OUT6;
	}

	if (device_create_file(cam_dev_back, &dev_attr_rear_camtype) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_camtype.attr.name);
		goto OUT5;
	}

	if (device_create_file(cam_dev_back, &dev_attr_rear_camfw) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_camfw.attr.name);
		goto OUT4;
	}

	cam_dev_front = device_create(camera_class, NULL,
		1, NULL, "front");

	if (IS_ERR(cam_dev_front)) {
		pr_err("Failed to create cam_dev_front device!");
		goto OUT3;
	}

	if (device_create_file(cam_dev_front, &dev_attr_front_camtype) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_camtype.attr.name);
		goto OUT2;
	}

	if (device_create_file(cam_dev_front, &dev_attr_front_camfw) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_camfw.attr.name);
		goto OUT1;
	}

	return;

OUT1:
	device_remove_file(cam_dev_back, &dev_attr_front_camtype);
OUT2:
	device_destroy(camera_class, 1);
OUT3:
	device_remove_file(cam_dev_back, &dev_attr_rear_camfw);
OUT4:
	device_remove_file(cam_dev_back, &dev_attr_rear_camtype);
OUT5:
	device_remove_file(cam_dev_back, &dev_attr_rear_flash);
OUT6:
	device_destroy(camera_class, 0);
OUT7:
	return;
}
static int get_mclk_rev(void)
{
	return ((system_rev >= BOARD_REV04) ? 1 : 0);
}

void __init msm8960_init_cam(void)
{
	int rev = 0;
	struct msm_camera_sensor_info *s_info;

	rev = get_mclk_rev();

	if (rev) {
		int rc;

		struct pm_gpio param_flash = {
			.direction      = PM_GPIO_DIR_OUT,
			.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
			.output_value   = 0,
			.pull	   = PM_GPIO_PULL_NO,
			.vin_sel	= PM_GPIO_VIN_S4,
			.out_strength   = PM_GPIO_STRENGTH_MED,
			.function       = PM_GPIO_FUNC_NORMAL,
		};

		rc = pm8xxx_gpio_config(PM8921_GPIO_PM_TO_SYS
			(PMIC_MSM_FLASH_CNTL_EN), &param_flash);

		if (rc) {
			pr_err("%s pmic gpio config failed\n", __func__);
			return;
		}
		pmic_gpio_msm_flash_cntl_en =
			PM8921_GPIO_PM_TO_SYS(PMIC_MSM_FLASH_CNTL_EN);
	} else {
		pmic_gpio_msm_flash_cntl_en = 0;
	}
	isFlashCntlEn = false;

	msm8960_cam_create_node();

	msm_gpiomux_install(msm8960_cam_common_configs,
			ARRAY_SIZE(msm8960_cam_common_configs));

	gpio_tlmm_config(GPIO_CFG(GPIO_CAM_CORE_EN, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(GPIO_CAM_A_EN, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
#ifndef CONFIG_MACH_EXPRESS
	if (system_rev >= BOARD_REV02) {
		gpio_tlmm_config(GPIO_CFG(GPIO_CAM_SENSOR_IO_EN, 0,
			GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
			GPIO_CFG_ENABLE);
	}
#endif
	/*Main cam reset */
	gpio_tlmm_config(GPIO_CFG(GPIO_CAM1_RST_N, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	/*Main cam stby*/
	gpio_tlmm_config(GPIO_CFG(GPIO_MAIN_CAM_STBY, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	/*Front cam reset*/
	gpio_tlmm_config(GPIO_CFG(GPIO_CAM2_RST_N, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	/*Front cam stby*/
	gpio_tlmm_config(GPIO_CFG(GPIO_VT_STBY, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	/*Falsh Enable*/
	if (!pmic_gpio_msm_flash_cntl_en) {
		gpio_tlmm_config(GPIO_CFG(GPIO_MSM_FLASH_CNTL_EN, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
		GPIO_CFG_ENABLE);
	}
	/*Flash Set*/
	gpio_tlmm_config(GPIO_CFG(GPIO_MSM_FLASH_NOW, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);

	/*CAM_MCLK0*/
	gpio_tlmm_config(GPIO_CFG(GPIO_CAM_MCLK, 1, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	s_info = &msm_camera_sensor_isx012_data;
	if (rev) {
#if defined(CONFIG_MACH_EXPRESS)
		if (system_rev >= BOARD_REV07) {
			s_info->sensor_platform_info->flash_en =
				GPIO_MSM_FLASH_NOW;
			s_info->sensor_platform_info->flash_set =
				-1;
		}
#else
		s_info->sensor_platform_info->flash_en =
			pmic_gpio_msm_flash_cntl_en;
#endif
	}
	gpio_flash_en = s_info->sensor_platform_info->flash_en;
	gpio_flash_set = s_info->sensor_platform_info->flash_set;

	s_info = &msm_camera_sensor_db8131m_data;

	if (rev) {
		s_info->sensor_platform_info->mclk =
			GPIO_CAM_MCLK2;
		s_info->sensor_platform_info->gpio_conf->cam_gpiomux_conf_tbl =
			msm8960_cam_2d_configs_v2;
		s_info->sensor_platform_info->gpio_conf->
			cam_gpiomux_conf_tbl_size =
			ARRAY_SIZE(msm8960_cam_2d_configs_v2);
		s_info->sensor_platform_info->gpio_conf->cam_gpio_common_tbl =
			msm8960_common_cam_gpio_v2;
		gpio_tlmm_config(GPIO_CFG(GPIO_CAM_MCLK2, 1, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	}

	pr_err("[%s:%d]setting done!!\n", __func__, __LINE__);

	platform_device_register(&msm8960_device_csiphy0);
	platform_device_register(&msm8960_device_csiphy1);
	platform_device_register(&msm8960_device_csid0);
	platform_device_register(&msm8960_device_csid1);
	platform_device_register(&msm8960_device_ispif);
	platform_device_register(&msm8960_device_vfe);
	platform_device_register(&msm8960_device_vpe);
}

static struct i2c_board_info msm8960_camera_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO("db8131m", 0x45),
		.platform_data = &msm_camera_sensor_db8131m_data,
	},
	{
		I2C_BOARD_INFO("isx012", 0x3D),
		.platform_data = &msm_camera_sensor_isx012_data,
	},
};

struct msm_camera_board_info msm8960_camera_board_info = {
	.board_info = msm8960_camera_i2c_boardinfo,
	.num_i2c_board_info = ARRAY_SIZE(msm8960_camera_i2c_boardinfo),
};

struct resource msm_camera_resources[] = {
	{
		.name   = "s3d_rw",
		.start  = 0x008003E0,
		.end    = 0x008003E0 + SZ_16 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "s3d_ctl",
		.start  = 0x008020B8,
		.end    = 0x008020B8 + SZ_16 - 1,
		.flags  = IORESOURCE_MEM,
	},
};

int __init msm_get_cam_resources(struct msm_camera_sensor_info *s_info)
{
	s_info->resource = msm_camera_resources;
	s_info->num_resources = ARRAY_SIZE(msm_camera_resources);
	return 0;
}

#endif

