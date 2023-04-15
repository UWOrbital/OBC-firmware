#include "comms_manager.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "telemetry_manager.h"
#include "obc_fs_utils.h"
#include "telemetry_fs_utils.h"
#include "telemetry_pack.h"
#include "obc_task_config.h"
#include "send_telemetry.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <redposix.h>

#include <sys_common.h>
#include <gio.h>

/* Comms Manager event queue config */
#define COMMS_MANAGER_QUEUE_LENGTH 10U
#define COMMS_MANAGER_QUEUE_ITEM_SIZE sizeof(comms_event_t)
#define COMMS_MANAGER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define COMMS_MANAGER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

static TaskHandle_t commsTaskHandle = NULL;
static StaticTask_t commsTaskBuffer;
static StackType_t commsTaskStack[COMMS_MANAGER_STACK_SIZE];

static QueueHandle_t commsQueueHandle = NULL;
static StaticQueue_t commsQueue;
static uint8_t commsQueueStack[COMMS_MANAGER_QUEUE_LENGTH*COMMS_MANAGER_QUEUE_ITEM_SIZE];

/**
 * @brief	Comms Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vCommsManagerTask(void * pvParameters);

void initCommsManager(void) {
    ASSERT( (commsTaskStack != NULL) && (&commsTaskBuffer != NULL) );
    if (commsTaskHandle == NULL) {
        commsTaskHandle = xTaskCreateStatic(vCommsManagerTask, COMMS_MANAGER_NAME, COMMS_MANAGER_STACK_SIZE, NULL, COMMS_MANAGER_PRIORITY, commsTaskStack, &commsTaskBuffer);
    }

    ASSERT( (commsQueueStack != NULL) && (&commsQueue != NULL) );
    if (commsQueueHandle == NULL) {
        commsQueueHandle = xQueueCreateStatic(COMMS_MANAGER_QUEUE_LENGTH, COMMS_MANAGER_QUEUE_ITEM_SIZE, commsQueueStack, &commsQueue);
    }
}

obc_error_code_t sendToCommsQueue(comms_event_t *event) {
    ASSERT(commsQueueHandle != NULL);
    
    if (event == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    if (xQueueSend(commsQueueHandle, (void *) event, COMMS_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

// Example function to show how to handle telemetry data
// This isn't meant to be the final implementation
static obc_error_code_t handleTelemetry(uint32_t telemetryBatchId) {
    obc_error_code_t errCode;

    // Open telemetry file
    int32_t fd;
    RETURN_IF_ERROR_CODE(openTelemetryFileRO(telemetryBatchId, &fd));

    size_t fileSize;
    RETURN_IF_ERROR_CODE(getFileSize(fd, &fileSize));
    LOG_INFO("Telemetry file size: %lu", fileSize);

    // Print telemetry file name
    char fileName[TELEMETRY_FILE_PATH_MAX_LENGTH] = {0};
    RETURN_IF_ERROR_CODE(constructTelemetryFilePath(telemetryBatchId, fileName, TELEMETRY_FILE_PATH_MAX_LENGTH));
    LOG_INFO("Telemetry file name: %s", fileName);
    
    // Read 1 telemetry data point
    telemetry_data_t telemetryData;
    while ((errCode = readNextTelemetryFromFile(fd, &telemetryData)) == OBC_ERR_CODE_SUCCESS) {
        LOG_DEBUG("Sending telemetry with ID %u", telemetryData.id);

        uint8_t telemParamBuf[MAX_TELEMETRY_DATA_SIZE] = {0};
        size_t telemSize = 0;
        LOG_IF_ERROR_CODE(packTelemetry(&telemetryData, telemParamBuf, MAX_TELEMETRY_DATA_SIZE, &telemSize));
        LOG_DEBUG("Packed telemetry size: %u", telemSize);
        for (size_t i = 0; i < telemSize; i++) {
            LOG_DEBUG("Packed telemetry data byte %lu: %u", i, telemParamBuf[i]);
        }
    }

    if (errCode == OBC_ERR_CODE_REACHED_EOF) {
        LOG_DEBUG("Reached end of telemetry file");
        errCode = OBC_ERR_CODE_SUCCESS;
    }

    LOG_IF_ERROR_CODE(errCode);

    // Close telemetry file
    RETURN_IF_ERROR_CODE(closeTelemetryFile(fd));
    return OBC_ERR_CODE_SUCCESS;
}

static void vCommsManagerTask(void * pvParameters) {
    obc_error_code_t errCode;
    
    while (1) {
        comms_event_t queueMsg;
        
        if (xQueueReceive(commsQueueHandle, &queueMsg, COMMS_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            continue;
        }
        
        switch (queueMsg.eventID) {
            case DOWNLINK_TELEMETRY:
                sendToTelemEncodeQueue(&queueMsg.telemetryBatchId);
                break;
        }
    }
}
