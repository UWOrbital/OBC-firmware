CC = gcc
BUILD_DIR = ground_station/build
BIN = gs

CFLAGS = -Wall -g

INCS :=

# Ground station specific headers
INCS += -I"ground_station/cmd_utils"
INCS += -I"ground_station/serial_utils"

# Headers shared with OBC firmware
INCS += -I"cdh/include"
INCS += -I"common/include"
INCS += -I"libs/tiny_aes/include"
INCS += -I"libs/libcorrect/include"
INCS += -I"comms/include"

# Ground station specific source files
SRCS := ground_station/main.c
SRCS += ground_station/common/gs_heap.c
SRCS += ground_station/common/gs_logging.c
SRCS += ground_station/cmd_utils/command_pack.c
SRCS += ground_station/serial_utils/win_uart.c

# Source files shared with OBC firmware
SRCS += common/source/obc_pack_utils.c
SRCS += common/source/obc_unpack_utils.c
SRCS += comms/source/ax25.c
SRCS += comms/source/fec.c
SRCS += comms/source/aes128.c

SRCS += libs/tiny_aes/source/aes.c

SRCS += libs/libcorrect/source/reed-solomon/decode.c
SRCS += libs/libcorrect/source/reed-solomon/encode.c
SRCS += libs/libcorrect/source/reed-solomon/polynomial.c
SRCS += libs/libcorrect/source/reed-solomon/reed-solomon.c

# Define the object files
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))

.PHONY: all clean

all: $(BUILD_DIR)/$(BIN)
	@echo  The program has been compiled

$(BUILD_DIR)/$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(INCS) $<  -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(RM) -rf $(BUILD_DIR)/*
