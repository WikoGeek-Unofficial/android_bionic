PRIVATE_CUSTOM_KERNEL_DCT:= $(if $(CUSTOM_KERNEL_DCT),$(CUSTOM_KERNEL_DCT),dct)
DRVGEN_TOOL := $(PWD)/tools/dct/DrvGen
DWS_FILE := $(PWD)/arch/arm/mach-$(MTK_PLATFORM)/$(MTK_PROJECT)/dct/$(PRIVATE_CUSTOM_KERNEL_DCT)/codegen.dws

ifndef DRVGEN_OUT
DRVGEN_OUT := $(objtree)/arch/arm/mach-$(MTK_PLATFORM)/$(MTK_PROJECT)/dct/$(PRIVATE_CUSTOM_KERNEL_DCT)
endif
export DRVGEN_OUT

DRVGEN_OUT_PATH := $(DRVGEN_OUT)/inc

DRVGEN_FILE_LIST := $(DRVGEN_OUT)/inc/cust_kpd.h \
		    $(DRVGEN_OUT)/inc/cust_eint.h \
		    $(DRVGEN_OUT)/inc/cust_gpio_boot.h \
		    $(DRVGEN_OUT)/inc/cust_gpio_usage.h \
		    $(DRVGEN_OUT)/inc/cust_adc.h \
		    $(DRVGEN_OUT)/inc/cust_eint_md1.h \
		    $(DRVGEN_OUT)/inc/cust_power.h \
		    $(DRVGEN_OUT)/inc/pmic_drv.h \
		    $(DRVGEN_OUT)/pmic_drv.c

archprepare: $(DRVGEN_FILE_LIST)
drvgen: $(DRVGEN_FILE_LIST)

$(DRVGEN_OUT)/inc/cust_kpd.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) kpd_h

$(DRVGEN_OUT)/inc/cust_eint.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) eint_h

$(DRVGEN_OUT)/inc/cust_gpio_boot.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) gpio_boot_h

$(DRVGEN_OUT)/inc/cust_gpio_usage.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) gpio_usage_h

$(DRVGEN_OUT)/inc/cust_adc.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) adc_h

$(DRVGEN_OUT)/inc/cust_eint_md1.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) md1_eint_h

$(DRVGEN_OUT)/inc/cust_power.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) power_h

$(DRVGEN_OUT)/inc/pmic_drv.h: $(DRVGEN_TOOL) $(DWS_FILE)
	@mkdir -p $(DRVGEN_OUT_PATH)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT_PATH) $(DRVGEN_OUT_PATH) pmic_h

$(DRVGEN_OUT)/pmic_drv.c: $(DRVGEN_TOOL) $(DWS_FILE)
	@$(DRVGEN_TOOL) $(DWS_FILE) $(DRVGEN_OUT) $(DRVGEN_OUT_PATH) pmic_c
