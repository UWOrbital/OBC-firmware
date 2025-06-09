#include "command_callbacks.h"
#include "FreeRTOS.h"

obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}
