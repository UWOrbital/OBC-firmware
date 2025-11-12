#include "obc_gs_command_data.h"
#include "obc_gs_command_id.h"
#include "obc_i2c_io.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_time.h"
#include "obc_time_utils.h"
#include "downlink_encoder.h"
#include "os_portmacro.h"
#include "os_projdefs.h"
#include "telemetry_manager.h"
#include "command.h"
#include "obc_general_util.h"

#include <redposix.h>
#include <stddef.h>
#include <stdint.h>

static obc_error_code_t execObcResetCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement safe reset (i.e. save state somewhere)
  LOG_DEBUG("Executing OBC reset command");
  resetSystem(RESET_REASON_CMD_EXEC_OBC_RESET);

  // Should never get here
  return OBC_ERR_CODE_UNKNOWN;
}

static obc_error_code_t rtcSyncCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  obc_error_code_t errCode;

  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint32_t currentUnixTime = getCurrentUnixTime();
  memcpy(responseData, &currentUnixTime, sizeof(currentUnixTime));
  *responseDataLen = sizeof(currentUnixTime);

  rtc_date_time_t dt;
  RETURN_IF_ERROR_CODE(unixToDatetime(cmd->rtcSync.unixTime, &dt));
  RETURN_IF_ERROR_CODE(setCurrentDateTimeRTC(&dt));
  RETURN_IF_ERROR_CODE(syncUnixTime());

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t downlinkLogsNextPassCmdCallback(cmd_msg_t *cmd, uint8_t *responseData,
                                                        uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement handling for this command. Check if the log level is valid
  LOG_DEBUG("Executing log downlink command");
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t microSDFormatCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  int32_t ret = red_format("");
  if (ret != 0) {
    LOG_ERROR_CODE(OBC_ERR_CODE_FS_FORMAT_FAILED);
    return OBC_ERR_CODE_FS_FORMAT_FAILED;
  }

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t pingCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  responseData[0] = 0xFF;
  responseData[1] = 0xFF;
  *responseDataLen = 2;

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t downlinkTelemCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  obc_error_code_t errCode;

  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  RETURN_IF_ERROR_CODE(setTelemetryManagerDownlinkReady());

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t I2CProbeCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t messageData[] = {0xff, 0x00, 0xff};
  uint8_t validAddresses = 0;
  for (uint8_t i = 0; i < 128; i++) {
    if (i2cSendTo(i, 3, messageData, pdMS_TO_TICKS(10), portMAX_DELAY) == OBC_ERR_CODE_SUCCESS) {
      responseData[validAddresses] = i;
      validAddresses++;
    }
  }

  *responseDataLen = validAddresses;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t armCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // Parsing the cmdArm data into bytes (Little Endian)
  uint8_t b0 = cmd->cmdArm.cmdArmData <<= 24;
  b0 >>= 24;

  uint8_t b1 = cmd->cmdArm.cmdArmData <<= 16;
  b1 >>= 24;

  uint8_t b2 = cmd->cmdArm.cmdArmData <<= 8;
  b2 >>= 24;

  uint8_t b3 = cmd->cmdArm.cmdArmData;
  b3 >>= 24;

  // Set the first four bytes to the command data
  responseData[0] = b0;
  responseData[1] = b1;
  responseData[2] = b2;
  responseData[3] = b3;

  // Parse the armId data into bytes (Little Endian)
  b0 = cmd->cmdArm.armIdData <<= 24;
  b0 >>= 24;

  b1 = cmd->cmdArm.armIdData <<= 16;
  b1 >>= 24;

  b2 = cmd->cmdArm.armIdData <<= 8;
  b2 >>= 24;

  b3 = cmd->cmdArm.armIdData;
  b3 >>= 24;

  // Parsing the armId data into bytes
  responseData[4] = b0;
  responseData[5] = b1;
  responseData[6] = b2;
  responseData[7] = b3;

  *responseDataLen = 8;

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t executeCmdCallback(cmd_msg_t *cmd, uint8_t *responseData, uint8_t *responseDataLen) {
  if (cmd == NULL || responseData == NULL || responseDataLen == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // Parsing the cmdExecute data into bytes (Little Endian)
  uint8_t b0 = cmd->cmdExecute.cmdExecuteData <<= 24;
  b0 >>= 24;

  uint8_t b1 = cmd->cmdExecute.cmdExecuteData <<= 16;
  b1 >>= 24;

  uint8_t b2 = cmd->cmdExecute.cmdExecuteData <<= 8;
  b2 >>= 24;

  uint8_t b3 = cmd->cmdExecute.cmdExecuteData;
  b3 >>= 24;

  // First four bytes are the cmdExecute data
  responseData[0] = b0;
  responseData[1] = b1;
  responseData[2] = b2;
  responseData[3] = b3;

  b0 = cmd->cmdExecute.execIdData <<= 24;
  b0 >>= 24;

  b1 = cmd->cmdExecute.execIdData <<= 16;
  b1 >>= 24;

  b2 = cmd->cmdExecute.execIdData <<= 8;
  b2 >>= 24;

  b3 = cmd->cmdExecute.execIdData;
  b3 >>= 24;

  // Last four bytes are the execId data
  responseData[4] = b0;
  responseData[5] = b1;
  responseData[6] = b2;
  responseData[7] = b3;

  *responseDataLen = 8;

  return OBC_ERR_CODE_SUCCESS;
}

const cmd_info_t cmdsConfig[] = {
    [CMD_END_OF_FRAME] = {NULL, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    // TODO: Change this to critial once critical commands are implemented
    [CMD_EXEC_OBC_RESET] = {execObcResetCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_RTC_SYNC] = {rtcSyncCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = {downlinkLogsNextPassCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_MICRO_SD_FORMAT] = {microSDFormatCmdCallback, CMD_POLICY_PROD, CMD_TYPE_CRITICAL},
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLINK_TELEM] = {downlinkTelemCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_I2C_PROBE] = {I2CProbeCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_ARM] = {armCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_EXECUTE] = {executeCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
};

// This function is purely to trick the compiler into thinking we are using the cmdsConfig variable so we avoid the
// unused variable error
void unusedFunc() { UNUSED(cmdsConfig); }
