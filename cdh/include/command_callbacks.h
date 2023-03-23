#ifndef CDH_INCLUDE_COMMAND_CALLBACKS_H_
#define CDH_INCLUDE_COMMAND_CALLBACKS_H_

#include "obc_errors.h"
#include "command_data.h"

typedef obc_error_code_t (*cmd_callback_t)(cmd_msg_t *cmd);

/* Declare all command callbacks below */

obc_error_code_t execObcResetCmdCallback(cmd_msg_t *cmd);

obc_error_code_t rtcSyncCmdCallback(cmd_msg_t *cmd);

obc_error_code_t downlinkLogsNextPassCmdCallback(cmd_msg_t *cmd);

#endif // CDH_INCLUDE_COMMAND_CALLBACKS_H_
