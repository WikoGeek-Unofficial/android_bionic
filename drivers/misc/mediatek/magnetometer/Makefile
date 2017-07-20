include $(srctree)/drivers/misc/mediatek/Makefile.custom

# In case the platform does NOT support this type of sensors
obj-y   +=  magnetmeter_dummy.o
obj-y   +=  mag.o

ifeq ($(CONFIG_MTK_HSCDTD006),y)
obj-y	+=  hscdtd006/
endif

ifeq ($(CONFIG_MTK_AKM8963),y)
obj-y	+=  akm8963/
endif

ifeq ($(CONFIG_MTK_BMM050),y)
obj-y   +=  bmm050/
endif

ifeq ($(CONFIG_MTK_BMM050_NEW),y)
obj-y   +=  bmm050-new/
endif

ifeq ($(CONFIG_MTK_BMM056_NEW),y)
obj-y   +=  bmm056-new/
endif

ifeq ($(CONFIG_MTK_MMC3416X),y)
obj-y   +=  mmc3416x/
endif

ifeq ($(CONFIG_MTK_AKM09911),y)
obj-y   +=  akm09911/
endif

