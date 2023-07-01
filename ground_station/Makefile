CC = gcc
BUILD_DIR = ground_station/build
BIN = gs

CFLAGS = -Wall -g

INCS :=

# Ground station specific headers
INCS += -I"ground_station/common"
INCS += -I"ground_station/serial_utils"

# Headers shared with OBC firmware
INCS += -I"fw/interfaces/common"
INCS += -I"fw/interfaces/obc_gs_interface/commands"
INCS += -I"fw/interfaces/obc_gs_interface/telemetry"
INCS += -I"fw/interfaces/obc_gs_interface/common"
INCS += -I"fw/interfaces/obc_gs_interface/aes128"
INCS += -I"fw/interfaces/obc_gs_interface/fec"
INCS += -I"fw/interfaces/obc_gs_interface/ax25"

INCS += -I"libs/tiny_aes/include"
INCS += -I"libs/libcorrect/include"

# Ground station specific source files
SRCS := ground_station/main.c
SRCS += ground_station/common/gs_heap.c
SRCS += ground_station/serial_utils/win_uart.c

# Source files shared with OBC firmware
SRCS += fw/interfaces/common/data_pack_utils.c
SRCS += fw/interfaces/common/data_unpack_utils.c
SRCS += fw/interfaces/obc_gs_interface/commands/command_pack.c
SRCS += fw/interfaces/obc_gs_interface/commands/command_unpack.c
SRCS += fw/interfaces/obc_gs_interface/telemetry/telemetry_pack.c
SRCS += fw/interfaces/obc_gs_interface/aes128/obc_gs_aes128.c
SRCS += fw/interfaces/obc_gs_interface/ax25/obc_gs_ax25.c
SRCS += fw/interfaces/obc_gs_interface/fec/obc_gs_fec.c

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
