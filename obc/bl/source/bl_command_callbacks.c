#include "bl_errors.h"
#include "bl_config.h"
#include "obc_errors.h"
#include "obc_general_util.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_id.h"
#include "command.h"
#include <stddef.h>
#include <stdint.h>
#include "bl_uart.h"
#include "bl_flash.h"
#include "obc_gs_crc.h"
#include "obc_metadata.h"
#include <stdio.h>

#define BL_BIN_RX_CHUNK_SIZE 208U  // Bytes
#define MAX_PACKET_SIZE 223

programming_session_t programmingSession = APPLICATION;
extern uint32_t __APP_IMAGE_TOTAL_SECTION_SIZE;

static obc_error_code_t pingCmdCallback(cmd_msg_t *cmd, uint8_t *responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  blUartWriteBytes(strlen("BL Ping Success\r\n"), (uint8_t *)"BL Ping Success\r\n");
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t setProgrammingSessionCmdCallback(cmd_msg_t *cmd, uint8_t *responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (cmd->setProgrammingSession.programmingSession == APPLICATION) {
    programmingSession = APPLICATION;
  } else {
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t eraseAppCmdCallback(cmd_msg_t *cmd, uint8_t *responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  bl_error_code_t errCode =
      blFlashFapiBlockErase((uint32_t)APP_START_ADDRESS, (uint32_t)(&__APP_IMAGE_TOTAL_SECTION_SIZE));

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

static obc_error_code_t downloadDataCmdCallback(cmd_msg_t *cmd, uint8_t *responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  // TODO: Replace magic number
  if (!blFlashIsStartAddrValid(cmd->downloadData.address, 208)) {
    blUartWriteBytes(strlen("Invalid start address\r\n"), (uint8_t *)"Invalid start address\r\n");
    return OBC_ERR_CODE_INVALID_ARG;
  }

  blUartWriteBytes(strlen("Received packet\r\n"), (uint8_t *)"Received packet\r\n");

  bl_error_code_t errCode =
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

static obc_error_code_t verifyCrcCmdCallback(cmd_msg_t *cmd, uint8_t *responseData) {
  if (cmd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  metadata_t *app_metadata = (metadata_t *)(APP_START_ADDRESS + APP_METADATA_OFFSET);
  uint32_t calculatedCrc = crc32(0, (uint8_t *)APP_START_ADDRESS, app_metadata->crc_addr - APP_START_ADDRESS);
  memcpy(responseData, &calculatedCrc, sizeof(calculatedCrc));

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t resetBlCmdCallback(cmd_msg_t *cmd, uint8_t *responseData) {
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
