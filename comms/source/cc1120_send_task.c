#include "ax25.h"
#include "reed_solomon.h"
#include "cc1120_txrx.h"
#include "comms_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

#define CC1120_TRANSMIT_STACK_SIZE 512U
#define CC1120_TRANSMIT_TASK_NAME "comms_transmit"
#define CC1120_TRANSMIT_TASK_PRIORITY tskIDLE_PRIORITY + 1U

#define ENCODE_PIPELINE_STACK_SIZE 1024U
#define ENCODE_PIPELINE_TASK_NAME "encode_pipeline"
#define ENCODE_PIPELINE_TASK_PRIORITY tskIDLE_PRIORITY + 1U

#define ENCODE_PIPELINE_QUEUE_LENGTH 10U
#define ENCODE_PIPELINE_QUEUE_ITEM_SIZE sizeof(comms_event_t)
#define ENCODE_PIPELINE_QUEUE_RX_WAIT_PERIOD portMAX_DELAY

#define CC1120_TRANSMIT_QUEUE_LENGTH 10U
#define CC1120_TRANSMIT_QUEUE_ITEM_SIZE CC1120_TX_RX_PKT_SIZE
#define CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD portMAX_DELAY

static TaskHandle_t cc1120TransmitTaskHandle = NULL;
static StaticTask_t cc1120TransmitTaskBuffer;
static StackType_t cc1120TransmitTaskStack[CC1120_TRANSMIT_STACK_SIZE];

static TaskHandle_t encodePipelineTaskHandle = NULL;
static StaticTask_t encodePipelineTaskBuffer;
static StackType_t encodePipelineTaskStack[ENCODE_PIPELINE_STACK_SIZE];

static QueueHandle_t cc1120TransmitQueueHandle = NULL;
static StaticQueue_t cc1120TransmitQueue = NULL;
static uint8_t cc1120TransmitQueueStack[CC1120_TRANSMIT_QUEUE_LENGTH*CC1120_TRANSMIT_QUEUE_ITEM_SIZE];

static QueueHandle_t encodePipelineQueueHandle = NULL;
static StaticQueue_t encodePipelineQueue = NULL;
static uint8_t encodePipelineQueueStack[ENCODE_PIPELINE_QUEUE_LENGTH*ENCODE_PIPELINE_QUEUE_ITEM_SIZE];

void vCC1120TransmitTask(void *pvParameters);
void vEncodePipelineTask(void *pvParameters);

/**
 * @brief Initialize the CC1120 transmit task and queue
 * 
 */
void initCC1120TransmitTask(void) {
    ASSERT( (cc1120TransmitTaskStack != NULL) && (&cc1120TransmitTaskBuffer != NULL) );

    if (cc1120TransmitTaskHandle == NULL) {
        cc1120TransmitTaskHandle = xTaskCreateStatic(vCC1120TransmitTask, CC1120_TRANSMIT_TASK_NAME, CC1120_TRANSMIT_STACK_SIZE,
                                                     NULL, CC1120_TRANSMIT_TASK_PRIORITY, cc1120TransmitTaskStack,
                                                     &cc1120TransmitTaskBuffer);
    }

    if (cc1120TransmitQueueHandle == NULL) {
        cc1120TransmitQueueHandle = xQueueCreateStatic(CC1120_TRANSMIT_QUEUE_LENGTH, CC1120_TRANSMIT_QUEUE_ITEM_SIZE,
                                                       cc1120TransmitQueueStack, &cc1120TransmitQueue);
    }
}

/**
 * @brief Initialize the comms encode pipeline task and queue
 * 
 */
void initEncodePipeline(void) {
    ASSERT( (encodePipelineTaskStack != NULL) && (&encodePipelineTaskBuffer != NULL) );

    if (encodePipelineTaskHandle == NULL) {
        encodePipelineTaskHandle = xTaskCreateStatic(vEncodePipelineTask, ENCODE_PIPELINE_TASK_NAME, ENCODE_PIPELINE_STACK_SIZE,
                                                     NULL, ENCODE_PIPELINE_TASK_PRIORITY, encodePipelineTaskStack,
                                                     &encodePipelineTaskBuffer);
    }

    if (encodePipelineQueueHandle == NULL) {
        encodePipelineQueueHandle = xQueueCreateStatic(ENCODE_PIPELINE_QUEUE_LENGTH, ENCODE_PIPELINE_QUEUE_ITEM_SIZE,
                                                       encodePipelineQueueStack, &encodePipelineQueue);
    }
}

/**
 * @brief Sends data from the CC1120 transmit queue into the CC1120 FIFO memory
 * 
 * @param pvParameters - NULL
 */
void vCC1120TransmitTask(void *pvParameters) {
    ASSERT( (cc1120TransmitQueueHandle != NULL) );
    obc_error_code_t errCode;

    while (1) {
        // Wait for CC1120 transmit queue
        uint8_t ax25_pkt[CC1120_TX_RX_PKT_SIZE];
        if (xQueueReceive(cc1120TransmitQueueHandle, &ax25_pkt, CC1120_TRANSMIT_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            // TODO: Restart the task by telling health manager
            LOG_ERROR_CODE(OBC_ERR_CODE_CC1120_TRANSMIT_QUEUE_TIMEOUT);
        }
        
        // Write to CC1120 FIFO
        cc1120Send(ax25_pkt, AX25_PKT_LEN);
    }
}

void sendTelemetry() {

}

/**
 * @brief Reads telemetry from the file system, applies Reed Solomon FEC, AX.25 framing,
 *        and sends into the CC1120 transmit queue
 * 
 * @param pvParameters - NULL
 */
void vEncodePipelineTask(void *pvParameters) {
    ASSERT( (encodePipelineQueueHandle != NULL) );

    while (1) {
        // Run when called by CDH (task notification)
        comms_event_t queueMsg;
        if (xQueueReceive(encodePipelineQueueHandle, &queueMsg, ENCODE_PIPELINE_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            queueMsg.eventID = COMMS_MANAGER_NULL_EVENT_ID;

        switch (queueMsg.eventID) {
            case COMMS_MANAGER_BEGIN_DOWNLINK_EVENT_ID:
                sendTelemetry();
                break;
            case COMMS_MANAGER_NULL_EVENT_ID:
                break;
        }
        // Read file, get telemetry and put it into rs_encode, 233B input
        
        
        // Put RS packet into AX.25 header

        // Send into a queue to write into CC1120 FIFO
    }
}
