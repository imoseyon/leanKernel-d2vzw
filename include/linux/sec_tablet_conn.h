/*
 * Copyright (C) 2008 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __ASM_ARCH_ACC_CONN_H
#define __ASM_ARCH_ACC_CONN_H

#ifdef CONFIG_SEC_KEYBOARD_DOCK
struct sec_keyboard_callbacks {
	int (*check_keyboard_dock)(struct sec_keyboard_callbacks *cb, \
			bool attached);
};

struct sec_keyboard_platform_data {
	int accessory_irq_gpio;
	int (*wakeup_key)(void);
	void (*check_uart_path)(bool en);
	void (*acc_power)(u8 token, bool active);
	int (*noti_univ_kbd_dock)(unsigned int code);
	void (*register_cb)(struct sec_keyboard_callbacks *cb);
};
#endif

struct sec_30pin_callbacks {
	int (*noti_univ_kdb_dock)(struct sec_30pin_callbacks *cb,
		unsigned int code);
};

struct acc_con_platform_data {
/*	void	(*otg_en) (int active);
	void	(*usb_en) (int active); */
	int	(*otg_en) (bool active);
	int	(*usb_en) (bool active);
#ifdef CONFIG_CAMERON_HEALTH
	void (*cameron_health_en) (bool active);
#endif
	void	(*acc_power) (u8 token, bool active);
	void    (*usb_ldo_en) (int active);
	int (*get_dock_state)(void);
	int (*check_keyboard)(bool attached);
	void (*register_cb)(struct sec_30pin_callbacks *cb);
	int     accessory_irq_gpio;
	int     dock_irq_gpio;
	int     mhl_irq_gpio;
	int     hdmi_hpd_gpio;
};

extern int64_t acc_get_adc_value(void);

extern struct class *sec_class;

#endif
