#
# Copyright (C) 2009-2011 The Android-x86 Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
ifneq ($(strip $(MTK_PROJECT_NAME)),)

ifneq ($(wildcard $(call my-dir)/arch/arm/configs/$(KERNEL_DEFCONFIG)),)
KERNEL_DIR := $(call my-dir)
ROOTDIR := $(abspath $(TOP))

ifneq ($(filter /% ~%,$(OUT_DIR)),)
KERNEL_OUT := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
KERNEL_MODULES_OUT := $(TARGET_OUT)/lib/modules
KERNEL_MODULES_SYMBOLS_OUT := $(PRODUCT_OUT)/symbols/system/lib/modules
else
KERNEL_OUT := $(ROOTDIR)/$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
KERNEL_MODULES_OUT := $(ROOTDIR)/$(TARGET_OUT)/lib/modules
KERNEL_MODULES_SYMBOLS_OUT := $(ROOTDIR)/$(PRODUCT_OUT)/symbols/system/lib/modules
endif
TARGET_PREBUILT_KERNEL := $(KERNEL_OUT)/arch/arm/boot/zImage
ifneq ($(strip $(MTK_BASIC_PACKAGE)),yes)
  TARGET_PREBUILT_KERNEL_BIN := $(KERNEL_OUT)/arch/arm/boot/zImage.bin
endif
TARGET_KERNEL_CONFIG := $(KERNEL_OUT)/.config
KERNEL_HEADERS_INSTALL := $(KERNEL_OUT)/usr

ifeq ($(KERNEL_CROSS_COMPILE),)
KERNEL_CROSS_COMPILE := arm-eabi-
endif

define mv-modules
mdpath=`find $(1) -type f -name modules.dep`;\
if [ "$$mdpath" != "" ];then\
mpath=`dirname $$mdpath`;\
ko=`find $$mpath/kernel -type f -name *.ko`;\
for i in $$ko; do mv $$i $(1)/; done;\
fi
endef

define clean-module-folder
rm -rf $(1)/lib
endef

$(KERNEL_OUT):
	mkdir -p $@

$(KERNEL_MODULES_OUT):
	mkdir -p $@

.PHONY: kernel kernel-defconfig kernel-menuconfig kernel-modules clean-kernel
kernel-menuconfig: | $(KERNEL_OUT)
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) menuconfig
	
kernel-savedefconfig: | $(KERNEL_OUT)
	cp $(TARGET_KERNEL_CONFIG) $(KERNEL_DIR)/arch/arm/configs/$(KERNEL_DEFCONFIG)

$(TARGET_PREBUILT_KERNEL): kernel

$(TARGET_KERNEL_CONFIG) kernel-defconfig: $(KERNEL_DIR)/arch/arm/configs/$(KERNEL_DEFCONFIG) | $(KERNEL_OUT)
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) $(KERNEL_DEFCONFIG)
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) oldconfig
	
$(KERNEL_HEADERS_INSTALL): $(TARGET_KERNEL_CONFIG) | $(KERNEL_OUT)
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) headers_install

kernel: $(TARGET_KERNEL_CONFIG) $(KERNEL_HEADERS_INSTALL) | $(KERNEL_OUT)
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) modules
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) INSTALL_MOD_PATH=$(KERNEL_MODULES_SYMBOLS_OUT) modules_install
	$(call mv-modules,$(KERNEL_MODULES_SYMBOLS_OUT))
	$(call clean-module-folder,$(KERNEL_MODULES_SYMBOLS_OUT))
	$(MAKE) -C $(KERNEL_DIR) O=$(KERNEL_OUT) ARCH=arm MTK_TARGET_PROJECT=${MTK_TARGET_PROJECT} CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) INSTALL_MOD_PATH=$(KERNEL_MODULES_OUT) INSTALL_MOD_STRIP=1 modules_install
	$(call mv-modules,$(KERNEL_MODULES_OUT))
	$(call clean-module-folder,$(KERNEL_MODULES_OUT))

kernel-modules: kernel | $(KERNEL_MODULES_OUT)

$(INSTALLED_KERNEL_TARGET): kernel

$(INSTALLED_KERNEL_TARGET): $(TARGET_PREBUILT_KERNEL) | $(ACP)
	$(copy-file-to-target)

systemimage: kernel-modules

clean-kernel:
	@rm -rf $(KERNEL_OUT)
	@rm -rf $(KERNEL_MODULES_OUT)

endif
endif
endif
