
CC = "${CC_FOLDER_ROOT}/bin/arm-none-eabi-gcc"
BUILD_DIR = build
BIN = max17320.out

ARM_FLAGS :=
ARM_FLAGS += -mcpu=cortex-r4
ARM_FLAGS += -march=armv7-r
ARM_FLAGS += -mtune=cortex-r4
ARM_FLAGS += -marm
ARM_FLAGS += -mfpu=vfpv3-d16

CC_FLAGS :=
CC_FLAGS += -O0
CC_FLAGS += -g
CC_FLAGS += -gdwarf-3
CC_FLAGS += -gstrict-dwarf
CC_FLAGS += -Wall
CC_FLAGS += -specs="nosys.specs"
CC_FLAGS += -MMD
CC_FLAGS += -std=gnu99

CPP_FLAGS :=

BOARD_TYPE ?= OBC_REVISION_1
CPP_FLAGS += -D$(BOARD_TYPE)

LOG_OUTPUT ?= LOG_TO_UART
CPP_FLAGS += -DLOG_DEFAULT_OUTPUT_LOCATION=$(LOG_OUTPUT)

LOG_LEVEL ?= LOG_TRACE
CPP_FLAGS += -DLOG_DEFAULT_LEVEL=$(LOG_LEVEL)

INCLUDE_DIRS :=
INCLUDE_DIRS += -I"${CC_FOLDER_ROOT}/arm-none-eabi/include"
INCLUDE_DIRS += -I"../hal/obc_rev2/include"
INCLUDE_DIRS += -I"../drivers/*"
INCLUDE_DIRS += -I"../config/"
INCLUDE_DIRS += -I"../modules/*"
INCLUDE_DIRS += -I"../reliance_edge/*"
INCLUDE_DIRS += -I"../sys/*"

LIBS :=

SRC_DIRS :=
SRC_DIRS += $(wildcard ../hal/common)
SRC_DIRS += $(wildcard ../hal/obc_rev2/source)
SRC_DIRS += $(wildcard .../modules/*)
SRC_DIRS += $(wildcard ../drivers/*)
SRC_DIRS += $(wildcard ../config/*)
SRC_DIRS += $(wildcard ../sys/*/)


SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRCS += $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))

OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(file)).o)
DEPS := $(patsubst %.o,%.d,$(OBJS))
OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))
-include $(DEPS)

$(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

all: $(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $(ARM_FLAGS) $(CC_FLAGS) -o $@ $(OBJS) -Wl,-T"../..//hal/obc_rev2/source/sys_link.ld"

$(BUILD_DIR)/%.o : %.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $<

$(BUILD_DIR)/%.o : %.s
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
