/*
 * include/linux/sec_param.h
 *
 * Copyright (c) 2011 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

typedef struct _sec_param_data {
	unsigned int debuglevel;
	unsigned int uartsel;
	unsigned int rory_control;
	unsigned int movinand_checksum_done;
	unsigned int movinand_checksum_pass;
#if defined(CONFIG_MACH_APEXQ) || defined(CONFIG_MACH_AEGIS2)
	unsigned int slideCount;
#endif
#ifdef CONFIG_SEC_SSR_DEBUG_LEVEL_CHK
	unsigned int cp_debuglevel;
#endif
} sec_param_data;

typedef enum {
	param_index_debuglevel,
	param_index_uartsel,
	param_rory_control,
	param_index_movinand_checksum_done,
	param_index_movinand_checksum_pass,
#if defined(CONFIG_MACH_APEXQ) || defined(CONFIG_MACH_AEGIS2)
	param_slideCount,
#endif
#ifdef CONFIG_SEC_SSR_DEBUG_LEVEL_CHK
	param_cp_debuglevel,
#endif
} sec_param_index;

extern bool sec_open_param(void);
extern bool sec_get_param(sec_param_index index, void *value);
extern bool sec_set_param(sec_param_index index, void *value);
