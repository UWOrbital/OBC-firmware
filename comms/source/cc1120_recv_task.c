#include "decode_telemetry.h"
#include "obc_logging.h"
#include "cc1120_recv_task.h"
#include "comms_manager.h"
#include "cc1120_txrx.h"
#include "ax25.h"
#include "obc_task_config.h"
#include "cc1120_spi.h"
#include "cc1120_defs.h"

#if COMMS_PHY == COMMS_PHY_UART
#include "obc_sci_io.h"
#endif

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
                #if COMMS_PHY == COMMS_PHY_UART
                uint8_t rxByte;

                // Read first byte
                LOG_IF_ERROR_CODE(sciReadBytes(&rxByte, 1, pdMS_TO_TICKS(1000)));

                if (errCode == OBC_ERR_CODE_SUCCESS) {
                    LOG_IF_ERROR_CODE(sendToDecodeDataQueue(&rxByte));
                }

                if (errCode == OBC_ERR_CODE_SUCCESS) {
                    // Read the rest of the bytes until we stop uplinking
                    while (1) {   
                        LOG_IF_ERROR_CODE(sciReadBytes(&rxByte, 1, pdMS_TO_TICKS(500)));

                        if (errCode == OBC_ERR_CODE_SUCCESS) {
                            LOG_IF_ERROR_CODE(sendToDecodeDataQueue(&rxByte));
                        }
                    }
                }
                #if CSDC_DEMO_ENABLED == 1
                LOG_IF_ERROR_CODE(startUplink());
                #endif
                #else
                if (xSemaphoreTake(cc1120Mutex, CC1120_MUTEX_TIMEOUT) != pdTRUE) {
                    LOG_ERROR_CODE(OBC_ERR_CODE_MUTEX_TIMEOUT);
                    break;
                }
                // switch cc1120 to receive mode and start receiving all the bytes for one continuous transmission
                LOG_IF_ERROR_CODE(cc1120Receive());
                LOG_IF_ERROR_CODE(cc1120StrobeSpi(CC1120_STROBE_SFSTXON));
                xSemaphoreGive(cc1120Mutex);
                #endif
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
