#include "obc_gs_command_data.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_time.h"
#include "obc_time_utils.h"
#include "downlink_encoder.h"
#include "telemetry_manager.h"
#include "command.h"
#include "obc_general_util.h"

#include <redposix.h>
#include <stddef.h>

static obc_error_code_t execObcResetCmdCallback(cmd_msg_t* cmd, uint8_t* responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement safe reset (i.e. save state somewhere)
  LOG_DEBUG("Executing OBC reset command");
  resetSystem(RESET_REASON_CMD_EXEC_OBC_RESET);

  // We set the data here to indicate that the reset was unsuccessful
  memset(responseData, 0xFF, MAX_RESPONSE_BUFFER_SIZE);

  // Should never get here
  return OBC_ERR_CODE_UNKNOWN;
}

static obc_error_code_t rtcSyncCmdCallback(cmd_msg_t* cmd, uint8_t* responseData) {
  obc_error_code_t errCode;

  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  rtc_date_time_t dt;
  RETURN_IF_ERROR_CODE(unixToDatetime(cmd->rtcSync.unixTime, &dt));

  RETURN_IF_ERROR_CODE(setCurrentDateTimeRTC(&dt));

  RETURN_IF_ERROR_CODE(syncUnixTime());

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t downlinkLogsNextPassCmdCallback(cmd_msg_t* cmd, uint8_t* responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement handling for this command. Check if the log level is valid
  LOG_DEBUG("Executing log downlink command");
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t microSDFormatCmdCallback(cmd_msg_t* cmd, uint8_t* responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  int32_t ret = red_format("");
  if (ret != 0) {
    LOG_ERROR_CODE(OBC_ERR_CODE_FS_FORMAT_FAILED);
    return OBC_ERR_CODE_FS_FORMAT_FAILED;
  }

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t pingCmdCallback(cmd_msg_t* cmd, uint8_t* responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  responseData[0] = 0xFF;
  responseData[1] = 0xFF;

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t downlinkTelemCmdCallback(cmd_msg_t* cmd, uint8_t* responseData) {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(setTelemetryManagerDownlinkReady());

  return OBC_ERR_CODE_SUCCESS;
}

const cmd_info_t cmdsConfig[] = {
    [CMD_END_OF_FRAME] = {NULL, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_EXEC_OBC_RESET] = {execObcResetCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_RTC_SYNC] = {rtcSyncCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = {downlinkLogsNextPassCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_MICRO_SD_FORMAT] = {microSDFormatCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};

// This function is purely to trick the compiler into thinking we are using the cmdsConfig variable so we avoid the
// unused variable error
void unusedFunc() { UNUSED(cmdsConfig); }
