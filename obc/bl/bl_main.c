#include "bl_uart.h"
#include "bl_flash.h"
#include "obc_gs_errors.h"
#include "rti.h"
#include "obc_errors.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_unpack.h"
#include <metadata_struct.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "command.h"
#include "obc_logging.h"
#include "bl_config.h"
#include "bl_errors.h"
#include "bl_time.h"

/* LINKER EXPORTED SYMBOLS */
extern uint32_t __ramFuncsLoadStart__;
extern uint32_t __ramFuncsSize__;

extern uint32_t __ramFuncsRunStart__;
extern uint32_t __ramFuncsRunEnd__;

/* DEFINES */
// These values were chosen so that the UART transfers and flash writes are quick, but don't
// use too much RAM
#define BL_ECC_FIX_CHUNK_SIZE 128U  // Bytes
#define LAST_SECTOR_START_ADDR blFlashSectorStartAddr(15U)
#define WAIT_FOREVER UINT32_MAX
#define MAX_PACKET_SIZE 223

/* TYPEDEFS */
typedef void (*appStartFunc_t)(void);

// Get this from the bl_command_callbacks for simplicity
extern programming_session_t programmingSession;

obc_error_code_t blRunCommand(uint8_t recvBuffer[]) {
  if (recvBuffer == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
  cmd_info_t currCmdInfo;
  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  obc_gs_error_code_t interfaceErr = unpackCmdMsg(recvBuffer, &unpackOffset, &unpackedCmdMsg);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    blUartWriteBytes(strlen("Message Corrupted\r\n"), (uint8_t *)"Message Corrupted\r\n");
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }
  RETURN_IF_ERROR_CODE(verifyCommand(&unpackedCmdMsg, &currCmdInfo));
  RETURN_IF_ERROR_CODE(processNonTimeTaggedCommand(&unpackedCmdMsg, &currCmdInfo));
  return errCode;
}

void blJumpToApp() {
  // Jump to app
  blUartWriteBytes(strlen("Running application\r\n"), (uint8_t *)"Running application\r\n");

  uint32_t initTime = blGetCurrentTick();
  while ((blGetCurrentTick() - initTime) < 100 || blGetCurrentTick() < initTime) {
  };
  // Go to the application's entry point
  uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
  ((appStartFunc_t)appStartAddress)();

  // If it was not possible to jump to the app, we log that error here
  blUartWriteBytes(strlen("Failed to run application\r\n"), (uint8_t *)"Failed to run application\r\n");
}

/* PUBLIC FUNCTIONS */
int main(void) {
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

  blUartInit();
  blInitTick();
  uint8_t recvBuffer[MAX_PACKET_SIZE] = {0U};

  // F021 API and the functions that use it must be executed from RAM since they
  // can't execute from the same flash bank being modified
  memcpy(&__ramFuncsRunStart__, &__ramFuncsLoadStart__, (uint32_t)&__ramFuncsSize__);

  bl_error_code_t interfaceErr = blFlashFapiInitBank(RM46_FLASH_BANK);

  if (interfaceErr != BL_ERR_CODE_SUCCESS) {
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

  if (blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 5000) != OBC_ERR_CODE_SUCCESS) {
    // Jump to app if the initial timeout is exceeded
    blJumpToApp();
  } else {
    LOG_IF_ERROR_CODE(blRunCommand(recvBuffer));

    while (1) {
      if (blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 7000) != OBC_ERR_CODE_SUCCESS) {
        // Verify CRC
        // Verify Hardware
        blJumpToApp();
        break;
      }
      LOG_IF_ERROR_CODE(blRunCommand(recvBuffer));
    }
  }
}
