#include "command_unpack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_unpack_utils.h"
#include "obc_errors.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*unpack_func_t)(const uint8_t*, uint32_t*, cmd_msg_t*);

static const unpack_func_t unpackFns[] = {
    [CMD_EXEC_OBC_RESET] = unpackExecObcResetCmdData,
    [CMD_RTC_SYNC] = unpackRtcSyncCmdData,
    [CMD_DOWNLINK_LOGS_NEXT_PASS] = unpackDownlinkLogsNextPassCmdData,
    // Add more functions for other commands as needed
};

#define MAX_CMD_ID ((sizeof(unpackFns) / sizeof(unpack_func_t)) - 1)

// Unpack the command message
obc_error_code_t unpackCmdMsg(const uint8_t* buffer, uint32_t *offset, cmd_msg_t* cmdMsg) {
    if (buffer == NULL || offset == NULL || cmdMsg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint8_t id = unpackUint8(buffer, offset);

    // MSB is 0 if the command is time tagged    
    bool isTimeTagged = (id & 0x80) ? false : true;

    // Mask out the MSB
    id = id & 0x7F;

    if (id > MAX_CMD_ID) {
        return OBC_ERR_CODE_UNSUPPORTED_CMD;
    }

    if (unpackFns[id] == NULL) {
        return OBC_ERR_CODE_UNSUPPORTED_CMD;
    }

    uint32_t timestamp = unpackUint32(buffer, offset);

    cmdMsg->id = id;
    cmdMsg->timestamp = timestamp;
    cmdMsg->isTimeTagged = isTimeTagged;

    unpackFns[id](buffer, offset, cmdMsg);

    return OBC_ERR_CODE_SUCCESS;
}

// CMD_EXEC_OBC_RESET
void unpackExecObcResetCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* msg) {
    // No data to unpack
}

// CMD_RTC_SYNC
void unpackRtcSyncCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {
    cmdMsg->rtcSync.unixTime = unpackUint32(buffer, offset);
}

// CMD_DOWNLINK_LOGS_NEXT_PASS
void unpackDownlinkLogsNextPassCmdData(const uint8_t* buffer, uint32_t* offset, cmd_msg_t* cmdMsg) {
    cmdMsg->downlinkLogsNextPass.logLevel = (log_level_t)unpackUint8(buffer, offset);
}
