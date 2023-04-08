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
#define DECODE_STACK_SIZE 1024U
#define DECODE_PRIORITY tskIDLE_PRIORITY + 1U

#define LARGEST_COMMAND_SIZE 45U

/**
 * @brief parses the completely decoded data and sends it to the command manager and detects end of transmission
 * 
 * @param cmdBytes 128 byte storing the completely decoded data
 * @param residualBytes (LARGEST_COMMAND_SIZE - 1) byte array with decoded data from previous function call or to store data for next function call if NULL
 * @param residualBytesLen the number of bytes with command information in residualBytes
 * 
 * @return obc_error_code_t - whether or not the data was successfullysent to the command manager
*/
obc_error_code_t tabulateCommands(uint8_t *cmdBytes, uint8_t *residualBytes, uint8_t *residualBytesLen);

/**
 * @brief initializes the decode data pipeline task
 * 
 * @return void
*/
void initDecodeTask(void);

/**
 * @brief send a received packet to the decode data pipeline to be sent to command manager
 * 
 * @param data array storing the packet
 * 
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
*/
obc_error_code_t sendToDecodeDataQueue(uint8_t *data);

#endif /* COMMS_INCLUDE_CC1120_DECODE_H */