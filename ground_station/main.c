#include <stdio.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"

int main(void) {
    initLogger();

    obc_error_code_t errCode;
    cmd_msg_t cmdMsg = {.id = CMD_EXEC_OBC_RESET, .isTimeTagged = true, .timestamp = 0x12345678UL};
    
    uint8_t buff[24] = {0};
    size_t offset = 0;

    LOG_IF_ERROR_CODE(packCmdMsg(buff, &offset, &cmdMsg));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        return 1;
    }

    for (int i = 0; i < offset; i++) {
        printf("%02x ", buff[i]);
    }
    printf("\n");

    return 0;
}
