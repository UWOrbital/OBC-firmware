include .env

CC = "${CC_FOLDER_ROOT}/bin/arm-none-eabi-gcc"
BUILD_DIR = examples/fram_persist_demo/build
BIN = fram_persist_demo.out

ARM_FLAGS :=
ARM_FLAGS += -mcpu=cortex-r4
ARM_FLAGS += -march=armv7-r
ARM_FLAGS += -mtune=cortex-r4
ARM_FLAGS += -marm
ARM_FLAGS += -mfpu=vfpv3-d16

CC_FLAGS :=
#CC_FLAGS += -Og
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

INCLUDE_DIRS :=
INCLUDE_DIRS += -I"${CC_FOLDER_ROOT}/arm-none-eabi/include"
INCLUDE_DIRS += -I"hal/include"
INCLUDE_DIRS += -I"drivers/common/include"
INCLUDE_DIRS += -I"drivers/fram/include"
INCLUDE_DIRS += -I"common/include"
INCLUDE_DIRS += -I"cdh/include"
INCLUDE_DIRS += -I"drivers/ds3232/include"
INCLUDE_DIRS += -I"drivers/cc1120/include"
INCLUDE_DIRS += -I"drivers/lm75bd/include"

INCLUDE_DIRS +=-I"comms/include"
INCLUDE_DIRS +=-I"eps/include"
INCLUDE_DIRS +=-I"adcs/include"
INCLUDE_DIRS +=-I"cdh/include"
INCLUDE_DIRS +=-I"payload/include"
# Reliance Edge File System
INCLUDE_DIRS += -I"reliance_edge/fatfs_port"
INCLUDE_DIRS += -I"reliance_edge/include"
INCLUDE_DIRS += -I"reliance_edge/core/include"
INCLUDE_DIRS += -I"reliance_edge/os/freertos/include"
INCLUDE_DIRS += -I"reliance_edge/projects/freertos_rm46/host"

INCLUDE_DIRS += -I"tiny_aes"
INCLUDE_DIRS += -I"libcorrect/include"

LIBS :=

SRC_DIRS :=
SRC_DIRS += hal/source
SRC_DIRS += drivers/common/source
SRC_DIRS += drivers/fram/source
SRC_DIRS += common/source
SRC_DIRS += examples/fram_persist_demo
SRC_DIRS += cdh/source
SRC_DIRS += drivers/ds3232/source
SRC_DIRS += drivers/cc1120/source
SRC_DIRS += drivers/lm75bd/source

SRC_DIRS += comms/source
SRC_DIRS += eps/source
SRC_DIRS += adcs/source
SRC_DIRS += payload/source

# Reliance Edge File System
SRC_DIRS += reliance_edge/fatfs_port
SRC_DIRS += reliance_edge/bdev
SRC_DIRS += reliance_edge/core/driver
SRC_DIRS += reliance_edge/fse
SRC_DIRS += reliance_edge/os/freertos/services
SRC_DIRS += reliance_edge/posix
SRC_DIRS += reliance_edge/util
SRC_DIRS += reliance_edge/projects/freertos_rm46/host

SRC_DIRS += tiny_aes
SRC_DIRS += libcorrect/src/reed-solomon

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRCS += $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))

OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(file)).o)
DEPS := $(patsubst %.o,%.d,$(OBJS))
OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))
-include $(DEPS)

$(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

all: $(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $(ARM_FLAGS) $(CC_FLAGS) -Wl,-Map,$@.map -o $@ $(OBJS) -Wl,-T"hal/source/sys_link.ld"

$(BUILD_DIR)/%.o : %.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $<

$(BUILD_DIR)/%.o : %.s
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
