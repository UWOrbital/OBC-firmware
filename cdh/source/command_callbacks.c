#include "command_callbacks.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"

obc_error_code_t execObcResetCmdCallback(void) {
    // TODO: Implement safe reset (i.e. save state somewhere)
    LOG_DEBUG("Executing OBC reset command");
    resetSystem(RESET_REASON_CMD_EXEC_OBC_RESET);

    // Should never get here
    return OBC_ERR_CODE_UNKNOWN;
}

obc_error_code_t rtcSyncCmdCallback(rtc_sync_cmd_data_t cmd) {
    LOG_DEBUG("Executing RTC sync command - time %lu", cmd.unixTime);
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t downlinkLogsNextPassCmdCallback(downlink_logs_next_pass_cmd_data_t cmd) {
    LOG_DEBUG("Executing log downlink command - log level %u", cmd.logLevel);
    return OBC_ERR_CODE_SUCCESS;
}
