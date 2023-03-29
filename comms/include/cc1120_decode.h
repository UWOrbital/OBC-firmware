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
#define AES_BLOCK_SIZE 128U

typedef uint8_t aes_block_t[AES_BLOCK_SIZE];

/**
 * @brief strips away the ax.25 headers from a received packet
 * 
 * @param in the received ax.25 frame
 * @param out 255 byte array to store the packet
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
*/
obc_error_code_t ax25Recv(uint8_t *in, uint8_t *out);

/**
 * @brief decodes the reed solomon data and splits it into 2 128B AES blocks
 * 
 * @param in 255 byte array that has encoded reed solomon data
 * @param out 128 byte array to store the AES block
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(uint8_t *in, aes_block_t *out);

/**
 * @brief decrypts the AES blocks
 * 
 * @param in 128 byte AES block that needs to be decrypted
 * @param cmdBytes 128 byte array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_block_t in, uint8_t *cmdBytes);

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