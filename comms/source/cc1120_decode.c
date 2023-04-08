#include "cc1120_decode.h"
#include "obc_logging.h"
#include "aes128.h"
#include "ax25.h"
#include "fec.h"
#include "cc1120_defs.h"
#include "command_unpack.h"
#include "command_data.h"
#include "command_manager.h"

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
 * @brief parses the completely decoded data and sends it to the command manager and detects end of transmission
 * 
 * @param cmdBytes 256 byte array storing the completely decoded data
 * 
 * @return obc_error_code_t - whether or not the data was successfullysent to the command manager
*/
obc_error_code_t handleCommands(uint8_t *cmdBytes){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    if(cmdBytes == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }
    uint32_t bytesRead = 0;
    while(bytesRead < CC1120_MAX_PACKET_LEN){
        cmd_msg_t command;
        RETURN_IF_ERROR_CODE(unpackCmdMsg(cmdBytes, &bytesRead, &command));
        RETURN_IF_ERROR_CODE(sendToCommandQueue(&command));
    }
    return OBC_ERR_CODE_SUCCESS;
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
    packed_ax25_packet_t data;
    packed_rs_packet_t rsData;
    aes_block_t *aesBlocks[2];
    uint8_t serializedDecryptedData[2*AES_BLOCK_SIZE];
    while (1) {
        if(xQueueReceive(decodeDataQueueHandle, &data, DECODE_DATA_QUEUE_RX_WAIT_PERIOD) == pdPASS){
            // Strip away the ax.25 headers from the data and store the encode reed solomon data in rsData
            LOG_IF_ERROR_CODE(ax25Recv(&data, &rsData));
            // Decode the reed solomon data and store it in aesBlocks
            LOG_IF_ERROR_CODE(rsDecode(rsData.data, aesBlocks));
            // Decrypt the first aes128 block and store it in the first AES_BLOCK_SIZE bytes of serializedDecryptedData
            LOG_IF_ERROR_CODE(aes128Decrypt(aesBlocks[0], serializedDecryptedData));
            // Decrypt the second aes128 block and store it in the second half of serializedDecryptedData
            LOG_IF_ERROR_CODE(aes128Decrypt(aesBlocks[1], serializedDecryptedData + AES_BLOCK_SIZE));
            // Parse the serializedDecryptedData into cmd_msg_t structs and send them to command manager
            LOG_IF_ERROR_CODE(handleCommands(serializedDecryptedData));
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
obc_error_code_t sendToDecodeDataQueue(packed_ax25_packet_t *data) {
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