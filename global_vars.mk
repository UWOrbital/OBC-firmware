# This is a top level makefile
# Used for building the final overall firmware with all components
# Maintained by CDH, only for building overall firmware, should not include individual subcomponent testing code

# Please also use these flags in subsystem compilation as well
# Please be careful when modifying anything here, you should not need to

ROOT_DIR ?= .
BUILD_DIR ?= $(ROOT_DIR)/build

include $(ROOT_DIR)/.env

CC = "${CC_FOLDER_ROOT}/bin/arm-none-eabi-gcc"

ARM_FLAGS :=
ARM_FLAGS += -mcpu=cortex-r4
ARM_FLAGS += -march=armv7-r
ARM_FLAGS += -mtune=cortex-r4
ARM_FLAGS += -marm
ARM_FLAGS += -mfpu=vfpv3-d16

CC_FLAGS :=
CC_FLAGS += -g
CC_FLAGS += -gdwarf-3
CC_FLAGS += -gstrict-dwarf
CC_FLAGS += -Wall
CC_FLAGS += -Wextra
CC_FLAGS += -Wno-unused-parameter
# CC_FLAGS += -Werror TODO: Enable this if we solve all hal warnings

CC_FLAGS += -fstack-usage
CC_FLAGS += -fdump-ipa-cgraph

CC_FLAGS += -specs="nosys.specs"
CC_FLAGS += -MMD
CC_FLAGS += -std=gnu99

CPP_FLAGS :=

BOARD_TYPE ?= RM46_LAUNCHPAD
CPP_FLAGS += -D$(BOARD_TYPE)

LOG_OUTPUT ?= LOG_TO_UART
CPP_FLAGS += -DLOG_DEFAULT_OUTPUT_LOCATION=$(LOG_OUTPUT) 

LOG_LEVEL ?= LOG_TRACE
CPP_FLAGS += -DLOG_DEFAULT_LEVEL=$(LOG_LEVEL)

CMD_POLICY ?= CMD_POLICY_RND
CPP_FLAGS += -DOBC_ACTIVE_POLICY=$(CMD_POLICY) 

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CPP_FLAGS += -DDEBUG
endif

INCLUDE_DIRS :=
INCLUDE_DIRS += -I"${CC_FOLDER_ROOT}/arm-none-eabi/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/hal/include"

# Drivers
INCLUDE_DIRS += -I"$(ROOT_DIR)/drivers/common/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/drivers/ds3232/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/drivers/fram/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/drivers/lm75bd/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/drivers/cc1120/include"

INCLUDE_DIRS += -I"$(ROOT_DIR)/common/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/adcs/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/cdh/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/comms/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/eps/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/payload/include"

# Reliance Edge File System
INCLUDE_DIRS += -I"$(ROOT_DIR)/reliance_edge/fatfs_port"
INCLUDE_DIRS += -I"$(ROOT_DIR)/reliance_edge/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/reliance_edge/core/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/reliance_edge/os/freertos/include"
INCLUDE_DIRS += -I"$(ROOT_DIR)/reliance_edge/projects/freertos_rm46/host"

INCLUDE_DIRS += -I"$(ROOT_DIR)/tiny_aes"

LIBS := 

$(BUILD_DIR)/%.o : $(ROOT_DIR)/%.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $< 

$(BUILD_DIR)/%.o : $(ROOT_DIR)/%.s
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $<