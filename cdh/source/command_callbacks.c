#include "command_callbacks.h"
#include "command_data.h"
#include "comms_manager.h"
#include "ds3232_mz.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_time.h"

#include <FreeRTOS.h>
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
    if (cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
    // Update RTC and Unix Time
    uint32_t timeRTC = cmd->timestamp;
    setSecondsRTC(timeRTC / SECONDS_TO_HOURS);
    setMinutesRTC((timeRTC % SECONDS_TO_HOURS) / SECONDS_TO_MINS);
    setHourRTC(timeRTC % SECONDS_TO_MINS);
    setCurrentUnixTime(cmd->rtcSync.unixTime);

    LOG_DEBUG("Executing RTC sync command - time %lu", cmd->rtcSync.unixTime);
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

obc_error_code_t downlinkTelemetryCmdCallback(cmd_msg_t *cmd) {
    if(cmd == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if(sendDownlinkTelemetryEvent() != OBC_ERR_CODE_SUCCESS) {
        return OBC_ERR_CODE_UNKNOWN;
    }

    LOG_DEBUG("Executing telemetry downlink command");
    return OBC_ERR_CODE_SUCCESS;
}
