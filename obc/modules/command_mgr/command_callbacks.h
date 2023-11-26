#pragma once

#include "obc_errors.h"
#include "obc_gs_command_data.h"

typedef obc_error_code_t (*cmd_callback_t)(cmd_msg_t *);

/* Declare all command callbacks below */

// CMD_EXEC_OBC_RESET
obc_error_code_t execObcResetCmdCallback(cmd_msg_t *cmd);

// CMD_RTC_SYNC
obc_error_code_t rtcSyncCmdCallback(cmd_msg_t *cmd);

// CMD_DOWNLINK_LOGS_NEXT_PASS
obc_error_code_t downlinkLogsNextPassCmdCallback(cmd_msg_t *cmd);

// CMD_MICRO_SD_FORMAT
obc_error_code_t microSDFormatCmdCallback(cmd_msg_t *cmd);

// CMD_PING
obc_error_code_t pingCmdCallback(cmd_msg_t *cmd);

// CMD_DOWNLINK_TELEM
obc_error_code_t downlinkTelemCmdCallback(cmd_msg_t *cmd);

// CMD_FRAM_DUMP
obc_error_code_t framDumpCmdCallback(cmd_msg_t *cmd);
