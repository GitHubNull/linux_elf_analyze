PROJECT_ROOT := ../..
BUILD_DIR := $(PROJECT_ROOT)/build/kernel
ifneq ($(KERNELRELEASE),)
obj-m := flm_kernel.o
flm_kernel-objs := string.o lm_kernel.o
else
KERNELDIR ?=/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	@rm -rf $(BUILD_DIR)/.tmp_versions \
		$(BUILD_DIR)/*.o \
		$(BUILD_DIR)/.*.cmd \
		$(BUILD_DIR)/Module.symvers \
		$(BUILD_DIR)/*.ko \
		$(BUILD_DIR)/*.mod.c \
		$(BUILD_DIR)/modules.order \
		2>$(BUILD_DIR)/erro_log/rm_erro	
	@echo "build kernel_model" 
	@make -C $(KERNELDIR) M=$(PWD) modules 2>$(BUILD_DIR)/erro_log/make_erro
	mv  -f -t $(BUILD_DIR)/ \
		.tmp_versions \
		.*.*.cmd \
		Module.* \
		*.ko \
		*.o \
		*.mod.c \
		modules.order \
		2>$(BUILD_DIR)/erro_log/mv_erro
clean: 
	@echo "clean kernel_model" 
	rm -rf $(BUILD_DIR)/.tmp_versions \
		$(BUILD_DIR)/*.o \
		$(BUILD_DIR)/.*.cmd \
		$(BUILD_DIR)/Module.symvers \
		$(BUILD_DIR)/*.ko \
		$(BUILD_DIR)/*.mod.c \
		$(BUILD_DIR)/modules.order \
		2>$(BUILD_DIR)/erro_log/rm_erro
endif

# \
# 		$(BUILD_DIR)/.*.*.*.cmd \
# 		$(BUILD_DIR)/.*.*.cmd \
# 		$(BUILD_DIR)/Module.* \
# 		$(BUILD_DIR)/*.ko \
# 		$(BUILD_DIR)/*.mod.c \
# 		$(BUILD_DIR)/modules.order \
# 		$(BUILD_DIR)/*.mod.o \
# 		$(BUILD_DIR)/*.o \		