/*
 *
 * Copyright(C) 2012 Samsung Electronics All rights reserved.
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
#if defined(_goghspr_)
#include "a2220_firmware_gogh_spr.h"
#elif defined(CONFIG_MACH_GOGH)
#include "a2220_firmware_gogh.h"
#elif defined(CONFIG_MACH_INFINITE)
#include "a2220_firmware_infinite.h"
#elif defined(CONFIG_MACH_COMANCHE)
#include "a2220_firmware_comanche.h"
#elif defined(CONFIG_MACH_EXPRESS)
#include "a2220_firmware_express.h"
#elif defined(CONFIG_MACH_AEGIS2)
#include "a2220_firmware_aegis2.h"
#elif defined(CONFIG_MACH_JASPER)
#include "a2220_firmware_jasper.h"
#elif defined(_d2tmo_) || defined(_d2ltetmo_) || defined(_d2lteMetroPCS_)
#include "a2220_firmware_t999.h"
#elif defined(_d2spr_) || defined(_d2vmu_)
#include "a2220_firmware_l710.h"
#else
#include "a2220_firmware_i747.h"
#endif

