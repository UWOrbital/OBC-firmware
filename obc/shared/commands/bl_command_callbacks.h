#pragma once

#include "obc_errors.h"
#include "obc_gs_command_data.h"

typedef obc_error_code_t (*cmd_callback_t)(cmd_msg_t *);

// CMD_PING
obc_error_code_t pingCmdCallback(cmd_msg_t *cmd);
