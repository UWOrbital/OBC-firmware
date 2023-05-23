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
#include "cc1120_txrx.h"

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

// decode data queue length should be double TXRX_INTERRUPT_THRESHOLD for safety to avoid cc1120 getting blocked
// can be reduced later depending on memory limitations
#define DECODE_DATA_QUEUE_LENGTH 2*TXRX_INTERRUPT_THRESHOLD
#define DECODE_DATA_QUEUE_ITEM_SIZE sizeof(uint8_t)
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
static obc_error_code_t decodePacket(packed_ax25_packet_t *data, packed_rs_packet_t *rsData, aes_data_t *aesData);

/**
 * @brief parses the completely decoded data and sends it to the command manager and detects end of transmission
 * 
 * @param cmdBytes 223B-AES_IV_SIZE array storing the completely decoded data
 * @param dataLen length of the data in cmdBytes
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
        if(cmdBytes[bytesUnpacked] == CMD_END_OF_FRAME){
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
    uint8_t byte;
    packed_ax25_packet_t axData;
    packed_rs_packet_t rsData;
    aes_data_t aesData;
    uint16_t axDataIndex = 0;
    bool startFlagReceived = false;
    while (1) {
        if(xQueueReceive(decodeDataQueueHandle, &byte, DECODE_DATA_QUEUE_RX_WAIT_PERIOD) == pdPASS){
            if(byte == AX25_FLAG){
                // if we have received a ax_25 flag and are past the first index, this means that this is the end flag
                // this ensures that we do not count consecutive ax_25 flags used when idling as start and end flags
                if(axDataIndex > 1){
                    axData.length = axDataIndex + 1;
                    LOG_IF_ERROR_CODE(decodePacket(&axData, &rsData, &aesData));
                    axDataIndex = 0;
                    startFlagReceived = false;
                }
                else{
                    startFlagReceived = true;
                    axData.data[0] = byte;
                    axDataIndex = 1;
                    continue;
                }
            }
            if(startFlagReceived) {
                axData.data[axDataIndex++] = byte;
            }
        }
    }
} 

/**
 * @brief completely decode a recieved packet
 * 
 * @param data - packed ax25 packet with received data
 * @param rsData - holds packed reed solomon data
 * @param aesData - pointer to an aes_data_t type, which holds the data to decrypt & the IV
 * @param decryptedData - holds the decrypted data from the aesBlock
 * 
 * @return obc_error_code_t - whether or not the data was completely decoded successfully 
*/
static obc_error_code_t decodePacket(packed_ax25_packet_t *data, packed_rs_packet_t *rsData, aes_data_t *aesData) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(ax25Recv(data, rsData));
    RETURN_IF_ERROR_CODE(rsDecode(rsData, aesData->rawData));
    uint8_t decryptedData[RS_DECODED_SIZE-AES_IV_SIZE];
    RETURN_IF_ERROR_CODE(aes128Decrypt(aesData, decryptedData));
    RETURN_IF_ERROR_CODE(handleCommands(decryptedData));
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief send a received packet to the decode data pipeline to be sent to command manager
 * 
 * @param data pointer to a single byte
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