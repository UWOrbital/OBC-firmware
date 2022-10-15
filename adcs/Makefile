# This is the makefile for inidividual subteam component,
# To allow for build and testing of individual components
# Maintained by individual subteam, not called when building for overall firmware

# Note that this Makefile is based upon posix_example

# Sets compiler to gcc (GNU Compiler Collection)
CC := gcc

# Set the ADCS executable name and build directory
BIN := demo
BUILD_DIR := adcs/build

# Compiler flags
CFLAGS := -ggdb3
CFLAGS += -O3

# Linker flags
LDFLAGS := -ggdb3 -pthread
LDFLAGS += -O3
LDFLAGS += -lm

# Include directories
INCLUDE_DIRS := -I"posix/include"
INCLUDE_DIRS += -I"adcs/include"
INCLUDE_DIRS += -I"adcs/include/propagator/sgp4"

# Source directories
SRC_DIRS := posix/source
SRC_DIRS += adcs/source/propagator
SRC_DIRS += adcs/source/propagator/sgp4
SRC_DIRS += adcs/source

# Preprocessor flags
CPPFLAGS := $(INCLUDE_DIRS) 
CPPFLAGS += -DBUILD_DIR=\"$(BUILD_DIR)\"
CPPFLAGS += -D_WINDOWS_
CPPFLAGS += -DTRACE_ON_ENTER=0
CPPFLAGS += -DprojCOVERAGE_TEST=0

# Get all c files from the source directories
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# Create a list of all the object files based on the c files
OBJS := $(foreach file,$(SRCS),$(BUILD_DIR)/$(basename $(file)).o)

DEPS := $(patsubst %.o,%.d,$(OBJS))
OBJ_DIRS := $(sort $(foreach obj,$(OBJS),$(dir $(obj))))
-include $(DEPS)

$(foreach dir,$(OBJ_DIRS), $(shell mkdir -p $(dir)))

.PHONY: all
all: $(BUILD_DIR)/${BIN}

${BUILD_DIR}/${BIN} : ${OBJS}
	-mkdir -p ${@D}
	$(CC) $^ ${LDFLAGS} -o $@

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR)
