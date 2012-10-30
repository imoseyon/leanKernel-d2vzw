
#ifndef _SEC_CAM_PMIC_H
#define _SEC_CAM_PMIC_H


#if 0
#define	CAM_8M_RST		50
#define	CAM_VGA_RST		41

#define	CAM_MEGA_EN		37

#define	CAM_VGA_EN		42

#define	CAM_PMIC_STBY		37

#define	CAM_IO_EN		37
#endif


#define	ISP_INT			4
#define	CAM_CORE_EN	6
//#define	5M_RST			107
#define	VT_RST			76
#define	VT_EN			18

#define	ON		1
#define	OFF		0
#define LOW		0
#define HIGH		1

void cam_ldo_power_on_s5k6aa(void);
void cam_ldo_power_off_s5k6aa(void);


#endif
