#ifndef CDH_INCLUDE_COMMAND_PACK_H_
#define CDH_INCLUDE_COMMAND_PACK_H_

#include "command_data.h"

#include <stdint.h>
#include <stddef.h>

obc_error_code_t packCmdMsg(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg, uint8_t* numPacked);

/* Pack functions for each command */

// CMD_EXEC_OBC_RESET
void packExecObcResetCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* msg);

// CMD_RTC_SYNC
void packRtcSyncCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* msg);

// CMD_DOWNLINK_LOGS_NEXT_PASS
void packDownlinkLogsNextPassCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* msg);

// CMD_MICRO_SD_FORMAT
void packMicroSdFormat(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg);

// CMD_PING
void packPingCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* msg);

// CMD_DOWNLINK_TELEM
void packDownlinkTelemCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* msg);

#endif  // CDH_INCLUDE_COMMAND_PACK_H_
