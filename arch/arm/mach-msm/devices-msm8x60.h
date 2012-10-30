/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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
#ifndef __ARCH_ARM_MACH_MSM_DEVICES_MSM8X60_H
#define __ARCH_ARM_MACH_MSM_DEVICES_MSM8X60_H

#define MSM_GSBI3_QUP_I2C_BUS_ID 0
#define MSM_GSBI4_QUP_I2C_BUS_ID 1
#define MSM_GSBI9_QUP_I2C_BUS_ID 2
#define MSM_GSBI8_QUP_I2C_BUS_ID 3
#define MSM_GSBI7_QUP_I2C_BUS_ID 4
#define MSM_GSBI12_QUP_I2C_BUS_ID 5
#define MSM_SSBI1_I2C_BUS_ID     6
#define MSM_SSBI2_I2C_BUS_ID     7
#define MSM_SSBI3_I2C_BUS_ID     8

/* add more I2C BUS */
#define MSM_8960_GSBI1_QUP_I2C_BUS_ID 1
#define MSM_8960_GSBI4_QUP_I2C_BUS_ID 4
#define MSM_8960_GSBI7_QUP_I2C_BUS_ID 7

#define MSM_MHL_I2C_BUS_ID             9
#define MSM_FSA9485_I2C_BUS_ID	11
#define MSM_SNS_I2C_BUS_ID     12
#define MSM_FUELGAUGE_I2C_BUS_ID             13
#define MSM_OPT_I2C_BUS_ID     14
#define MSM_TOUCHKEY_I2C_BUS_ID		16
#define MSM_NFC_I2C_BUS_ID		17
#define MSM_A2220_I2C_BUS_ID            18
#ifdef CONFIG_SAMSUNG_CMC624
#define MSM_CMC624_I2C_BUS_ID   19
#endif

#if defined(CONFIG_KEYBOARD_ADP5588) || defined(CONFIG_KEYBOARD_ADP5588_MODULE)
#define MSM_ADP5588_KEYS_BUS_ID	20
#endif
#ifdef CONFIG_IRDA_MC96
#define MSM_MC96_I2C_BUS_ID   21
#endif
#ifdef CONFIG_ADC_STMPE811
#define MSM_STMPE811_I2C_BUS_ID 22
#endif
#ifdef CONFIG_SND_SOC_MSM8660_APQ
extern struct platform_device msm_pcm;
extern struct platform_device msm_pcm_routing;
extern struct platform_device msm_cpudai0;
extern struct platform_device msm_cpudai1;
extern struct platform_device msm_cpudai_hdmi_rx;
extern struct platform_device msm_cpudai_bt_rx;
extern struct platform_device msm_cpudai_bt_tx;
extern struct platform_device msm_cpudai_fm_rx;
extern struct platform_device msm_cpudai_fm_tx;
extern struct platform_device msm_cpu_fe;
extern struct platform_device msm_stub_codec;
extern struct platform_device msm_voice;
extern struct platform_device msm_voip;
extern struct platform_device msm_lpa_pcm;
extern struct platform_device msm_pcm_hostless;
#endif

#ifdef CONFIG_SPI_QUP
extern struct platform_device msm_gsbi1_qup_spi_device;
extern struct platform_device msm_gsbi10_qup_spi_device;
#endif

extern struct platform_device msm_bus_apps_fabric;
extern struct platform_device msm_bus_sys_fabric;
extern struct platform_device msm_bus_mm_fabric;
extern struct platform_device msm_bus_sys_fpb;
extern struct platform_device msm_bus_cpss_fpb;
extern struct platform_device msm_bus_def_fab;

extern struct platform_device msm_device_smd;
extern struct platform_device msm_device_gpio;
extern struct platform_device msm_device_vidc;

extern struct platform_device msm_charm_modem;
extern struct platform_device msm_device_tz_log;
#ifdef CONFIG_HW_RANDOM_MSM
extern struct platform_device msm_device_rng;
#endif

void __init msm8x60_init_irq(void);
void __init msm8x60_check_2d_hardware(void);

#ifdef CONFIG_MSM_DSPS
extern struct platform_device msm_dsps_device;
#endif

#if defined(CONFIG_MSM_RPM_STATS_LOG)
extern struct platform_device msm_rpm_stat_device;
#endif
#endif
