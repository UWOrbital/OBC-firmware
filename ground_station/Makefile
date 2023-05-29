CC = gcc
BUILD_DIR = ground_station/build

CFLAGS = -Wall -g

INCS := 
INCS += -I"cdh/include"
INCS += -I"common/include"
INCS += -I"tiny_aes"
INCS += -I"libcorrect/include"

SRCS := ground_station/main.c
SRCS += ground_station/gs_heap.c
SRCS += common/source/obc_pack_utils.c
SRCS += common/source/obc_unpack_utils.c

# tiny_aes
SRCS += tiny_aes/aes.c

# libcorrect
SRCS += libcorrect/src/reed-solomon/decode.c
SRCS += libcorrect/src/reed-solomon/encode.c
SRCS += libcorrect/src/reed-solomon/polynomial.c
SRCS += libcorrect/src/reed-solomon/reed-solomon.c

# Define the object files 
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))

# Define the executable file 
MAIN = $(BUILD_DIR)/ground_station

.PHONY: all clean

all: $(MAIN)
	@echo  The program has been compiled

$(MAIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(INCS) $<  -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(RM) -rf $(BUILD_DIR)/*
