#include "obc_general_util.h"
#include "obc_gs_command_id.h"
#include "command.h"
#include <stddef.h>
#include "bl_uart.h"

static obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  blUartWriteBytes(1, (uint8_t *)"A");
  return OBC_ERR_CODE_SUCCESS;
}

const cmd_info_t cmdsConfig[] = {[CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};

// This function is purely to trick the compiler into thinking we are using the cmdsConfig variable so we avoid the
// unused variable error
void unusedFunc() { UNUSED(cmdsConfig); }
