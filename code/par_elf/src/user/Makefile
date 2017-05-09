PROJECT_ROOT := ../..
BUILD_DIR := $(PROJECT_ROOT)/build/user
USER_C_SRC := $(shell pwd)
default:
	@echo "build user_client"
	gcc -ggdb client.c -o $(BUILD_DIR)/app 2>$(BUILD_DIR)/erro_log/make_erro
clean:
	@rm $(BUILD_DIR)/app 2>$(BUILD_DIR)/erro_log/rm_erro