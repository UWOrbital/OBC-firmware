#include "command_data.h"
#include "command_id.h"
#include "command_callbacks.h"
#include "obc_task_config.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define COMMAND_QUEUE_LENGTH    25
#define COMMAND_QUEUE_ITEM_SIZE sizeof(cmd_msg_t)

static TaskHandle_t cmdManagerTaskHandle;
static StaticTask_t cmdManagerTaskBuffer;
static StackType_t cmdManagerTaskStack[CMD_MANAGER_STACK_SIZE];

static QueueHandle_t commandQueueHandle;
static StaticQueue_t commandQueue;
static uint8_t commandQueueStack[COMMAND_QUEUE_LENGTH*COMMAND_QUEUE_ITEM_SIZE];

static const cmd_callback_t cmdCallbacks[] = {
    [CMD_EXEC_OBC_RESET] = execObcResetCmdCallback,
    [CMD_RTC_SYNC] = rtcSyncCmdCallback,
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = downlinkLogsNextPassCmdCallback
};

static const size_t cmdArraySize = sizeof(cmdCallbacks) / sizeof(cmd_callback_t);

STATIC_ASSERT(sizeof(cmdCallbacks)/sizeof(cmd_callback_t) <= UINT8_MAX, "Max command ID must be less than 256");

/**
 * @brief Task that manages the command queue and executes commands
 */
static void commandManagerTask(void *pvParameters);

void initCommandManager(void) {
    ASSERT( (cmdManagerTaskStack != NULL) && (&cmdManagerTaskBuffer != NULL) );
    if (cmdManagerTaskHandle == NULL) {
        cmdManagerTaskHandle = xTaskCreateStatic(
            commandManagerTask,
            CMD_MANAGER_NAME,
            CMD_MANAGER_STACK_SIZE,
            NULL,
            CMD_MANAGER_PRIORITY,
            cmdManagerTaskStack,
            &cmdManagerTaskBuffer
        );
    }

    ASSERT( (commandQueueStack != NULL) && (&commandQueue != NULL) );
    if (commandQueueHandle == NULL) {
        commandQueueHandle = xQueueCreateStatic(
            COMMAND_QUEUE_LENGTH,
            COMMAND_QUEUE_ITEM_SIZE,
            commandQueueStack,
            &commandQueue
        );
    }
}

obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd) {
    if (commandQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    if (xQueueSend(commandQueueHandle, (void *) cmd, portMAX_DELAY) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static uint32_t getCurrentTime(void) {
    // Mock for testing; replace with actual implementation when available
    return 0;
}

static void commandManagerTask(void *pvParameters) {
    obc_error_code_t errCode;

    while (1) {
        cmd_msg_t cmd;
        if (xQueueReceive(commandQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
            LOG_DEBUG("Received command %u", cmd.id);

            // Check if the ID is a valid index
            if (cmd.id >= cmdArraySize) {
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CMD);
                continue;
            }

            // Check if the ID has a callback
            if (cmdCallbacks[cmd.id] == NULL) {
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CMD);
                continue;
            }

            // If the command is not time-tagged, execute it immediately
            if (!cmd.isTimeTagged) {
                // TODO: Deal with error code
                LOG_IF_ERROR_CODE(cmdCallbacks[cmd.id](&cmd));
                continue;
            }

            // If the timetag is in the past, throw away the command
            if (cmd.timestamp < getCurrentTime()) {
                continue;
            }

            // TODO: Handle time-tagged commands

        }
    }
}
