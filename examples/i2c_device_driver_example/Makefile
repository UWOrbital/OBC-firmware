include .env

CC = "${CC_FOLDER_ROOT}/bin/arm-none-eabi-gcc"
BUILD_DIR = examples/i2c_device_driver_example/build
BIN = i2c_example.out

CC_FLAGS :=
CC_FLAGS += -Og
CC_FLAGS += -g
CC_FLAGS += -gdwarf-3
CC_FLAGS += -gstrict-dwarf
CC_FLAGS += -Wall
CC_FLAGS += -specs="nosys.specs"
CC_FLAGS += -MMD
CC_FLAGS += -std=gnu99

ARM_FLAGS :=
ARM_FLAGS += -mcpu=cortex-r4
ARM_FLAGS += -march=armv7-r
ARM_FLAGS += -mtune=cortex-r4
ARM_FLAGS += -marm
ARM_FLAGS += -mfpu=vfpv3-d16

INCLUDE_DIRS :=
INCLUDE_DIRS += -I"${CC_FOLDER_ROOT}/arm-none-eabi/include"
INCLUDE_DIRS += -I"hal/include"
INCLUDE_DIRS += -I"obc_hal/include"
INCLUDE_DIRS += -I"examples/i2c_device_driver_example/include"

LIBS := 

SRC_DIRS := examples/i2c_device_driver_example
SRC_DIRS += hal/source
SRC_DIRS += obc_hal/source
SRC_DIRS += examples/i2c_device_driver_example/source

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRCS += $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))

OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(file)).o)
DEPS := $(patsubst %.o,%.d,$(OBJS))
OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))
-include $(DEPS)

# OBJS += build/examples/i2c_device_driver_example/main.o

$(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

.PHONY: all
all: $(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $(ARM_FLAGS) $(CC_FLAGS) -Wl,-Map,$@.map -o $@ $(OBJS) -Wl,-T"hal/source/sys_link.ld"

$(BUILD_DIR)/%.o : %.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CC_FLAGS) $(LIBS) -o $@ $< 

$(BUILD_DIR)/%.o : %.s
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CC_FLAGS) $(LIBS) -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
