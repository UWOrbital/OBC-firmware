CC                    := gcc
BIN                   := test_param_manager

CURR_DIR_REL          := .
CURR_DIR              := $(abspath $(CURR_DIR_REL))

BUILD_DIR_REL         := ./build
BUILD_DIR             := $(abspath $(BUILD_DIR_REL))

POSIX_DIR_REL         := ../../../posix
POSIX_DIR             := $(abspath $(POSIX_DIR_REL))

INCLUDE_DIRS          := -I.
INCLUDE_DIRS          += -I$(POSIX_DIR)/include
INCLUDE_DIRS          += -I../../include

SOURCE_FILES          := $(wildcard *.c)
SOURCE_FILES          += $(wildcard $(POSIX_DIR)/source/*.c)
SOURCE_FILES          += ../../source/param_manager.c

CFLAGS                :=    -ggdb3
LDFLAGS               :=    -ggdb3 -pthread
CPPFLAGS              :=    $(INCLUDE_DIRS) -DBUILD_DIR=\"$(BUILD_DIR_ABS)\"
CPPFLAGS              +=    -D_WINDOWS_

CPPFLAGS              += -DTRACE_ON_ENTER=0

CPPFLAGS              += -DprojCOVERAGE_TEST=0

CFLAGS                +=   -O3
LDFLAGS               +=   -O3

OBJ_FILES = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

DEP_FILE = $(OBJ_FILES:%.o=%.d)

${BIN} : $(BUILD_DIR)/$(BIN)

${BUILD_DIR}/${BIN} : ${OBJ_FILES}
	-mkdir -p ${@D}
	$(CC) $^ ${LDFLAGS} -o $@

-include ${DEP_FILE}

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c $< -o $@

.PHONY: clean

clean:
	-rm -rf $(BUILD_DIR)
