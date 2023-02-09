# This is a top level makefile
# Used for building the final overall firmware with all components
# Maintained by CDH, only for building overall firmware, should not include individual subcomponent testing code

include global_vars.mk
include sources.mk

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRCS += $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))

OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(file)).o)
DEPS := $(patsubst %.o,%.d,$(OBJS))
OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))
-include $(DEPS)

OBJS += $(BUILD_DIR)/main.o

$(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

all: $(BUILD_DIR)/OBC-firmware.out

$(BUILD_DIR)/OBC-firmware.out: $(OBJS)
	$(CC) $(ARM_FLAGS) $(CC_FLAGS) -Wl,-Map,$@.map -o $@ $(OBJS) -Wl,-T"$(ROOT_DIR)/hal/source/sys_link.ld"


clean:
	rm -rf $(BUILD_DIR)/*
	rm -f $(ROOT_DIR)/hal/source/sys_main.c

.PHONY: all clean
