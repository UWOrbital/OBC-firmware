#include "bl_config.h"
#include "bl_flash.h"
#include "bl_uart.h"
#include "bl_errors.h"
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
#define BL_BIN_RX_CHUNK_SIZE 128U   // Bytes
#define BL_ECC_FIX_CHUNK_SIZE 128U  // Bytes
#define BL_MAX_MSG_SIZE 64U
#define RM46_FLASH_BANK 0U
#define LAST_SECTOR_START_ADDR blFlashSectorStartAddr(15U)
#define WAIT_FOREVER UINT32_MAX
#define MAX_PACKET_SIZE 223

/* TYPEDEFS */
typedef void (*appStartFunc_t)(void);

typedef enum {
  BL_STATE_IDLE,
  BL_STATE_DOWNLOAD_IMAGE,
  BL_STATE_ERASE_IMAGE,
  BL_STATE_RUN_APP,
} bl_state_t;

/* PUBLIC FUNCTIONS */
int main(void) {
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

  blUartInit();
  rtiInit();
  rtiStartCounter(rtiCOUNTER_BLOCK1);

  // F021 API and the functions that use it must be executed from RAM since they
  // can't execute from the same flash bank being modified
  memcpy(&__ramFuncsRunStart__, &__ramFuncsLoadStart__, (uint32_t)&__ramFuncsSize__);

  // bl_state_t state = BL_STATE_IDLE;
  uint8_t recvBuffer[MAX_PACKET_SIZE] = {0U};
  blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 200);
  if (recvBuffer[1] == 0U) {
    // Jump to app
  } else {
    while (1) {
      cmd_info_t currCmdInfo;
      cmd_msg_t unpackedCmdMsg = {0};
      uint32_t unpackOffset = 0;
      unpackCmdMsg(recvBuffer, &unpackOffset, &unpackedCmdMsg);
      LOG_IF_ERROR_CODE(verifyCommand(&unpackedCmdMsg, &currCmdInfo));
      LOG_IF_ERROR_CODE(processNonTimeTaggedCommand(&unpackedCmdMsg, &currCmdInfo));
      memset(recvBuffer, 0, sizeof(recvBuffer));
      blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 2000);
      if (recvBuffer[1] == 0U) {
        // Verify CRC
        // Verify Hardware
        // Jump to app
      }
    }
  }
}
