#include "alarm_handler.h"
#include "command.h"
#include "command_manager.h"
#include "obc_general_util.h"
#include "obc_gs_aes128.h"
#include "obc_gs_ax25.h"
#include "obc_gs_command_data.h"
#include "obc_errors.h"
#include "obc_gs_commands_response.h"
#include "obc_gs_fec.h"
#include "obc_logging.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define COMMAND_QUEUE_LENGTH 25UL
#define COMMAND_QUEUE_ITEM_SIZE sizeof(cmd_msg_t)

static QueueHandle_t commandQueueHandle;
static StaticQueue_t commandQueue;
static uint8_t commandQueueStack[COMMAND_QUEUE_LENGTH * COMMAND_QUEUE_ITEM_SIZE];

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

obc_error_code_t processTimeTaggedCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo, uint8_t *responseData,
                                          uint8_t *responseDataLen) {
  if (cmd == NULL || currCmdInfo == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // If the timetag is in the past, throw away the command
  obc_error_code_t errCode;

  if (!cmd->isTimeTagged) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (cmd->timestamp < getCurrentUnixTime()) {
    return OBC_ERR_CODE_SUCCESS;
  }

  alarm_handler_event_t alarm = {.id = ALARM_HANDLER_NEW_ALARM,
                                 .alarmInfo = {
                                     .unixTime = cmd->timestamp,
                                     .callbackDef =
                                         {
                                             .cmdCallback = currCmdInfo->callback,
                                         },
                                     .type = ALARM_TYPE_TIME_TAGGED_CMD,
                                     .cmdMsg = *cmd,
                                 }};

  RETURN_IF_ERROR_CODE(sendToAlarmHandlerQueue(&alarm));
  return OBC_ERR_CODE_SUCCESS;
}

void obcTaskFunctionCommandMgr(void *pvParameters) {
  obc_error_code_t errCode = 0;
  uint8_t responseData[CMD_RESPONSE_DATA_MAX_SIZE] = {0};

  while (1) {
    cmd_msg_t cmd;
    cmd_response_header_t cmdResHeader = {0};
    uint8_t responseDataLen = 0;
    if (xQueueReceive(commandQueueHandle, &cmd, portMAX_DELAY) == pdPASS) {
      cmd_info_t currCmdInfo;

      LOG_IF_ERROR_CODE(verifyCommand(&cmd, &currCmdInfo));

      if (errCode != OBC_ERR_CODE_SUCCESS) {
        continue;
      }

      if (cmd.isTimeTagged) {
        LOG_IF_ERROR_CODE(processTimeTaggedCommand(&cmd, &currCmdInfo, responseData, &responseDataLen));
      } else {
        LOG_IF_ERROR_CODE(processNonTimeTaggedCommand(&cmd, &currCmdInfo, responseData, &responseDataLen));
      }

      cmdResHeader.cmdId = cmd.id;
      cmdResHeader.dataLen = responseDataLen;

      if (errCode == OBC_ERR_CODE_SUCCESS) {
        cmdResHeader.errCode = CMD_RESPONSE_SUCCESS;
      } else {
        cmdResHeader.errCode = CMD_RESPONSE_ERROR;
      }

      // TODO: Implement this in a downlink flow
      UNUSED(cmdResHeader);
      memset(responseData, 0, CMD_RESPONSE_DATA_MAX_SIZE);
    }
  }
}
