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

typedef enum {
  CMD_SEQ_DISARM,
  CMD_SEQ_ARM,
  CMD_SEQ_EXEC,
} cmd_seq_t;

typedef struct {
  QueueHandle_t commandQueueHandle;
  StaticQueue_t commandQueue;
  uint8_t commandQueueStack[COMMAND_QUEUE_LENGTH * COMMAND_QUEUE_ITEM_SIZE];
  int8_t armedCommandId;
} command_manager_private_data_t;

static command_manager_private_data_t command_manager_privateData;

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
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};

#define CMDS_CONFIG_SIZE (sizeof(cmdsConfig) / sizeof(cmd_info_t))

STATIC_ASSERT(CMDS_CONFIG_SIZE <= UINT8_MAX, "Max command ID must be less than 256");

void obcTaskInitCommandMgr(void) {
  ASSERT((command_manager_privateData.commandQueueStack != NULL) &&
         (&command_manager_privateData.commandQueue != NULL));
  if (command_manager_privateData.commandQueueHandle == NULL) {
    command_manager_privateData.commandQueueHandle =
        xQueueCreateStatic(COMMAND_QUEUE_LENGTH, COMMAND_QUEUE_ITEM_SIZE, command_manager_privateData.commandQueueStack,
                           &command_manager_privateData.commandQueue);
  }
  command_manager_privateData.armedCommandId = -1;
}

obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd) {
  if (command_manager_privateData.commandQueueHandle == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(command_manager_privateData.commandQueueHandle, (void *)cmd, portMAX_DELAY) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionCommandMgr(void *pvParameters) {
  obc_error_code_t errCode;

  while (1) {
    cmd_msg_t cmd;
    if (xQueueReceive(command_manager_privateData.commandQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
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

      if (!cmd.isTimeTagged) {
        LOG_IF_ERROR_CODE(currCmdInfo.callback(&cmd));
        continue;
      }

      switch (cmd.seq) {  // add seq field to cmd_msg_t?
        case CMD_SEQ_ARM:
          if (currCmdInfo.callback != NULL) {
            command_manager_privateData.armedCommandId = cmd.id;
            LOG_IF_ERROR_CODE(currCmdInfo.callback(&cmd));
          }
          break;

        case CMD_SEQ_EXEC:
          if (currCmdInfo.callback != NULL && command_manager_privateData.armedCommandId == cmd.id) {
            command_manager_privateData.armedCommandId = -1;  // reset
            LOG_IF_ERROR_CODE(currCmdInfo.callback(&cmd));
          }
          break;

        case CMD_SEQ_DISARM:
          if (command_manager_privateData.armedCommandId == cmd.id) {
            command_manager_privateData.armedCommandId = -1;  // disarm
            LOG_IF_ERROR_CODE(currCmdInfo.callback(&cmd));
          }
          break;

        default:
          LOG_ERROR_CODE(OBC_ERR_CODE_INVALID_ARG);
          break;
      }

      // If the timetag is in the past, throw away the command
      if (cmd.timestamp < getCurrentUnixTime()) {
        continue;
      }

      alarm_handler_event_t alarm = {.id = ALARM_HANDLER_NEW_ALARM,
                                     .alarmInfo = {
                                         .unixTime = cmd.timestamp,
                                         .callbackDef = {.cmdCallback = currCmdInfo.callback},
                                         .type = ALARM_TYPE_TIME_TAGGED_CMD,
                                         .cmdMsg = cmd,
                                     }};

      LOG_IF_ERROR_CODE(sendToAlarmHandlerQueue(&alarm));
    }
  }
}
