#ifndef CDH_INCLUDE_COMMAND_UNPACK_H_
#define CDH_INCLUDE_COMMAND_UNPACK_H_

#include "command_data.h"
#include <stdint.h>

obc_error_code_t unpackCmdMsg(const uint8_t* buffer, uint32_t *offset, cmd_msg_t* cmdMsg);

/* Unpack functions for each command */

// CMD_EXEC_OBC_RESET
void unpackExecObcResetCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// CMD_RTC_SYNC
void unpackRtcSyncCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// CMD_DOWNLINK_LOGS_NEXT_PASS
void unpackDownlinkLogsNextPassCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

#endif // CDH_INCLUDE_COMMAND_UNPACK_H_