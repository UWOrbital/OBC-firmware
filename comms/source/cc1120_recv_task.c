#include "cc1120_decode.h"
#include "obc_logging.h"
#include "cc1120_decode.h"
#include "comms_manager.h"

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
static TaskHandle_t recvTaskHandle = NULL;
static StaticTask_t recvTaskBuffer;
static StackType_t recvTaskStack[RECV_STACK_SIZE];

// Recv Data Queue
static QueueHandle_t recvDataQueueHandle = NULL;
static StaticQueue_t recvDataQueue;
static uint8_t recvDataQueueStack[RECV_DATA_QUEUE_LENGTH*RECV_DATA_QUEUE_ITEM_SIZE];

static void vRecvTask(void * pvParameters);

void initRecvTask(void){
    memset(&recvTaskBuffer, 0, sizeof(recvTaskBuffer));
    memset(&recvTaskStack, 0, sizeof(recvTaskStack));

    memset(&recvDataQueue, 0, sizeof(recvDataQueue));
    memset(&recvDataQueueStack, 0, sizeof(recvDataQueueStack));

    ASSERT( (recvTaskStack != NULL) && (&recvTaskBuffer != NULL) );
    recvTaskHandle = xTaskCreateStatic(vRecvTask, RECV_TASK_NAME, RECV_STACK_SIZE, NULL, RECV_PRIORITY, recvTaskStack, &recvTaskBuffer);

    ASSERT( (recvDataQueueStack != NULL) && (&recvDataQueue != NULL) );
    recvDataQueueHandle = xQueueCreateStatic(RECV_DATA_QUEUE_LENGTH, RECV_DATA_QUEUE_ITEM_SIZE, recvDataQueueStack, &recvDataQueue);
}

static void vRecvTask(void * pvParameters){
    while (1) {
        comms_event_t queueMsg;
        if(xQueueReceive(recvDataQueueHandle, &queueMsg, RECV_DATA_QUEUE_WAIT_PERIOD) != pdPASS){
            queueMsg.eventID = COMMS_MANAGER_NULL_EVENT_ID;
            LOG_ERROR_CODE(CC1120_ERROR_CODE_RECV_QUEUE_TIMEOUT);
        }
        switch (queueMsg.eventID) {
            case COMMS_MANAGER_NULL_EVENT_ID:
                break;
            case COMMS_MANAGER_BEGIN_UPLINK_EVENT_ID:
                uint8_t recvData[RX_EXPECTED_PACKET_SIZE];
                uint8_t recvDataLen = RX_EXPECTED_PACKET_SIZE;
                cc1120_receive(recvData, recvDataLen);
                if(xQueueSend(getDecodeQueueHandle(), recvData, DECODE_DATA_QUEUE_TX_WAIT_PERIOD) != pdPASS){
                    LOG_ERROR_CODE(CC1120_ERROR_CODE_RECV_QUEUE_TIMEOUT);
                }
                break;
        }

    }
} 

obc_error_code_t SendToRecvDataQueue(uint8_t *data) {
    if (recvDataQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xQueueSend(recvDataQueueHandle, (void *) data, RECV_DATA_QUEUE_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}