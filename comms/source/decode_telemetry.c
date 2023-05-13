#include "decode_telemetry.h"
#include "obc_logging.h"
#include "aes128.h"
#include "ax25.h"
#include "fec.h"
#include "cc1120_defs.h"
#include "command_unpack.h"
#include "command_data.h"
#include "command_manager.h"
#include "obc_task_config.h"
#include "cc1120_txrx.h"
#include "command_id.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_semphr.h>

#include <sys_common.h>
#include <gio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DECODE_DATA_QUEUE_LENGTH 10U
#define DECODE_DATA_QUEUE_ITEM_SIZE AX25_PKT_LEN
#define DECODE_DATA_QUEUE_RX_WAIT_PERIOD portMAX_DELAY 
#define DECODE_DATA_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

// Decode Data task
static TaskHandle_t decodeTaskHandle = NULL;
static StaticTask_t decodeTaskBuffer;
static StackType_t decodeTaskStack[COMMS_DECODE_STACK_SIZE];

// Decode Data Queue
static QueueHandle_t decodeDataQueueHandle = NULL;
static StaticQueue_t decodeDataQueue;
static uint8_t decodeDataQueueStack[DECODE_DATA_QUEUE_LENGTH*DECODE_DATA_QUEUE_ITEM_SIZE];

static void vDecodeTask(void * pvParameters);
static obc_error_code_t decodePacket(packed_ax25_packet_t *data, packed_rs_packet_t *rsData, aes_block_t *aesBlock[]);

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
    uint32_t bytesUnpacked = 0;
    // Keep unpacking cmdBytes into cmd_msg_t commands to send to command manager until we have unpacked all the bytes in cmdBytes
    // If the command id is the id for end of transmission, isStillUplinking should be set to false
    while(bytesUnpacked < AES_BLOCK_SIZE){
        // Check if we have reached
        if(cmdBytes[bytesUnpacked] == CMD_END_OF_TRANSMISSION){
            isStillUplinking = false;
            // give RX semaphore so that the cc1120Receive can unblock and return
            if(xSemaphoreGive(getCC1120RxSemaphoreHandle()) != pdPASS){
                LOG_ERROR_CODE(OBC_ERR_CODE_SEMAPHORE_FULL);
                return OBC_ERR_CODE_SEMAPHORE_FULL;
            }
            return OBC_ERR_CODE_SUCCESS;
        }
        if(cmdBytes[bytesUnpacked] == 0){
            // means we have reached the end of the packet and rest can be ignored
            return OBC_ERR_CODE_SUCCESS;
        }
        cmd_msg_t command;
        RETURN_IF_ERROR_CODE(unpackCmdMsg(cmdBytes, &bytesUnpacked, &command));
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
        decodeTaskHandle = xTaskCreateStatic(vDecodeTask, COMMS_DECODE_TASK_NAME, COMMS_DECODE_STACK_SIZE, NULL, COMMS_DECODE_PRIORITY, decodeTaskStack, &decodeTaskBuffer);
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
    aes_block_t *aesBlocks[(REED_SOLOMON_DECODED_BYTES - IV_BYTES_PER_TRANSMISSION) / AES_BLOCK_SIZE];
    while (1) {
        if(xQueueReceive(decodeDataQueueHandle, &data, DECODE_DATA_QUEUE_RX_WAIT_PERIOD) == pdPASS){
            LOG_IF_ERROR_CODE(decodePacket(&data, &rsData, aesBlocks));
        }
    }
} 

/**
 * @brief completely decode a recieved packet
 * 
 * @param data - packed ax25 packet with received data
 * @param rsData - holds packed reed solomon data
 * @param aesBlock - pointer to an array of aesBlocks that need to be decrypted
 * @param decryptedData - holds the decrypted data from the aesBlock
 * 
 * @return obc_error_code_t - whether or not the data was completely decoded successfully 
*/
static obc_error_code_t decodePacket(packed_ax25_packet_t *data, packed_rs_packet_t *rsData, aes_block_t *aesBlocks[]) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(ax25Recv(data, rsData));
    RETURN_IF_ERROR_CODE(rsDecode(rsData, aesBlocks));
    for(uint8_t i = 0; i < ((REED_SOLOMON_DECODED_BYTES - IV_BYTES_PER_TRANSMISSION) / AES_BLOCK_SIZE); ++i){
        uint8_t decryptedData[AES_BLOCK_SIZE];
        RETURN_IF_ERROR_CODE(aes128Decrypt(aesBlocks[i], decryptedData));
        RETURN_IF_ERROR_CODE(handleCommands(decryptedData));
    }
    return OBC_ERR_CODE_SUCCESS;
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