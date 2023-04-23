#ifndef CDH_INCLUDE_COMMAND_CALLBACKS_H_
#define CDH_INCLUDE_COMMAND_CALLBACKS_H_

#include "obc_errors.h"
#include "command_data.h"

typedef obc_error_code_t (*cmd_callback_t)(cmd_msg_t *);

/* Declare all command callbacks below */

// CMD_EXEC_OBC_RESET
obc_error_code_t execObcResetCmdCallback(cmd_msg_t *cmd);

// CMD_RTC_SYNC
obc_error_code_t rtcSyncCmdCallback(cmd_msg_t *cmd);

// CMD_DOWNLINK_LOGS_NEXT_PASS
obc_error_code_t downlinkLogsNextPassCmdCallback(cmd_msg_t *cmd);

#endif // CDH_INCLUDE_COMMAND_CALLBACKS_H_
