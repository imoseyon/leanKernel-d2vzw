GCC_VERSION      := $(shell $(CONFIG_SHELL) $(PWD)/scripts/gcc-version.sh $(CROSS_COMPILE)gcc)
EXTRA_CFLAGS += -Idrivers/media/video/msm_zsl
EXTRA_CFLAGS += -Idrivers/media/video/msm_zsl/io
obj-$(CONFIG_MSM_ACTUATOR) += msm_actuator.o
obj-$(CONFIG_IMX074_ACT) += imx074_act.o
