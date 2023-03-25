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

obc_error_code_t ax25Recv(uint8_t *in, uint8_t *out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

obc_error_code_t rsDecode(uint8_t *in, aes_block_t *out){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

obc_error_code_t aes128Decrypt(aes_block_t in, uint8_t *cmdBytes){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

obc_error_code_t tabulateCommands(uint8_t *cmdBytes, uint8_t *residualBytes){
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
    /* Fill in later */
    return errCode;
}

void initDecodeTask(void){
    memset(&decodeTaskBuffer, 0, sizeof(decodeTaskBuffer));
    memset(&decodeTaskStack, 0, sizeof(decodeTaskStack));

    memset(&decodeDataQueue, 0, sizeof(decodeDataQueue));
    memset(&decodeDataQueueStack, 0, sizeof(decodeDataQueueStack));

    ASSERT( (decodeTaskStack != NULL) && (&decodeTaskBuffer != NULL) );
    decodeTaskHandle = xTaskCreateStatic(vDecodeTask, DECODE_TASK_NAME, DECODE_STACK_SIZE, NULL, DECODE_PRIORITY, decodeTaskStack, &decodeTaskBuffer);

    ASSERT( (decodeDataQueueStack != NULL) && (&decodeDataQueue != NULL) );
    decodeDataQueueHandle = xQueueCreateStatic(DECODE_DATA_QUEUE_LENGTH, DECODE_DATA_QUEUE_ITEM_SIZE, decodeDataQueueStack, &decodeDataQueue);
}

static void vDecodeTask(void * pvParameters){
    while (1) {
        uint8_t data[278];
        if(xQueueReceive(decodeDataQueueHandle, data, DECODE_DATA_QUEUE_WAIT_PERIOD) == pdPASS){
            uint8_t axPacket[255];

            RETURN_IF_ERROR_CODE(ax25Recv(data, axPacket));

            aes_block_t aesBlocks[2];
            
            RETURN_IF_ERROR_CODE(rsDecode(data, aesBlocks[0]));

            RETURN_IF_ERROR_CODE(rsDecode(data+128, aesBlocks[1]));

            uint8_t commands[128];
            uint8_t residual[LARGEST_COMMAND_SIZE - 1];
            memset(residual, 0, sizeof(residual));

            RETURN_IF_ERROR_CODE(aes128Decrypt(aesBlocks[0], commands));

            RETURN_IF_ERROR_CODE(tabulateCommands(commands, residual));

            RETURN_IF_ERROR_CODE(aes128Decrypt(aesBlocks[1], commands));

            RETURN_IF_ERROR_CODE(tabulateCommands(commands, residual));
        }

    }
} 

obc_error_code_t SendToDecodeDataQueue(uint8_t *data) {
    if (decodeDataQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xQueueSend(decodeDataQueueHandle, (void *) data, DECODE_DATA_QUEUE_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}