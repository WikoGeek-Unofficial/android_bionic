include $(srctree)/drivers/misc/mediatek/Makefile.custom

ifneq ($(filter "mt6582_3sim_phone",$(CONFIG_ARCH_MTK_PROJECT)),)

obj-y += kd_dummy.o

else

obj-$(CONFIG_MTK_SENSOR_SUPPORT) += $(subst ",,$(CONFIG_MTK_PLATFORM))/

endif

