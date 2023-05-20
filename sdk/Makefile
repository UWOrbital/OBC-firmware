CC = gcc
BUILD_DIR = sdk/build

CFLAGS = -Wall -g

INCS := 
INCS += -I"cdh/include"
INCS += -I"common/include"

SRCS := sdk/main.c
SRCS += common/source/obc_pack_utils.c
SRCS += common/source/obc_unpack_utils.c
# SRCS += cdh/source/command_pack.c

# Define the object files 
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))

# Define the executable file 
MAIN = $(BUILD_DIR)/sdk

.PHONY: depend clean

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
	$(RM) -rf $(BUILD_DIR)/*.o *~ $(MAIN)
