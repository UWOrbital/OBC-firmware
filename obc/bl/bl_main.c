#include "bl_uart.h"
#include "bl_flash.h"
#include "gio.h"
#include "obc_gs_errors.h"
#include "obc_gs_crc.h"
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
#include "obc_metadata.h"

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

#if defined(BOARD_TYPE_TO_INT_MAP)
#define BOARD_ID BOARD_TYPE_TO_INT_MAP
#endif

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
    blUartWriteBytes(strlen("ERROR: Message Corrupted\r\n"), (uint8_t *)"ERROR: Message Corrupted\r\n");
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }

  RETURN_IF_ERROR_CODE(verifyCommand(&unpackedCmdMsg, &currCmdInfo));
  RETURN_IF_ERROR_CODE(processNonTimeTaggedCommand(&unpackedCmdMsg, &currCmdInfo));

  return errCode;
}

obc_error_code_t verifyBoardType(uint8_t boardType) {
  if (boardType == BOARD_ID) {
    blUartWriteBytes(strlen("SUCCESS: Board ID of bootloader and app match\r\n"),
                     (uint8_t *)"SUCCESS: Board ID of bootloader and app match\r\n");
    return OBC_ERR_CODE_SUCCESS;
  } else {
    blUartWriteBytes(strlen("ERROR: Board ID of bootloader and app are different, aborting...\r\n"),
                     (uint8_t *)"ERROR: Board ID of bootloader and app are different, aborting...\r\n");
    return OBC_ERR_CODE_BOARD_MISMATCH;
  }
}

obc_error_code_t verifyCrc(uint32_t crc_addr) {
  // Calculate crc via the crc32 algorithm (same one used in python's binascii and zlib libraries)
  uint32_t calculatedCrc = crc32(0, (uint8_t *)APP_START_ADDRESS, crc_addr - APP_START_ADDRESS);

  if (calculatedCrc == *((uint32_t *)crc_addr)) {
    blUartWriteBytes(strlen("SUCCESS: Crc matches\r\n"), (uint8_t *)"SUCCESS: Crc matches\r\n");
    return OBC_ERR_CODE_SUCCESS;
  } else {
    blUartWriteBytes(strlen("ERROR: Failed to run application\r\n"), (uint8_t *)"ERROR: Failed to run application\r\n");
    return OBC_ERR_CODE_CORRUPTED_MSG;
  }
}

void blJumpToApp() {
  // Cast the metadata of the flash into a usable pointer
  metadata_t *app_metadata = (metadata_t *)(APP_START_ADDRESS + APP_METADATA_OFFSET);

  // Check board id and verify the crc
  if (verifyBoardType(app_metadata->board_id) == OBC_ERR_CODE_SUCCESS &&
      verifyCrc(app_metadata->crc_addr) == OBC_ERR_CODE_SUCCESS) {
    blUartWriteBytes(strlen("ATTEMPTING: Running application...\r\n"),
                     (uint8_t *)"ATTEMPTING: Running application..\r\n");

    // We wait for about 100ms so that the remaining uart info can be sent before the buffer is cleared
    // by the app being initialized
    uint32_t initTime = blGetCurrentTick();
    while ((blGetCurrentTick() - initTime) < 100 || blGetCurrentTick() < initTime) {
    };

    // Go to the application's entry point
    uint32_t appStartAddress = (uint32_t)app_metadata->app_entry_func_addr;
    ((appStartFunc_t)appStartAddress)();
  }

  // If it was not possible to jump to the app, we log that error here
  blUartWriteBytes(strlen("ERROR: Failed to run application\r\n"), (uint8_t *)"ERROR: Failed to run application\r\n");
}

/* PUBLIC FUNCTIONS */
int main(void) {
  gioSetBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT, 1);
  obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

  blUartInit();
  blInitTick();
  gioInit();
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

  uint32_t flashTick = blGetCurrentTick();
  uint32_t ledTick = blGetCurrentTick();
  uint32_t timeout = 5000;
  gioSetBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT, 1);
  while (1) {
    if (blGetCurrentTick() - ledTick > 500) {
      gioToggleBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT);
      ledTick = blGetCurrentTick();
    }

    if (blGetCurrentTick() - flashTick > timeout) {
      blJumpToApp();
      break;
    } else if (blUartReadBytes(recvBuffer, MAX_PACKET_SIZE, 0) == OBC_ERR_CODE_SUCCESS) {
      LOG_IF_ERROR_CODE(blRunCommand(recvBuffer));
      timeout = 7000;
      flashTick = blGetCurrentTick();
    }
  }
}
