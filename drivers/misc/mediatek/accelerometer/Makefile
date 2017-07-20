include $(srctree)/drivers/misc/mediatek/Makefile.custom

# In case the platform does NOT support this type of sensors
obj-y   +=  accelerometer_dummy.o
obj-y   +=  accel.o

ifeq ($(CONFIG_MTK_KXTJ2_1009),y)
obj-y	+=  KXTJ2_1009/
endif

ifeq ($(CONFIG_MTK_K2DH),y)
obj-y	+=  k2dh/
endif

ifeq ($(CONFIG_MTK_BMA050),y)
obj-y   +=  bma050/
endif

ifeq ($(CONFIG_MTK_BMA250),y)
obj-y   +=  bma250/
endif

ifeq ($(CONFIG_MTK_BMA050_NEW),y)
obj-y   +=  bma050-new/
endif

ifeq ($(CONFIG_MTK_KXTIK1004),y)
obj-y   +=  kxtik1004/
endif

ifeq ($(CONFIG_MTK_BMA222E),y)
obj-y   +=  bma222E/
endif

ifeq ($(CONFIG_MTK_BMA056),y)
obj-y   +=  bma056/
endif

