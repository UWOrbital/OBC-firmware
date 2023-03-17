#include "telemetry.h"
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

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE   1024U
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY     1U
#define TELEMETRY_DELAY_TICKS  pdMS_TO_TICKS(1000)

/* Telemetry data queue config */
#define TELEMETRY_DATA_QUEUE_LENGTH 128U
#define TELEMETRY_DATA_QUEUE_ITEM_SIZE sizeof(telemetry_data_t)
#define TELEMETRY_DATA_QUEUE_WAIT_PERIOD pdMS_TO_TICKS(1000)

STATIC_ASSERT(sizeof(telemetry_data_t) <= INT32_MAX, "Telemetry data size is too large");

static void vTelemetryTask(void * pvParameters);
static obc_error_code_t writeTelemetryToFile(int32_t telFileId, telemetry_data_t telemetryData);
static obc_error_code_t openTelemetryFile(uint32_t telemBatchId, int32_t *telemFileId);
static obc_error_code_t closeTelemetryFile(int32_t telemFileId);

static bool checkDownlinkAlarm(void);

static TaskHandle_t telemetryTaskHandle = NULL;
static StaticTask_t telemetryTaskBuffer;
static StackType_t telemetryTaskStack[TELEMETRY_STACK_SIZE];

// Telemetry Data Queue
static QueueHandle_t telemetryDataQueueHandle = NULL;
static StaticQueue_t telemetryDataQueue;
static uint8_t telemetryDataQueueStack[TELEMETRY_DATA_QUEUE_LENGTH*TELEMETRY_DATA_QUEUE_ITEM_SIZE];

// Current telemetry file ID
static int32_t telemetryFileId;
static uint32_t telemetryBatchId;

void initTelemetry(void) {
    memset(&telemetryTaskBuffer, 0, sizeof(telemetryTaskBuffer));
    memset(&telemetryTaskStack, 0, sizeof(telemetryTaskStack));

    memset(&telemetryDataQueue, 0, sizeof(telemetryDataQueue));
    memset(&telemetryDataQueueStack, 0, sizeof(telemetryDataQueueStack));

    memset(&telemetryFileId, 0, sizeof(telemetryFileId));
    memset(&telemetryBatchId, 0, sizeof(telemetryBatchId));

    ASSERT( (telemetryTaskStack != NULL) && (&telemetryTaskBuffer != NULL) );
    telemetryTaskHandle = xTaskCreateStatic(vTelemetryTask, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, telemetryTaskStack, &telemetryTaskBuffer);

    ASSERT( (telemetryDataQueueStack != NULL) && (&telemetryDataQueue != NULL) );
    telemetryDataQueueHandle = xQueueCreateStatic(TELEMETRY_DATA_QUEUE_LENGTH, TELEMETRY_DATA_QUEUE_ITEM_SIZE, telemetryDataQueueStack, &telemetryDataQueue);
}

