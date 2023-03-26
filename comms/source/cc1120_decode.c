#include "cc1120_decode.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Decode Data task
static TaskHandle_t decodeTaskHandle = NULL;
static StaticTask_t decodeTaskBuffer;
static StackType_t decodeTaskStack[DECODE_STACK_SIZE];

// Decode Data Queue
static QueueHandle_t decodeDataQueueHandle = NULL;
static StaticQueue_t decodeDataQueue;
static uint8_t decodeDataQueueStack[DECODE_DATA_QUEUE_LENGTH*DECODE_DATA_QUEUE_ITEM_SIZE];

static void vDecodeTask(void * pvParameters);

/**
 * @brief strips away the ax.25 headers from a received packet
 * 
 * @param in the received ax.25 frame
 * @param out 255 byte array to store the packet
 * 
 * @return obc_error_code_t - whether or not the ax.25 headers were successfully stripped
*/
obc_error_code_t ax25Recv(uint8_t *in, uint8_t *out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

/**
 * @brief decodes the reed solomon data and splits it into 2 128B AES blocks
 * 
 * @param in 255 byte array that has encoded reed solomon data
 * @param out 128 byte array to store the AES block
 * 
 * @return obc_error_code_t - whether or not the data was successfully decoded
*/
obc_error_code_t rsDecode(uint8_t *in, aes_block_t *out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

/**
 * @brief decrypts the AES blocks
 * 
 * @param in 128 byte AES block that needs to be decrypted
 * @param cmdBytes 128 byte array to store the decrypted data
 * 
 * @return obc_error_code_t - whether or not the data was successfully decrypted
*/
obc_error_code_t aes128Decrypt(aes_block_t in, uint8_t *cmdBytes){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

/**
 * @brief parses the completely decoded data and sends it to the command manager
 * 
 * @param cmdBytes 128 byte storing the completely decoded data
 * @param residualBytes (LARGEST_COMMAND_SIZE - 1) byte array with decoded data from previous function call or to store data for next function call if NULL
 * @param residualBytesLen the number of bytes with command information in residualBytes
 * 
 * @return obc_error_code_t - whether or not the data was successfullysent to the command manager
*/
obc_error_code_t tabulateCommands(uint8_t *cmdBytes, uint8_t *residualBytes, uint8_t *residualBytesLen){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    /*
    - if residualBytesLen != 0, then parse the first (residualBytesLen) Bytes of residualBytes for commands 
    - and store them in cmd_msg_t structs
    - parse cmdBytes and store the data in cmd_msg_t structs
    - if a command is cut off, store those bytes in residualBytes to be parsed in the next function call and store
    - the number of bytes in residualBytes in residualBytesLen
    */
    return errCode;
}

/**
 * @brief initializes the decode data pipeline task
 * 
 * @return void
*/
void initDecodeTask(void){
    ASSERT( (decodeTaskStack != NULL) && (&decodeTaskBuffer != NULL) );
    if(decodeTaskHandle == NULL){
        decodeTaskHandle = xTaskCreateStatic(vDecodeTask, DECODE_TASK_NAME, DECODE_STACK_SIZE, NULL, DECODE_PRIORITY, decodeTaskStack, &decodeTaskBuffer);
    }
    ASSERT( (decodeDataQueueStack != NULL) && (&decodeDataQueue != NULL) );
    if(decodeDataQueueHandle == NULL){
        decodeDataQueueHandle = xQueueCreateStatic(DECODE_DATA_QUEUE_LENGTH, DECODE_DATA_QUEUE_ITEM_SIZE, decodeDataQueueStack, &decodeDataQueue);
    }
}

/**
 * @brief takes a received packet from a vRecvTask and completely decodes it and sends the commands to command manager
 * 
 * @param pvParamaters NULL
 * 
 * @return void
*/
static void vDecodeTask(void * pvParameters){
    obc_error_code_t errCode;
    while (1) {
        uint8_t data[278];
        if(xQueueReceive(decodeDataQueueHandle, data, DECODE_DATA_QUEUE_RX_WAIT_PERIOD) == pdPASS){
            uint8_t axPacket[255];

            LOG_IF_ERROR_CODE(ax25Recv(data, axPacket));

            aes_block_t aesBlocks[2];
            
            LOG_IF_ERROR_CODE(rsDecode(data, aesBlocks[0]));

            LOG_IF_ERROR_CODE(rsDecode(data+128, aesBlocks[1]));

            uint8_t commands[128];
            uint8_t residual[LARGEST_COMMAND_SIZE - 1];
            uint8_t residualLen;

            LOG_IF_ERROR_CODE(aes128Decrypt(aesBlocks[0], commands));

            LOG_IF_ERROR_CODE(tabulateCommands(commands, residual, &residualLen));

            LOG_IF_ERROR_CODE(aes128Decrypt(aesBlocks[1], commands));

            LOG_IF_ERROR_CODE(tabulateCommands(commands, residual, &residualLen));
        }

    }
} 

/**
 * @brief send a received packet to the decode data pipeline to be sent to command manager
 * 
 * @param data array storing the packet
 * 
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
*/
obc_error_code_t sendToDecodeDataQueue(uint8_t *data) {
    if (decodeDataQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xQueueSend(decodeDataQueueHandle, (void *) data, DECODE_DATA_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}