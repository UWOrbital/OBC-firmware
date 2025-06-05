#include "command_manager.h"
#include "obc_gs_command_data.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_assert.h"
#include "command.h"

#include <FreeRTOS.h>
#include <sys_common.h>
#include <os_task.h>
#include <os_queue.h>

#define COMMAND_QUEUE_LENGTH 25UL
#define COMMAND_QUEUE_ITEM_SIZE sizeof(cmd_msg_t)

static QueueHandle_t commandQueueHandle;
static StaticQueue_t commandQueue;
static uint8_t commandQueueStack[COMMAND_QUEUE_LENGTH * COMMAND_QUEUE_ITEM_SIZE];

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
      cmd_info_t currCmdInfo;

      errCode = verifyCommand(&cmd, &currCmdInfo, cmdProgressTracker);

      if (errCode != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR_CODE(errCode);
        continue;
      }

      if (cmd.isTimeTagged) {
        LOG_IF_ERROR_CODE(processNonTimeTaggedCommand(&cmd, &currCmdInfo));
      } else {
        LOG_IF_ERROR_CODE(processTimeTaggedCommand(&cmd, &currCmdInfo));
      }
    }
  }
}
