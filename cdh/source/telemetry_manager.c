#include "telemetry_manager.h"
#include "telemetry_fs_utils.h"
#include "comms_manager.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_task_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <gio.h>

#include <redposix.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* Telemetry data queue config */
#define TELEMETRY_EVENT_QUEUE_LENGTH 128U
#define TELEMETRY_EVENT_QUEUE_ITEM_SIZE sizeof(telemetry_event_t)
#define TELEMETRY_EVENT_QUEUE_WAIT_PERIOD pdMS_TO_TICKS(1000)

#define STARTING_TELEMETRY_BATCH_ID 0UL

/**
 * @brief Telemetry Manager task.
 */
static void telemetryManager(void * pvParameters);

/**
 * @brief Check if it's time to downlink telemetry.
 * @return bool True if it's time to downlink telemetry, false otherwise 
 */
static bool checkDownlinkAlarm(void);

// Telemetry Manager Task
static TaskHandle_t telemetryTaskHandle = NULL;
static StaticTask_t telemetryTaskBuffer;
static StackType_t telemetryTaskStack[TELEMETRY_STACK_SIZE];

// Telemetry Data Queue
static QueueHandle_t telemetryEventQueueHandle = NULL;
static StaticQueue_t telemetryEventQueue;
static uint8_t telemetryEventQueueStack[TELEMETRY_EVENT_QUEUE_LENGTH*TELEMETRY_EVENT_QUEUE_ITEM_SIZE];

static uint32_t BatchId;

void initTelemetry(void) {
    memset(&telemetryTaskBuffer, 0, sizeof(telemetryTaskBuffer));
    memset(&telemetryTaskStack, 0, sizeof(telemetryTaskStack));

    memset(&telemetryEventQueue, 0, sizeof(telemetryEventQueue));
    memset(&telemetryEventQueueStack, 0, sizeof(telemetryEventQueueStack));

    ASSERT( (telemetryTaskStack != NULL) && (&telemetryTaskBuffer != NULL) );
    telemetryTaskHandle = xTaskCreateStatic(telemetryManager, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, telemetryTaskStack, &telemetryTaskBuffer);

    ASSERT( (telemetryEventQueueStack != NULL) && (&telemetryEventQueue != NULL) );
    telemetryEventQueueHandle = xQueueCreateStatic(TELEMETRY_EVENT_QUEUE_LENGTH, TELEMETRY_EVENT_QUEUE_ITEM_SIZE, telemetryEventQueueStack, &telemetryEventQueue);
}

static void telemetryManager(void * pvParameters) {
    obc_error_code_t errCode;

    // TODO: Get batch ID from the FRAM
    uint32_t telemetryBatchId = STARTING_TELEMETRY_BATCH_ID; 
    int32_t telemetryFileId = -1;

    // TODO: Deal with errors
    LOG_IF_ERROR_CODE(mkTelemetryDir());

    // TODO: Deal with errors
    LOG_IF_ERROR_CODE(createAndOpenTelemetryFileRW(telemetryBatchId, &telemetryFileId));
    BatchId = telemetryBatchId;

    while (1) {
        telemetry_event_t telemEvent;
        telemetry_event_id_t telemEventId;
        if (xQueueReceive(telemetryEventQueueHandle, &telemEvent, TELEMETRY_EVENT_QUEUE_WAIT_PERIOD) != pdPASS) {
            // TODO: Deal with errors
            continue;
        }
        
        switch(telemEvent.id){
            case DATA_EVENT:
                LOG_IF_ERROR_CODE(writeTelemetryToFile(telemetryFileId, telemEvent.data));
                // Important to close the file before sending it to the comms task
                LOG_IF_ERROR_CODE(closeTelemetryFile(telemetryFileId));
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    // TODO: Handle this error
                }
                // The lifetime of the CubeSat should not allow for this to overflow.
                // However, if it does, we can wrap around to 0 and start overwriting old files.
                telemetryBatchId++;
                
                // TODO: Save batch ID to FRAM

                LOG_IF_ERROR_CODE(createAndOpenTelemetryFileRW(telemetryBatchId, &telemetryFileId));
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    // TODO: Deal with errors
                }
                break;
            case DOWNLINK_EVENT:
                // Send downlink event when event is received by telemetry manager
                comms_event_t downlinkEvent = {.eventID = DOWNLINK_TELEMETRY, .telemetryBatchId = BatchId};
                LOG_IF_ERROR_CODE(sendToCommsQueue(&downlinkEvent));
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    // TODO: Handle this error, specifically if the queue is full. Other
                    // errors should be caught during testing.
                }
                break;
            default:
                // TODO: Handle unknown events
        }
        
        // // Check if we need to downlink telemetry
        // // TODO: This is a mock implementation. We need to implement a proper alarm system
        // if (!checkDownlinkAlarm()) {
        //     continue;
        // }
    }
}

obc_error_code_t addTelemetryData(telemetry_data_t *data) {
    telemetry_event_t telemEvent = {0};
    if (telemetryEventQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    telemEvent.id = DATA_EVENT;
    memcpy(&telemEvent.data, data, sizeof(telemetry_data_t));

    if (xQueueSend(telemetryEventQueueHandle, &telemEvent, TELEMETRY_EVENT_QUEUE_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static bool checkDownlinkAlarm(void) {
    // TODO: Check if it's time to downlink telemetry data. This is currently a mock implementation
    static uint32_t downlinkCounter = 0;
    downlinkCounter++;

    if (downlinkCounter % 10 != 0) {
        return false;
    }

    return true;
}

obc_error_code_t sendDownlinkTelemetryEvent(void) {
    telemetry_event_t downlinkEvent = {0};
    downlinkEvent.id = DOWNLINK_EVENT;
    if(xQueueSend(telemetryEventQueueHandle, &downlinkEvent, TELEMETRY_EVENT_QUEUE_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }

    return OBC_ERR_CODE_UNKNOWN;
}
