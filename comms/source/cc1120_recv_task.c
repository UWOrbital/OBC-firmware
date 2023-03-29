#include "cc1120_decode.h"
#include "obc_logging.h"
#include "cc1120_recv_task.h"
#include "comms_manager.h"
#include "cc1120_txrx.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <sys_common.h>

#include <sys_common.h>
#include <gio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Recv task
static TaskHandle_t recvTaskHandle = NULL;
static StaticTask_t recvTaskBuffer;
static StackType_t recvTaskStack[RECV_STACK_SIZE];

// Recv Data Queue
static QueueHandle_t recvDataQueueHandle = NULL;
static StaticQueue_t recvDataQueue;
static uint8_t recvDataQueueStack[RECV_DATA_QUEUE_LENGTH*RECV_DATA_QUEUE_ITEM_SIZE];

static void vRecvTask(void * pvParameters);

void initRecvTask(void){
    ASSERT( (recvTaskStack != NULL) && (&recvTaskBuffer != NULL) );
    if(recvTaskHandle == NULL){
        recvTaskHandle = xTaskCreateStatic(vRecvTask, RECV_TASK_NAME, RECV_STACK_SIZE, NULL, RECV_PRIORITY, recvTaskStack, &recvTaskBuffer);
    }
    ASSERT( (recvDataQueueStack != NULL) && (&recvDataQueue != NULL) );
    if(recvDataQueueHandle == NULL){
        recvDataQueueHandle = xQueueCreateStatic(RECV_DATA_QUEUE_LENGTH, RECV_DATA_QUEUE_ITEM_SIZE, recvDataQueueStack, &recvDataQueue);
    }
}

/**
 * @brief waits for a COMMS_MANAGER_BEGIN_UPLINK_EVENT_ID event to be sent and starts RX and sends the packet to the decode data pipeline queue
 * 
 * @param pvParameters NULL
 * 
 * @return void
*/
static void vRecvTask(void * pvParameters){
    obc_error_code_t errCode;
    while (1) {
        comms_event_t queueMsg;
        if(xQueueReceive(recvDataQueueHandle, &queueMsg, RECV_DATA_QUEUE_RX_WAIT_PERIOD) != pdPASS){
            queueMsg.eventID = COMMS_MANAGER_NULL_EVENT_ID;
            LOG_ERROR_CODE(OBC_ERR_CODE_CC1120_RECV_QUEUE_TIMEOUT);
        }
        switch (queueMsg.eventID) {
            case COMMS_MANAGER_NULL_EVENT_ID:
                break;
            case COMMS_MANAGER_BEGIN_UPLINK_EVENT_ID:
                bool isStillUplinking = TRUE;
                while(isStillUplinking){
                    uint8_t recvData[RX_EXPECTED_PACKET_SIZE];
                    uint8_t recvDataLen = RX_EXPECTED_PACKET_SIZE;
                    cc1120Receive(recvData, recvDataLen);
                    LOG_IF_ERROR_CODE(sendToDecodeDataQueue(recvData));
                }
                break;
        }

    }
} 

obc_error_code_t sendToRecvDataQueue(comms_event_t *event){
    if (recvDataQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (event == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xQueueSend(recvDataQueueHandle, (void *) event, RECV_DATA_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}