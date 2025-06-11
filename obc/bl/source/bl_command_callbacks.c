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

static obc_error_code_t setProgrammingSessionCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t eraseAppCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t downloadDataCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t verifyCrcCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t resetBlCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

const cmd_info_t cmdsConfig[] = {
    [CMD_PING] = {pingCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_SET_PROGRAMMING_SESSION] = {setProgrammingSessionCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_ERASE_APP] = {eraseAppCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_DOWNLOAD_DATA] = {downloadDataCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_VERIFY_CRC] = {verifyCrcCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
    [CMD_RESET_BL] = {resetBlCmdCallback, CMD_POLICY_PROD, CMD_TYPE_NORMAL},
};

// This function is purely to trick the compiler into thinking we are using the cmdsConfig variable so we avoid the
// unused variable error
void unusedFunc() { UNUSED(cmdsConfig); }
