PROJECT_ROOT := ../..
BUILD_DIR := $(PROJECT_ROOT)/build/kernel
ifneq ($(KERNELRELEASE),)
obj-m :=hello_kernel.o
hello_kernel-objs := fork.o exec.o binfmt_elf.o hello.o 
else
KERNELDIR ?=/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	@rm -rf -R $(BUILD_DIR)/.[!.]*
	@echo "build kernel_model" 
	@make -C $(KERNELDIR) M=$(PWD) modules 2>$(BUILD_DIR)/make_erro
	mv  -f -t $(BUILD_DIR)/ \
		.tmp_versions \
		.*.*.cmd \
		Module.* \
		*.ko \
		*.o \
		*.mod.c \
		modules.order \
		2>$(BUILD_DIR)/mv_erro
clean: 
	@echo "clean kernel_model" 
	rm -rf $(BUILD_DIR)/.[!.]* $(BUILD_DIR)/* \
		2>$(BUILD_DIR)/rm_erro
endif	