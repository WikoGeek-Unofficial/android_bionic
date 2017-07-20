include $(srctree)/drivers/misc/mediatek/Makefile.custom

# In case the platform does NOT support this type of sensors
obj-y   +=  alsps_dummy.o
obj-y   +=  alsps.o
obj-y   +=  aal_control.o

ifeq ($(CONFIG_MTK_GP2AP002S00F),y)
obj-y	+=  gp2ap002s00f/
endif

ifeq ($(CONFIG_MTK_CM36283),y)
obj-y	+=  cm36283/
endif

ifeq ($(CONFIG_MTK_APM_16D),y)
obj-y	+=  apm_16d/
endif

ifeq ($(CONFIG_MTK_EPL2182),y)
obj-y	+=  epl2182/
endif

ifeq ($(CONFIG_MTK_STK3X1X),y)
obj-y	+=  stk3x1x/
endif

ifeq ($(CONFIG_MTK_LTR553),y)
obj-y	+=  ltr553/
endif
