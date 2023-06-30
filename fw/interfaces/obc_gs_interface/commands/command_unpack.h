#ifndef CDH_INCLUDE_COMMAND_UNPACK_H_
#define CDH_INCLUDE_COMMAND_UNPACK_H_

#include "command_data.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

obc_error_code_t unpackCmdMsg(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg);

/* Unpack functions for each command */

// CMD_EXEC_OBC_RESET
void unpackExecObcResetCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// CMD_RTC_SYNC
void unpackRtcSyncCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// CMD_DOWNLINK_LOGS_NEXT_PASS
void unpackDownlinkLogsNextPassCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// CMD_MICRO_SD_FORMAT
void unpackMicroSdFormat(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg);

// CMD_PING
void unpackPingCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

// CMD_DOWNLINK_TELEM
void unpackDownlinkTelemCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif  // CDH_INCLUDE_COMMAND_UNPACK_H_