obc_error_code_t addTelemetryData(telemetry_data_t *data) {
    if (telemetryDataQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (data == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xQueueSend(telemetryDataQueueHandle, (void *) data, TELEMETRY_DATA_QUEUE_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

obc_error_code_t getTelemetryFileName(uint32_t telemBatchId, char *buff, size_t buffSize) {
    if (buff == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (buffSize < TELEMETRY_FILE_PATH_MAX_LENGTH) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    int ret = snprintf(buff, buffSize, "%s%s%lu%s", TELEMETRY_FILE_DIRECTORY, TELEMETRY_FILE_PREFIX, telemBatchId, TELEMETRY_FILE_EXTENSION);
    if (ret < 0) {
        return OBC_ERR_CODE_INVALID_FILE_NAME;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getNextTelemetry(int32_t telemFileId, telemetry_data_t *telemData) {
    // Assume file is open and valid
    
    if (telemData == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (telemFileId < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    int32_t bytesRead = red_read(telemFileId, (void *) telemData, sizeof(telemetry_data_t));

    if (bytesRead == 0) {
        return OBC_ERR_CODE_REACHED_EOF;
    }

    // Since we only write the telemetry data struct, the number of bytes in the file
    // should be a multiple of the size of the struct. TODO: Deal with incomplete writes
    if (bytesRead != sizeof(telemetry_data_t)) {
        return OBC_ERR_CODE_FAILED_FILE_READ;
    }

    return OBC_ERR_CODE_SUCCESS;
}

static void vTelemetryTask(void * pvParameters) {
    obc_error_code_t errCode;

    LOG_IF_ERROR_CODE(openTelemetryFile(telemetryBatchId, &telemetryFileId));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        // TODO: Handle this error
    }

    while (1) {
        telemetry_data_t telemData;
        if (xQueueReceive(telemetryDataQueueHandle, &telemData, TELEMETRY_DATA_QUEUE_WAIT_PERIOD) == pdPASS) {
            writeTelemetryToFile(telemetryFileId, telemData);
        }

        // Check if we need to downlink telemetry
        if (!checkDownlinkAlarm()) {
            continue;
        }

        // Important to close the file before sending it to the comms task
        LOG_IF_ERROR_CODE(closeTelemetryFile(telemetryFileId));
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            // TODO: Handle this error
        }

        comms_event_t downlinkEvent = {.eventID = DOWNLINK_TELEMETRY, .telemetryBatchId = telemetryBatchId};

        LOG_IF_ERROR_CODE(sendToCommsQueue(&downlinkEvent));
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            // TODO: Handle this error, specifically if the queue is full. Other
            // errors should be caught during testing.
        }

        // The lifetime of the CubeSat should not allow for this to overflow.
        // However, if it does, it'd be at a point where we can overwrite the
        // old telemetry files, so it's not a concern.
        if (telemetryBatchId == UINT32_MAX) {
            LOG_ERROR_CODE(OBC_ERR_CODE_BUFF_OVERFLOW);
            telemetryBatchId = 0;
        } else {
            telemetryBatchId++;
        }

        LOG_IF_ERROR_CODE(openTelemetryFile(telemetryBatchId, &telemetryFileId));
        if (errCode != OBC_ERR_CODE_SUCCESS) {
            // TODO: Handle this error. Testing will ensure most possible errors won't
            // occur in orbit, but file system errors could occur.
        }
    }
}

static obc_error_code_t writeTelemetryToFile(int32_t telFileId, telemetry_data_t telemetryData) {
    // Assume file is open and valid

    if (telFileId < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // TODO: Handle power resets during read/write (i.e partial data)
    // Idea: Add header and footer to each file, and check for them on read (investigate checksum feasability)
    int32_t ret = red_write(telFileId, &telemetryData, sizeof(telemetry_data_t));

    if(ret == sizeof(telemetry_data_t)) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    if (red_errno == RED_EFBIG) {
        // TODO: Handle this error; probably need to close the file and open a new one
        return OBC_ERR_CODE_MAX_FILE_SIZE_REACHED;
    }

    return OBC_ERR_CODE_FAILED_FILE_WRITE;
}

static obc_error_code_t openTelemetryFile(uint32_t telemBatchId, int32_t *telemFileId) {
    obc_error_code_t errCode;

    if (telemFileId == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    unsigned char telemFilePathBuffer[TELEMETRY_FILE_PATH_MAX_LENGTH] = {'\0'};
    RETURN_IF_ERROR_CODE(getTelemetryFileName(telemBatchId, (char *)telemFilePathBuffer, TELEMETRY_FILE_PATH_MAX_LENGTH));

    int32_t telFile = red_open((const char *)telemFilePathBuffer, RED_O_RDWR | RED_O_CREAT);
    if (telFile < 0) {
        return OBC_ERR_CODE_FAILED_FILE_OPEN;
    }

    *telemFileId = telFile;

    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t closeTelemetryFile(int32_t telemFileId) {
    if (telemFileId < 0) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    int32_t ret = red_close(telemFileId);
    if (ret < 0) {
        return OBC_ERR_CODE_FAILED_FILE_CLOSE;
    }

    return OBC_ERR_CODE_SUCCESS;
}

static bool checkDownlinkAlarm(void) {
    // TODO: Check if it's time to downlink telemetry data
    return false;
}
