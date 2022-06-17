CC                    := gcc
BIN                   := posix_demo

CURR_DIR_REL          := .
CURR_DIR              := $(abspath $(CURR_DIR_REL))

BUILD_DIR_REL         := ./build
BUILD_DIR             := $(abspath $(BUILD_DIR_REL))

HAL_DIR_REL           := ../../hal
HAL_DIR               := $(abspath $(HAL_DIR_REL))

INCLUDE_DIRS          := -I.
INCLUDE_DIRS          += -I../include/
INCLUDE_DIRS          += -I${HAL_DIR}/include/FreeRTOS/Linux

SOURCE_FILES          := $(wildcard *.c)
SOURCE_FILES          += $(wildcard ${HAL_DIR}/source/FreeRTOS/Linux/*.c)
SOURCE_FILES          += ${HAL_DIR}/source/FreeRTOS/Linux/MemMang/heap_3.c

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
