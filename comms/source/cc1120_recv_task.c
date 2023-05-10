#include "decode_telemetry.h"
#include "obc_logging.h"
#include "cc1120_recv_task.h"
#include "comms_manager.h"
#include "cc1120_txrx.h"
#include "ax25.h"
#include "obc_task_config.h"

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

#define RECV_DATA_QUEUE_LENGTH 10U 
#define RECV_DATA_QUEUE_ITEM_SIZE sizeof(comms_event_t)
#define RECV_DATA_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define RECV_DATA_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

// Recv task
static TaskHandle_t recvTaskHandle = NULL;
static StaticTask_t recvTaskBuffer;
static StackType_t recvTaskStack[COMMS_RECV_STACK_SIZE];

// Recv Data Queue
static QueueHandle_t recvDataQueueHandle = NULL;
static StaticQueue_t recvDataQueue;
static uint8_t recvDataQueueStack[RECV_DATA_QUEUE_LENGTH*RECV_DATA_QUEUE_ITEM_SIZE];

static void vRecvTask(void * pvParameters);

void initRecvTask(void){
    ASSERT( (recvTaskStack != NULL) && (&recvTaskBuffer != NULL) );
    if(recvTaskHandle == NULL){
        recvTaskHandle = xTaskCreateStatic(vRecvTask, COMMS_RECV_TASK_NAME, COMMS_RECV_STACK_SIZE, NULL, COMMS_RECV_PRIORITY, recvTaskStack, &recvTaskBuffer);
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
            continue;
        }
        switch (queueMsg.eventID) {
            case BEGIN_UPLINK:
                isStillUplinking = true;
                // Keep receiving packets until isStillUplinking is set to FALSE once we receive an end of transmission command
                while(isStillUplinking){
                    packed_ax25_packet_t recvData;
                    uint32_t recvDataLen = AX25_PKT_LEN;
                    // Receive AX25_PKT_LEN bytes from ground station and store them in recvData.data
                    LOG_IF_ERROR_CODE(cc1120Receive(recvData.data, recvDataLen));
                    // Send the received bytes to decode data queue to be decoded and sent to command manager
                    if(errCode == OBC_ERR_CODE_SUCCESS){
                        LOG_IF_ERROR_CODE(sendToDecodeDataQueue(&recvData));
                    }
                }
                break;
            default:
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_EVENT);
        }

    }
} 

obc_error_code_t startUplink(void){
    if (recvDataQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }
    comms_event_t event;
    event.eventID = BEGIN_UPLINK;
    if (xQueueSend(recvDataQueueHandle, (void *) &event, RECV_DATA_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_QUEUE_FULL;
}
