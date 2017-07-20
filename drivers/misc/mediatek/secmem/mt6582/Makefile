ifeq ($(CONFIG_TRUSTONIC_TEE_SUPPORT),y)

include $(srctree)/drivers/misc/mediatek/Makefile.custom
ccflags-y += -I$(srctree)/arch/arm/mach-$(MTK_PLATFORM)/include/trustzone/utils/tlutils
ccflags-y += -I$(srctree)/drivers/misc/mediatek/gud/$(MTK_PLATFORM)/MobiCoreKernelApi/include \
             -I$(srctree)/drivers/misc/mediatek/gud/$(MTK_PLATFORM)/MobiCoreKernelApi/public
obj-y += secmem.o

endif
