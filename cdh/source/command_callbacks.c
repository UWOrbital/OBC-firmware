#include "command_callbacks.h"
#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"

obc_error_code_t execObcResetCmdCallback(void) {
    // TODO: Implement safe reset (i.e. save state somewhere)
    resetSystem(RESET_REASON_CMD_EXEC_OBC_RESET);

    // Should never get here
    return OBC_ERR_CODE_UNKNOWN;
}

obc_error_code_t rtcSyncCmdCallback(rtc_sync_cmd_data_t cmd) {
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t downlinkLogsNextPassCmdCallback(downlink_logs_next_pass_cmd_data_t cmd) {
    return OBC_ERR_CODE_SUCCESS;
}
