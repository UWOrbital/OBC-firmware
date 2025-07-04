#include "command_callbacks.h"
#include "obc_gs_command_data.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_time.h"
#include "obc_time_utils.h"
#include "downlink_encoder.h"
#include "comms_manager.h"
#include "telemetry_manager.h"

#include <redposix.h>
#include <stddef.h>

obc_error_code_t execObcResetCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement safe reset (i.e. save state somewhere)
  LOG_DEBUG("Executing OBC reset command");
  resetSystem(RESET_REASON_CMD_EXEC_OBC_RESET);

  // Should never get here
  return OBC_ERR_CODE_UNKNOWN;
}

obc_error_code_t rtcSyncCmdCallback(cmd_msg_t *cmd) {
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

obc_error_code_t downlinkLogsNextPassCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement handling for this command. Check if the log level is valid
  LOG_DEBUG("Executing log downlink command");
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t microSDFormatCmdCallback(cmd_msg_t *cmd) {
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

obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
  obc_error_code_t errCode;

  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  encode_event_t encodeQueueMsg = {0};
  encodeQueueMsg.eventID = DOWNLINK_DATA_BUFFER;
  encodeQueueMsg.telemetryDataBuffer.bufferSize = 1;
  encodeQueueMsg.telemetryDataBuffer.telemData[0] =
      (telemetry_data_t){.id = TELEM_PONG, .timestamp = getCurrentUnixTime()};

  RETURN_IF_ERROR_CODE(sendToDownlinkEncodeQueue(&encodeQueueMsg));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t downlinkTelemCmdCallback(cmd_msg_t *cmd) {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(setTelemetryManagerDownlinkReady());

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t uplinkDisconnectCmdCallback(cmd_msg_t *cmd) {
  obc_error_code_t errCode;

  comms_event_t disconnectSentEvent = {.eventID = COMMS_EVENT_START_DISC};
  RETURN_IF_ERROR_CODE(sendToCommsManagerQueue(&disconnectSentEvent));

  return OBC_ERR_CODE_SUCCESS;
}
