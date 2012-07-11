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

#if defined(CONFIG_MACH_GOGH)
#include "a2220_firmware_gogh.h"
#elif defined(CONFIG_MACH_COMANCHE)
#include "a2220_firmware_comanche.h"
#elif defined(_d2tmo_)
#include "a2220_firmware_t999.h"
#else
#include "a2220_firmware_i747.h"
#endif

