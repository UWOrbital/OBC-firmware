#include "command.h"
#include "obc_logging.h"
#include "alarm_handler.h"

#define CMDS_CONFIG_SIZE (sizeof(cmdsConfig) / sizeof(cmd_info_t))

obc_error_code_t verifyCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo, bool cmdProgressTracker[]) {
  if (cmd->id >= CMDS_CONFIG_SIZE) {
    return OBC_ERR_CODE_UNSUPPORTED_CMD;
  }

  *currCmdInfo = cmdsConfig[cmd->id];
  // Check if the ID has a callback
  if (currCmdInfo->callback == NULL) {
    return OBC_ERR_CODE_UNSUPPORTED_CMD;
  }

  // Check if the command is allowed to be executed
  if (currCmdInfo->policy == CMD_POLICY_RND && OBC_ACTIVE_POLICY == CMD_POLICY_PROD) {
    return OBC_ERR_CODE_CMD_NOT_ALLOWED;
  }

  // Check if the command is safety-critical
  if (currCmdInfo->opts & CMD_TYPE_CRITICAL) {
    // TODO: Make this persistent across resets
    if (!cmdProgressTracker[cmd->id]) {
      // Begin the two-step process of executing a safety-critical command
      LOG_DEBUG("Process started to execute safety-critical command");
      cmdProgressTracker[cmd->id] = true;
      return OBC_ERR_CODE_SUCCESS;
    }

    // Reset the progress tracker
    cmdProgressTracker[cmd->id] = false;
  }

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t processNonTimeTaggedCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo) {
  // If the command is not time-tagged, execute it immediately
  obc_error_code_t errCode;
  if (cmd->isTimeTagged) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // TODO: Handle safety-critical command failures
  RETURN_IF_ERROR_CODE(currCmdInfo->callback(cmd));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t processTimeTaggedCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo) {
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
