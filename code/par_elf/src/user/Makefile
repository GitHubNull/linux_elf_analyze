CC := gcc  
CFLAGS := -std=gnu99
PROJECT_ROOT := ../..
BUILD_DIR := $(PROJECT_ROOT)/build/user
BUILD_INCLUDE_DIR := $(PROJECT_ROOT)/include/user
BUILD_COMMENT_DIR := $(PROJECT_ROOT)/unit
USER_C_SRC := $(shell pwd)

default:
	@echo "build app..."
	$(CC) $(CFLAGS) client.c client_nlk.c str_pack.c -o app \
	2>$(BUILD_DIR)/erro_log/make_erro
	mv app $(BUILD_DIR)/ 2>$(BUILD_DIR)/erro_log/mv_erro

clean:
	rm $(BUILD_DIR)/app \
	2>$(BUILD_DIR)/erro_log/rm_erro