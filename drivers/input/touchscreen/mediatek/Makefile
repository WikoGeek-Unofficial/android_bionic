include $(srctree)/drivers/misc/mediatek/Makefile.custom


# In case the platform does NOT support this type of sensors
obj-y   +=  mtk_tpd.o
obj-y   +=  tpd_button.o
obj-y   +=  tpd_calibrate.o
obj-y   +=  tpd_debug.o
obj-y   +=  tpd_default.o
obj-y   +=  tpd_init.o
obj-y   +=  tpd_misc.o
obj-y   +=  tpd_setting.o

ifeq ($(CONFIG_MTK_GT9XX),y)
obj-y	+=  GT9XX/
endif

ifeq ($(CONFIG_MTK_S2200),y)
obj-y   +=  synaptics_2200/
endif

ifeq ($(CONFIG_MTK_MMS134),y)
obj-y	+=  mms134/
endif

ifeq ($(CONFIG_TOUCHSCREEN_SYNAPTICS_S2202), y)
obj-y   +=  synaptics_2202/
endif

ifeq ($(CONFIG_TOUCHSCREEN_SYNAPTICS_S7020), y)
obj-y   +=  synaptics_7020/
endif

ifeq ($(CONFIG_MTK_FT5406),y)
obj-y	+=  ft5406/
endif

ifeq ($(CONFIG_MTK_MMS128),y)
obj-y	+=  mms128/
endif

ifeq ($(CONFIG_MTK_FT5406_82),y)
obj-y	+=  ft5406_82/
endif

ifeq ($(CONFIG_MTK_FT5206),y)
obj-y	+=  ft5206/
endif

ifeq ($(CONFIG_MTK_S7020),y)
obj-y	+=  S7020/
endif

ifeq ($(CONFIG_MTK_FT6336),y)
obj-y	+=  ft6336/
endif

