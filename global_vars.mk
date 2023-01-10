# This is a top level makefile
# Used for building the final overall firmware with all components
# Maintained by CDH, only for building overall firmware, should not include individual subcomponent testing code

# Please also use these flags in subsystem compilation as well
# Please be careful when modifying anything here, you should not need to

include .env

CC = "${CC_FOLDER_ROOT}/bin/arm-none-eabi-gcc"
BUILD_DIR = build

ARM_FLAGS :=
ARM_FLAGS += -mcpu=cortex-r4
ARM_FLAGS += -march=armv7-r
ARM_FLAGS += -mtune=cortex-r4
ARM_FLAGS += -marm
ARM_FLAGS += -mfpu=vfpv3-d16

CC_FLAGS :=
# CC_FLAGS += -Os # TODO: Figure out why this breaks the SD Card code
CC_FLAGS += -g
CC_FLAGS += -gdwarf-3
CC_FLAGS += -gstrict-dwarf
CC_FLAGS += -Wall
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

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CPP_FLAGS += -DDEBUG
endif

INCLUDE_DIRS :=
INCLUDE_DIRS += -I"${CC_FOLDER_ROOT}/arm-none-eabi/include"
INCLUDE_DIRS += -I"hal/include"
INCLUDE_DIRS += -I"drivers/include"
INCLUDE_DIRS += -I"common/include"
INCLUDE_DIRS += -I"adcs/include"
INCLUDE_DIRS += -I"cdh/include"
INCLUDE_DIRS += -I"comms/include"
INCLUDE_DIRS += -I"eps/include"
INCLUDE_DIRS += -I"payload/include"
# INCLUDE_DIRS += -I"fatfs/src"
INCLUDE_DIRS += -I"fatfs/port"
INCLUDE_DIRS += -I"reliance_edge/include"
INCLUDE_DIRS += -I"reliance_edge/core/include"
INCLUDE_DIRS += -I"reliance_edge/os/freertos/include"
INCLUDE_DIRS += -I"reliance_edge/projects/newproj/host"


# Reliance Edge File System
INCLUDE_DIRS += -I"reliance_edge/fatfs_port"
INCLUDE_DIRS += -I"reliance_edge/include"
INCLUDE_DIRS += -I"reliance_edge/core/include"
INCLUDE_DIRS += -I"reliance_edge/os/freertos/include"
INCLUDE_DIRS += -I"reliance_edge/projects/freertos_rm46/host"


LIBS := 

$(BUILD_DIR)/%.o : %.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $< 

$(BUILD_DIR)/%.o : %.s
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $<