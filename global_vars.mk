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
CC_FLAGS += -Og
CC_FLAGS += -g
CC_FLAGS += -gdwarf-3
CC_FLAGS += -gstrict-dwarf
CC_FLAGS += -Wall
CC_FLAGS += -specs="nosys.specs"
CC_FLAGS += -MMD
CC_FLAGS += -std=gnu99

INCLUDE_DIRS :=
INCLUDE_DIRS += -I"${CC_FOLDER_ROOT}/arm-none-eabi/include"
INCLUDE_DIRS += -I"hal/include"
INCLUDE_DIRS += -I"adcs/include"
INCLUDE_DIRS += -I"cdh/include"
INCLUDE_DIRS += -I"comms/include"
INCLUDE_DIRS += -I"payload/include"

LIBS := 

$(BUILD_DIR)/%.o : %.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CC_FLAGS) $(LIBS) -o $@ $< 

$(BUILD_DIR)/%.o : %.s
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CC_FLAGS) $(LIBS) -o $@ $<