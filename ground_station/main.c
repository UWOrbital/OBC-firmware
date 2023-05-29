#include <stdio.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"

int main(void) {
    cmd_msg_t cmdMsg = {.id = CMD_EXEC_OBC_RESET, .isTimeTagged = true, .timestamp = 0x12345678UL};
    
    uint8_t buff[24] = {0};
    size_t offset = 0;

    obc_error_code_t errCode = packCmdMsg(buff, &offset, &cmdMsg);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        printf("Error: %d\n", errCode);
        return 1;
    }

    for (int i = 0; i < offset; i++) {
        printf("%02x ", buff[i]);
    }
    printf("\n");

    return 0;
}
