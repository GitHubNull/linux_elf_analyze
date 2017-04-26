PROJECT_ROOT := ../..
BUILD_DIR := $(PROJECT_ROOT)/build/user
USER_C_SRC := $(shell pwd)
default:
	@echo "build user_client"
	gcc client.c -o app 2>$(BUILD_DIR)/erro_log/make_erro
	mv app $(BUILD_DIR)/ 2>$(BUILD_DIR)/erro_log/mv_erro
clean:
	@rm $(BUILD_DIR)/app \
		$(BUILD_DIR)/*.o \
	 2>$(BUILD_DIR)/erro_log/rm_erro