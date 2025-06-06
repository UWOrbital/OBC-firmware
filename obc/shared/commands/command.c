#include "command.h"
#include "obc_logging.h"

extern const cmd_info_t cmdConfig[];

#ifdef IS_BL
static const cmd_info_t cmdsConfig[] = {[CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};
#else
static const cmd_info_t cmdsConfig[] = {
    [CMD_END_OF_FRAME] = {NULL, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_EXEC_OBC_RESET] = {execObcResetCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_RTC_SYNC] = {rtcSyncCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = {downlinkLogsNextPassCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_MICRO_SD_FORMAT] = {microSDFormatCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};
#endif

#define CMDS_CONFIG_SIZE (sizeof(cmdsConfig) / sizeof(cmd_info_t))

// Used to track whether a safety-critical command is currently being executed
// This is inefficient space-wise, but simplifies the code. We can optimize later if needed.
static bool cmdProgressTracker[sizeof(cmdsConfig) / sizeof(cmd_info_t)] = {false};

obc_error_code_t verifyCommand(cmd_msg_t *cmd, cmd_info_t *currCmdInfo) {
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
