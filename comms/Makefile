# This is the makefile for inidividual subteam component,
# To allow for build and testing of individual components
# Maintained by individual subteam, not called when building for overall firmware
ROOT_DIR = ..
BUILD_DIR = build

include $(ROOT_DIR)/global_vars.mk

SRC_DIRS :=
SRC_DIRS += hal/source
SRC_DIRS += drivers/source
SRC_DIRS += common/source
SRC_DIRS += comms/source

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(ROOT_DIR)/$(dir)/*.c))
SRCS += $(foreach dir,$(SRC_DIRS),$(wildcard $(ROOT_DIR)/$(dir)/*.s))

OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(subst $(ROOT_DIR)/,,$(file))).o)
DEPS := $(patsubst %.o,%.d,$(OBJS))
OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))
-include $(DEPS)

OBJS += $(BUILD_DIR)/main.o

$(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

all: $(BUILD_DIR)/comms.out

$(BUILD_DIR)/main.o: main.c
	$(CC) -c $(ARM_FLAGS) $(INCLUDE_DIRS) $(CPP_FLAGS) $(CC_FLAGS) $(LIBS) -o $@ $< 

$(BUILD_DIR)/comms.out: $(OBJS)
	$(CC) $(ARM_FLAGS) $(CC_FLAGS) -Wl,-Map,$@.map -o $@ $(OBJS) -Wl,-T"$(ROOT_DIR)/hal/source/sys_link.ld"


clean:
	rm -rf $(BUILD_DIR)/*
	rm -f $(ROOT_DIR)/hal/source/sys_main.c

.PHONY: all clean
