#include "obc_general_util.h"
#include "obc_gs_command_id.h"
#include "command.h"
#include <stddef.h>

static obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

const cmd_info_t cmdsConfig[] = {[CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL}};

void unusedFunc() { UNUSED(cmdsConfig); }
