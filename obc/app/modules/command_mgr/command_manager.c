#include "command_manager.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_id.h"
#include "command_callbacks.h"
#include "obc_scheduler_config.h"
#include "obc_errors.h"
#include "obc_time.h"
#include "obc_logging.h"
#include "obc_assert.h"
#include "alarm_handler.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define COMMAND_QUEUE_LENGTH 25UL
#define COMMAND_QUEUE_ITEM_SIZE sizeof(cmd_msg_t)

static QueueHandle_t commandQueueHandle;
static StaticQueue_t commandQueue;
static uint8_t commandQueueStack[COMMAND_QUEUE_LENGTH * COMMAND_QUEUE_ITEM_SIZE];

typedef struct {
  cmd_callback_t callback;
  cmd_policy_t policy;
  cmd_opt_t opts;  // Mask of command options
} cmd_info_t;

// CMD_POLICY_PROD is a subset of CMD_POLICY_RND. That is, if a command has PROD policy, it can also be run in RND.
static const cmd_info_t cmdsConfig[] = {
    [CMD_END_OF_FRAME] = {NULL, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_EXEC_OBC_RESET] = {execObcResetCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_RTC_SYNC] = {rtcSyncCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = {downlinkLogsNextPassCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_MICRO_SD_FORMAT] = {microSDFormatCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_UPLINK_DISC] = {uplinkDisconnectCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
};

#define CMDS_CONFIG_SIZE (sizeof(cmdsConfig) / sizeof(cmd_info_t))

STATIC_ASSERT(CMDS_CONFIG_SIZE <= UINT8_MAX, "Max command ID must be less than 256");

void obcTaskInitCommandMgr(void) {
  ASSERT((commandQueueStack != NULL) && (&commandQueue != NULL));
  if (commandQueueHandle == NULL) {
    commandQueueHandle =
        xQueueCreateStatic(COMMAND_QUEUE_LENGTH, COMMAND_QUEUE_ITEM_SIZE, commandQueueStack, &commandQueue);
  }
}

obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd) {
  if (commandQueueHandle == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(commandQueueHandle, (void *)cmd, portMAX_DELAY) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionCommandMgr(void *pvParameters) {
  obc_error_code_t errCode;

  // Used to track whether a safety-critical command is currently being executed
  // This is inefficient space-wise, but simplifies the code. We can optimize later if needed.
  static bool cmdProgressTracker[sizeof(cmdsConfig) / sizeof(cmd_info_t)] = {false};

  while (1) {
    cmd_msg_t cmd;
    if (xQueueReceive(commandQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
      // Check if the ID is a valid index
      if (cmd.id >= CMDS_CONFIG_SIZE) {
        LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CMD);
        continue;
      }

      cmd_info_t currCmdInfo = cmdsConfig[cmd.id];

      // Check if the ID has a callback
      if (currCmdInfo.callback == NULL) {
        LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_CMD);
        continue;
      }

      // Check if the command is allowed to be executed
      if (currCmdInfo.policy == CMD_POLICY_RND && OBC_ACTIVE_POLICY == CMD_POLICY_PROD) {
        LOG_ERROR_CODE(OBC_ERR_CODE_CMD_NOT_ALLOWED);
        continue;
      }

      // Check if the command is safety-critical
      if (currCmdInfo.opts & CMD_TYPE_CRITICAL) {
        // TODO: Make this persistent across resets
        if (!cmdProgressTracker[cmd.id]) {
          // Begin the two-step process of executing a safety-critical command
          LOG_DEBUG("Process started to execute safety-critical command");
          cmdProgressTracker[cmd.id] = true;
          continue;
        }

        // Reset the progress tracker
        cmdProgressTracker[cmd.id] = false;
      }

      // If the command is not time-tagged, execute it immediately
      if (!cmd.isTimeTagged) {
        // TODO: Handle safety-critical command failures
        LOG_IF_ERROR_CODE(currCmdInfo.callback(&cmd));
        continue;
      }

      // If the timetag is in the past, throw away the command
      if (cmd.timestamp < getCurrentUnixTime()) {
        continue;
      }

      alarm_handler_event_t alarm = {.id = ALARM_HANDLER_NEW_ALARM,
                                     .alarmInfo = {
                                         .unixTime = cmd.timestamp,
                                         .callbackDef =
                                             {
                                                 .cmdCallback = currCmdInfo.callback,
                                             },
                                         .type = ALARM_TYPE_TIME_TAGGED_CMD,
                                         .cmdMsg = cmd,
                                     }};

      LOG_IF_ERROR_CODE(sendToAlarmHandlerQueue(&alarm));
    }
  }
}
