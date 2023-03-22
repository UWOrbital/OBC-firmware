#ifndef CDH_INCLUDE_COMMAND_CALLBACKS_H_
#define CDH_INCLUDE_COMMAND_CALLBACKS_H_

#include "obc_errors.h"
#include "command_data.h"

/* Declare all command callbacks below */

obc_error_code_t execObcResetCmdCallback(void);

obc_error_code_t rtcSyncCmdCallback(rtc_sync_cmd_data_t cmd);

obc_error_code_t downlinkLogsNextPassCmdCallback(downlink_logs_next_pass_cmd_data_t cmd);

#endif // CDH_INCLUDE_COMMAND_CALLBACKS_H_
