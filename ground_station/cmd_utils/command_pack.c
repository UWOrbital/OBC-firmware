#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_pack_utils.h"
#include "obc_errors.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*pack_func_t)(uint8_t*, size_t*, const cmd_msg_t *);

static const pack_func_t packFns[] = {
    [CMD_EXEC_OBC_RESET] = packExecObcResetCmdData,
    [CMD_RTC_SYNC] = packRtcSyncCmdData,
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = packDownlinkLogsNextPassCmdData,
    [CMD_MICRO_SD_FORMAT] = packMicroSdFormat,
    [CMD_PING] = packPingCmdData,
    [CMD_DOWNLINK_TELEM] = packDownlinkTelemCmdData,
    // Add more functions for other commands as needed
};

#define MAX_CMD_ID ((sizeof(packFns) / sizeof(pack_func_t)) - 1)

// Pack the command message
obc_error_code_t packCmdMsg(uint8_t* buffer, size_t *offset, const cmd_msg_t* cmdMsg, uint8_t *numPacked) {
    if (buffer == NULL || offset == NULL || cmdMsg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (cmdMsg->id > MAX_CMD_ID) {
        return OBC_ERR_CODE_UNSUPPORTED_CMD;
    }

    if (packFns[cmdMsg->id] == NULL) {
        return OBC_ERR_CODE_UNSUPPORTED_CMD;
    }

    uint8_t oldOffset = *offset;
    uint8_t uplinkedId = cmdMsg->isTimeTagged ? (cmdMsg->id | 0x80) : cmdMsg->id;
    packUint8(uplinkedId, buffer, offset);

    packUint32(cmdMsg->timestamp, buffer, offset);

    packFns[cmdMsg->id](buffer, offset, cmdMsg);

    *numPacked = *offset - oldOffset; 
    return OBC_ERR_CODE_SUCCESS;
}

// CMD_EXEC_OBC_RESET
void packExecObcResetCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg) {
    // No data to pack
}

// CMD_RTC_SYNC
void packRtcSyncCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg) {
    packUint32(cmdMsg->rtcSync.unixTime, buffer, offset);
}

// CMD_DOWNLINK_LOGS_NEXT_PASS
void packDownlinkLogsNextPassCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg) {
    packUint8((uint8_t)cmdMsg->downlinkLogsNextPass.logLevel, buffer, offset);
}

// CMD_MICRO_SD_FORMAT
void packMicroSdFormat(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg) {
    // No data to pack
}

// CMD_PING
void packPingCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* cmdMsg) {
    // No data to pack
}

// CMD_DOWNLINK_TELEM
void packDownlinkTelemCmdData(uint8_t* buffer, size_t* offset, const cmd_msg_t* msg) {
    // No data to pack
}
