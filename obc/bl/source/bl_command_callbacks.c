#include "bl_config.h"
#include "bl_errors.h"
#include "obc_errors.h"
#include "obc_general_util.h"
#include "obc_gs_command_id.h"
#include "command.h"
#include <stddef.h>
#include <stdint.h>
#include "bl_uart.h"
#include "bl_flash.h"
#include <stdio.h>

#define BL_BIN_RX_CHUNK_SIZE 208U  // Bytes
#define MAX_PACKET_SIZE 223
#define RM46_FLASH_BANK 0U
#define BL_MAX_MSG_SIZE 64U

programming_session_t programmingSession = BOOTLOADER;

static obc_error_code_t pingCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  blUartWriteBytes(strlen("BL Ping Success\r\n"), (uint8_t *)"BL Ping Success\r\n");
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t setProgrammingSessionCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (cmd->setProgrammingSession.programmingSession == BOOTLOADER) {
    programmingSession = BOOTLOADER;
  } else if (cmd->setProgrammingSession.programmingSession == APPLICATION) {
    programmingSession = APPLICATION;
  } else {
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t eraseAppCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // TODO: Verify Erase implementation
  bl_error_code_t errCode = blFlashFapiInitBank(RM46_FLASH_BANK);

  if (errCode != BL_ERR_CODE_SUCCESS) {
    char blUartWriteBuffer[BL_MAX_MSG_SIZE] = {0};
    int32_t blUartWriteBufferLen =
        snprintf(blUartWriteBuffer, BL_MAX_MSG_SIZE, "Failed to init flash, BL error code: %d\r\n", errCode);
    if (blUartWriteBufferLen < 0) {
      blUartWriteBytes(strlen("Error with processing message buffer length\r\n"),
                       (uint8_t *)"Error with processing message buffer length\r\n");
    } else {
      blUartWriteBytes(blUartWriteBufferLen, (uint8_t *)blUartWriteBuffer);
    }
  }

  errCode = blFlashFapiBlockErase(APP_START_ADDRESS, APP_SIZE);

  if (errCode != BL_ERR_CODE_SUCCESS) {
    char blUartWriteBuffer[BL_MAX_MSG_SIZE] = {0};
    int32_t blUartWriteBufferLen =
        snprintf(blUartWriteBuffer, BL_MAX_MSG_SIZE, "Failed to erase, BL error code: %d\r\n", errCode);
    if (blUartWriteBufferLen < 0) {
      blUartWriteBytes(strlen("Error with processing message buffer length\r\n"),
                       (uint8_t *)"Error with processing message buffer length\r\n");
    } else {
      blUartWriteBytes(blUartWriteBufferLen, (uint8_t *)blUartWriteBuffer);
    }
    return OBC_ERR_CODE_FAILED_FILE_WRITE;
  }

  blUartWriteBytes(strlen("Erase success\r\n"), (uint8_t *)"Erase success\r\n");
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t downloadDataCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // TODO: Replace magic number
  if (!blFlashIsStartAddrValid(cmd->downloadData.address, 208)) {
    blUartWriteBytes(strlen("Invalid start address\r\n"), (uint8_t *)"Invalid start address\r\n");
    return OBC_ERR_CODE_INVALID_ARG;
  }

  blUartWriteBytes(strlen("Received packet\r\n"), (uint8_t *)"Received packet\r\n");

  bl_error_code_t errCode = blFlashFapiInitBank(RM46_FLASH_BANK);

  if (errCode != BL_ERR_CODE_SUCCESS) {
    char blUartWriteBuffer[BL_MAX_MSG_SIZE] = {0};
    int32_t blUartWriteBufferLen =
        snprintf(blUartWriteBuffer, BL_MAX_MSG_SIZE, "Failed to init flash, BL error code: %d\r\n", errCode);
    if (blUartWriteBufferLen < 0) {
      blUartWriteBytes(strlen("Error with processing message buffer length\r\n"),
                       (uint8_t *)"Error with processing message buffer length\r\n");
    } else {
      blUartWriteBytes(blUartWriteBufferLen, (uint8_t *)blUartWriteBuffer);
    }
  }

  errCode =
      blFlashFapiBlockWrite(cmd->downloadData.address, (uint32_t)cmd->downloadData.data, cmd->downloadData.length);

  if (errCode != BL_ERR_CODE_SUCCESS) {
    char blUartWriteBuffer[BL_MAX_MSG_SIZE] = {0};
    int32_t blUartWriteBufferLen =
        snprintf(blUartWriteBuffer, BL_MAX_MSG_SIZE, "Failed to write, BL error code: %d\r\n", errCode);
    if (blUartWriteBufferLen < 0) {
      blUartWriteBytes(strlen("Error with processing message buffer length\r\n"),
                       (uint8_t *)"Error with processing message buffer length\r\n");
    } else {
      blUartWriteBytes(blUartWriteBufferLen, (uint8_t *)blUartWriteBuffer);
    }
    return OBC_ERR_CODE_FAILED_FILE_WRITE;
  }

  blUartWriteBytes(strlen("Write success\r\n"), (uint8_t *)"Write success\r\n");
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t verifyCrcCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement a check that verifies the crc

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t resetBlCmdCallback(cmd_msg_t *cmd) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // TODO: Implement OBC reset functionality

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
