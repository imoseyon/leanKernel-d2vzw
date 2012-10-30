

/***************************************************************
CAMERA Power control
****************************************************************/




#include <mach/gpio.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>

#include <linux/regulator/consumer.h>
#include "cam_pmic.h"



struct regulator *l11_s5k6aa,*l12_s5k6aa,*l16_s5k6aa,*l18_s5k6aa,*l29_s5k6aa;


/* CAM power
	CAM_SENSOR_A_2.8		:  VREG_L16		: l16
	CAM_SENSOR_IO_1.8		: VREG_L29		: l29
	CAM_AF_2.8				: VREG_L11		: l11
	CAM_SENSOR_CORE1.2		: VREG_L12		: l12
	CAM_ISP_CORE_1.2		: CAM_CORE_EN(GPIO 6)

	CAM_DVDD_1.5		: VREG_L18		: l18
*/
void cam_ldo_power_on_s5k6aa(void)
{
	int ret = 0;
	int temp = 0;
	printk("[s5k6aa]cam_ldo_power_on\n");

//CAM_IO_1.8V
	l29_s5k6aa = regulator_get(NULL, "8921_l29");
	ret = regulator_set_voltage(l29_s5k6aa, 1800000, 1800000);
	if (ret) {
	printk("%s: error setting voltage\n", __func__);
	}
	ret = regulator_enable(l29_s5k6aa);
	if (ret) {
	printk("%s: error enabling regulator\n", __func__);
	}
	mdelay(5);

// CAM_A_2.8V
	l16_s5k6aa = regulator_get(NULL, "8921_l16");
	ret = regulator_set_voltage(l16_s5k6aa, 2800000, 2800000);
	if (ret) {
	printk("%s: error setting voltage\n", __func__);
	}
	ret = regulator_enable(l16_s5k6aa);
	if (ret) {
	printk("%s: error enabling regulator\n", __func__);
	}
	mdelay(5);

//CAM_DVDD_1.5V(sub)
	l18_s5k6aa = regulator_get(NULL, "8921_l18");
	ret = regulator_set_voltage(l18_s5k6aa, 1500000, 1500000);
	if (ret) {
	printk("%s: error setting voltage\n", __func__);
	}
	ret = regulator_enable(l18_s5k6aa);
	if (ret) {
	printk("%s: error enabling regulator\n", __func__);
	}
	mdelay(5);
}

void cam_ldo_power_off_s5k6aa(void)
{
	int ret = 0;
	printk("#### cam_ldo_power_off ####\n");

	// 3M_AF 2.8V
	if (l11_s5k6aa) {
		ret = regulator_disable(l11_s5k6aa);
		if (ret) {
		printk("%s: error disabling regulator\n", __func__);
		}
		//regulator_put(lvs0);
	}
	mdelay(5);

	//CAM_IO_1.8V
	if (l29_s5k6aa) {
		ret = regulator_disable(l29_s5k6aa);
		if (ret) {
		printk("%s: error disabling regulator\n", __func__);
		}
		//regulator_put(lvs0);
	}
	mdelay(5);

	//VT_CORE_1.5V(sub)
	if (l18_s5k6aa) {
		ret = regulator_disable(l18_s5k6aa);
		if (ret) {
		printk("%s: error disabling regulator\n", __func__);
		}
		//regulator_put(lvs0);
	}
	mdelay(5);

	//SENSOR_CORE_1.2V
	if (l12_s5k6aa) {
		ret = regulator_disable(l12_s5k6aa);
		if (ret) {
		printk("%s: error disabling regulator\n", __func__);
		}
		//regulator_put(lvs0);
	}
	mdelay(5);

	// CAM_A_2.8V
	if (l16_s5k6aa) {
		ret = regulator_disable(l16_s5k6aa);
		if (ret) {
		printk("%s: error disabling regulator\n", __func__);
		}
		//regulator_put(lvs0);
	}
	mdelay(5);

	//CAM_ISP_CORE_1.2
	gpio_set_value_cansleep(CAM_CORE_EN, LOW);
	mdelay(5);

}
