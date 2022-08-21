# This example Makefile builds for a Posix system

# The compiler is gcc (GNU Compiler Collection)
CC := gcc

# Name for the output executable file
BIN := posix_example

# The build directory
BUILD_DIR := examples/posix_demo/build

# Compiler flags
CFLAGS := -ggdb3
CFLAGS += -O3

# Linker flags
LDFLAGS := -ggdb3 -pthread
LDFLAGS += -O3

# The include directories (the ones with header files)
INCLUDE_DIRS := -I"posix/include"
INCLUDE_DIRS += -I"examples/posix_demo/include"

# The source directories (the ones with source files)
SRC_DIRS := posix/source
SRC_DIRS += examples/posix_demo/source
SRC_DIRS += examples/posix_demo

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
