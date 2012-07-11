
# Kernel makefile for motions sensors
#
# 

# MPU
obj-$(CONFIG_INV_SENSORS)+=
ifdef CONFIG_MPU_SENSORS_MPU3050_NEW
INV_MODULE_NAME := mpu3050
endif

ifdef CONFIG_MPU_SENSORS_MPU6050A2
INV_MODULE_NAME := mpu6050
else
ifdef CONFIG_MPU_SENSORS_MPU6050B1
INV_MODULE_NAME := mpu6050
endif  
endif

obj-$(CONFIG_INV_SENSORS)	+= $(INV_MODULE_NAME).o

$(INV_MODULE_NAME)-objs += mpuirq.o \
	slaveirq.o \
	mpu-dev.o \
	mpu-i2c.o \
	mlsl-kernel.o \
	mlos-kernel.o \
	$(MLLITE_DIR)mldl_cfg.o

#
# Accel options
#
ifdef CONFIG_MPU_SENSORS_ADXL34X_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/adxl34x.o
endif

ifdef CONFIG_MPU_SENSORS_BMA150_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/bma150.o
endif

ifdef CONFIG_MPU_SENSORS_BMA222_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/bma222.o
endif

ifdef CONFIG_MPU_SENSORS_BMA250_NEW
mpu3050-objs += $(MLLITE_DIR)accel/bma250.o
endif

ifdef CONFIG_MPU_SENSORS_KXSD9_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/kxsd9.o
endif

ifdef CONFIG_MPU_SENSORS_KXTF9_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/kxtf9.o
endif

ifdef CONFIG_MPU_SENSORS_LIS331DLH_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/lis331.o
endif

ifdef CONFIG_MPU_SENSORS_LIS3DH_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/lis3dh.o
endif

ifdef CONFIG_MPU_SENSORS_LSM303DLHA_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/lsm303a.o
endif

ifdef CONFIG_MPU_SENSORS_MMA8450_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/mma8450.o
endif

ifdef CONFIG_MPU_SENSORS_MMA845X_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/mma845x.o
endif

ifeq ($(INV_MODULE_NAME), mpu6050)
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)accel/mantis.o
endif

#
# Compass options
#
ifdef CONFIG_MPU_SENSORS_AK8975_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/ak8975.o
endif

ifdef CONFIG_MPU_SENSORS_AMI30X_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/ami30x.o
endif

ifdef CONFIG_MPU_SENSORS_AMI306_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/ami306.o
endif

ifdef CONFIG_MPU_SENSORS_HMC5883_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/hmc5883.o
endif

ifdef CONFIG_MPU_SENSORS_LSM303DLHM_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/lsm303m.o
endif

ifdef CONFIG_MPU_SENSORS_MMC314X_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/mmc314x.o
endif

ifdef CONFIG_MPU_SENSORS_YAS529_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/yas529-kernel.o
endif

ifdef CONFIG_MPU_SENSORS_YAS530_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/yas530.o
endif

ifdef CONFIG_MPU_SENSORS_HSCDTD002B_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/hscdtd002b.o
endif

ifdef CONFIG_MPU_SENSORS_HSCDTD004A_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)compass/hscdtd004a.o
endif
#
# Pressure options
#
ifdef CONFIG_MPU_SENSORS_BMA085_NEW
$(INV_MODULE_NAME)-objs += $(MLLITE_DIR)pressure/bma085.o
endif

EXTRA_CFLAGS += -I$(M)/$(MLLITE_DIR) \
                -I$(M)/../../include \
                -Idrivers/misc/mpu3050 \
                -Iinclude/linux

obj-$(CONFIG_MPU_SENSORS_TIMERIRQ_NEW)+= timerirq.o

