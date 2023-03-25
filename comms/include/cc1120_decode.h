#ifndef COMMS_INCLUDE_CC1120_DECODE_H
#define COMMS_INCLUDE_CC1120_DECODE_H

#include "obc_logging.h"
#include "cc1120_txrx.h"

#include <os_portmacro.h>

#include <stdbool.h>

#define DECODE_DATA_QUEUE_LENGTH 10U
#define DECODE_DATA_QUEUE_ITEM_SIZE RX_EXPECTED_PACKET_SIZE
#define DECODE_DATA_QUEUE_RX_WAIT_PERIOD portMAX_DELAY 
#define DECODE_DATA_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

#define DECODE_TASK_NAME "decode_task"
#define DECODE_STACK_SIZE 512U
#define DECODE_PRIORITY 1U

#define LARGEST_COMMAND_SIZE 45U
#define AES_BLOCK_SIZE 128U

typedef uint8_t aes_block_t[AES_BLOCK_SIZE];

obc_error_code_t ax25Recv(uint8_t *in, uint8_t *out);

obc_error_code_t rsDecode(uint8_t *in, aes_block_t *out);

obc_error_code_t aes128Decrypt(aes_block_t in, uint8_t *cmdBytes);

obc_error_code_t tabulateCommands(uint8_t *cmdBytes, uint8_t *residualBytes);

void initDecodeTask(void);

obc_error_code_t SendToDecodeDataQueue(uint8_t *data);

QueueHandle_t getDecodeQueueHandle(void);

#endif /* COMMS_INCLUDE_CC1120_DECODE_H */