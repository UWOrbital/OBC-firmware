#include "bl_uart.h"
#include "obc_gs_errors.h"
#include "rti.h"
#include "obc_errors.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_unpack.h"
#include <metadata_struct.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "bl_logging.h"

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

uint8_t recvBuffer[MAX_PACKET_SIZE] = {0U};

static obc_error_code_t blRunCommand(uint8_t recvBuffer[]) {
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;
  cmd_info_t currCmdInfo;
  cmd_msg_t unpackedCmdMsg = {0};
  uint32_t unpackOffset = 0;
  obc_gs_error_code_t interfaceErr = unpackCmdMsg(recvBuffer, &unpackOffset, &unpackedCmdMsg);
  if (interfaceErr != OBC_GS_ERR_CODE_SUCCESS) {
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }
  RETURN_IF_ERROR_CODE(verifyCommand(&unpackedCmdMsg, &currCmdInfo));
  RETURN_IF_ERROR_CODE(processNonTimeTaggedCommand(&unpackedCmdMsg, &currCmdInfo));
  memset(recvBuffer, 0, MAX_PACKET_SIZE);
  return errCode;
}

/* PUBLIC FUNCTIONS */
int main(void) {
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

  blUartInit();
  rtiInit();
  rtiStartCounter(rtiCOUNTER_BLOCK1);

  // F021 API and the functions that use it must be executed from RAM since they
  // can't execute from the same flash bank being modified
  memcpy(&__ramFuncsRunStart__, &__ramFuncsLoadStart__, (uint32_t)&__ramFuncsSize__);

  if (blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 2000) != OBC_ERR_CODE_SUCCESS) {
    // Jump to app
    blUartWriteBytes(1, (uint8_t *)"B");
  } else {
    blUartWriteBytes(1, (uint8_t *)"Z");
    LOG_IF_ERROR_CODE(blRunCommand(recvBuffer));

    while (1) {
      if (blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 7000) != OBC_ERR_CODE_SUCCESS) {
        // Verify CRC
        // Verify Hardware
        // Jump to app
        blUartWriteBytes(1, (uint8_t *)"V");
        break;
      }
      blUartWriteBytes(1, (uint8_t *)"W");
      LOG_IF_ERROR_CODE(blRunCommand(recvBuffer));
    }
  }
}
