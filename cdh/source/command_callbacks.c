#include "command_callbacks.h"
#include "command_data.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_time.h"
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

    // TODO: Implement handling for this command
    LOG_DEBUG("Executing log downlink command - log level %u", cmd->downlinkLogsNextPass.logLevel);
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t microSDFormatCmdCallback(cmd_msg_t *cmd) {
    if(cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    int32_t ret = red_format("");
    if (ret != 0) {
        LOG_DEBUG("Executing microSD format error code %u", OBC_ERR_CODE_FS_FORMAT_FAILED);
        return OBC_ERR_CODE_FS_FORMAT_FAILED;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
    obc_error_code_t errCode;

    if (cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    comms_event_t queueMsg;
    queueMsg.eventID = DOWNLINK_DATA_BUFFER;
    queueMsg.telemetryDataBuffer.telemData[0] = (telemetry_data_t) {.id = TELEM_PONG, .timestamp = getCurrentUnixTime()};

    RETURN_IF_ERROR_CODE(sendToCommsQueue(&queueMsg));
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t downlinkTelemCmdCallback(cmd_msg_t *cmd) {
    obc_error_code_t errCode;

    RETURN_IF_ERROR_CODE(setTelemetryManagerDownlinkReady());

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
    obc_error_code_t errCode;

    if (cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    comms_event_t queueMsg;
    queueMsg.eventID = DOWNLINK_DATA_BUFFER;
    queueMsg.telemetryDataBuffer.telemData[0] = (telemetry_data_t) {.id = TELEM_PONG, .timestamp = getCurrentUnixTime()};

    RETURN_IF_ERROR_CODE(sendToCommsQueue(&queueMsg));
    
    return OBC_ERR_CODE_SUCCESS;
}
