#include "bl_config.h"
#include "bl_flash.h"
#include "bl_uart.h"

#include <stdio.h>
#include <string.h>

/* LINKER EXPORTED SYMBOLS */
extern uint32_t __ramFuncsLoadStart__;
extern uint32_t __ramFuncsSize__;

extern uint32_t __ramFuncsRunStart__;
extern uint32_t __ramFuncsRunEnd__;

/* TYPEDEFS */
typedef void (*appStartFunc_t)(void);

// If this header changes, update the host utility
typedef struct {
  uint32_t version;
  uint32_t size;
} app_header_t;

typedef enum {
  BL_STATE_IDLE,
  BL_STATE_DOWNLOAD_IMAGE,
  BL_STATE_ERASE_IMAGE,
  BL_STATE_RUN_APP,
} bl_state_t;

/* PUBLIC FUNCTIONS */
int main(void) {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;

  bl_uart_init();

  // F021 API and the functions that use it must be executed from RAM since they
  // can't execute from the same flash bank being modified
  memcpy(&__ramFuncsRunStart__, &__ramFuncsLoadStart__, (uint32_t)&__ramFuncsSize__);

  bl_state_t state = BL_STATE_IDLE;

  // TODO: Modify the transfer protocol to be faster and more robust
  while (1) {
    switch (state) {
      case BL_STATE_IDLE: {
        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Waiting for input\r\n"), (uint8_t *)"Waiting for input\r\n");

        char c = '\0';
        bl_uart_readBytes(BL_UART_SCIREG_2, (uint8_t *)&c, 1U);

        if (c == 'd') {
          state = BL_STATE_DOWNLOAD_IMAGE;
        } else if (c == 'e') {
          state = BL_STATE_ERASE_IMAGE;
        } else if (c == 'r') {
          state = BL_STATE_RUN_APP;
        }

        break;
      }
      case BL_STATE_DOWNLOAD_IMAGE: {
        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Downloading application\r\n"),
                           (uint8_t *)"Downloading application\r\n");

        uint8_t recvBuffer[sizeof(app_header_t)] = {0U};

        bl_uart_readBytes(BL_UART_SCIREG_2, recvBuffer, sizeof(app_header_t));

        app_header_t appHeader = {0};
        memcpy((void *)&appHeader, (void *)recvBuffer, sizeof(app_header_t));

        if (appHeader.size == 0U) {
          bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Invalid image size\r\n"), (uint8_t *)"Invalid image size\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        if (!bl_flash_isStartAddrValid(APP_START_ADDRESS, appHeader.size)) {
          bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Invalid start address\r\n"),
                             (uint8_t *)"Invalid start address\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Received header\r\n"), (uint8_t *)"Received header\r\n");

        errCode = bl_flash_FapiInitBank(0U);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Failed to init flash\r\n"),
                             (uint8_t *)"Failed to init flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        errCode = bl_flash_FapiBlockErase(APP_START_ADDRESS, appHeader.size);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Failed to erase flash\r\n"),
                             (uint8_t *)"Failed to erase flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Erased flash\r\n"), (uint8_t *)"Erased flash\r\n");

        // Host will send a 'D' before sending the image
        while (1) {
          char waitChar = '\0';

          bl_uart_readBytes(BL_UART_SCIREG_2, (uint8_t *)&waitChar, 1U);

          if (waitChar == 'D') {
            break;
          }
        }

        bl_flash_FapiInitBank(0U);

        // Receive image in chunks of 128 bytes and write to flash
        uint32_t numAppBytesToFlash = appHeader.size;
        while (numAppBytesToFlash > 0) {
          uint8_t recvBuffer[128] = {0U};

          uint32_t numBytesToRead = (numAppBytesToFlash > 128) ? 128 : numAppBytesToFlash;

          bl_uart_readBytes(BL_UART_SCIREG_2, recvBuffer, numBytesToRead);

          bl_flash_FapiBlockWrite(APP_START_ADDRESS + (appHeader.size - numAppBytesToFlash), (uint32_t)recvBuffer,
                                  numBytesToRead);

          numAppBytesToFlash -= numBytesToRead;
        }

        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Wrote application\r\n"), (uint8_t *)"Wrote application\r\n");

        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Fixing ECC\r\n"), (uint8_t *)"Fixing ECC\r\n");

        // Fix the ECC for any flash memory that was erased, but not overwritten by the new app
        uint8_t eccFixWriteBuf[128] = {0U};
        memset(eccFixWriteBuf, 0xFFU, sizeof(eccFixWriteBuf));  // Erased flash defaults to 0xFF

        const uint32_t eccFixTotalBytes =
            bl_flash_sectorEndAddr(bl_flash_sectorOfAddr(APP_START_ADDRESS + appHeader.size)) -
            (APP_START_ADDRESS + appHeader.size);

        uint32_t eccFixBytesLeft = eccFixTotalBytes;
        while (eccFixBytesLeft > 0) {
          const uint32_t numBytesToWrite = (eccFixBytesLeft > 128U) ? 128U : eccFixBytesLeft;

          const uint32_t baseAddr = APP_START_ADDRESS + appHeader.size + 1U;
          const uint32_t addr = baseAddr + (eccFixTotalBytes - eccFixBytesLeft);

          bl_flash_FapiBlockWrite(addr, (uint32_t)eccFixWriteBuf, numBytesToWrite);

          eccFixBytesLeft -= numBytesToWrite;
        }

        if (state == BL_STATE_IDLE) {
          break;
        }

        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Finished writing to flash\r\n"),
                           (uint8_t *)"Finished writing to flash\r\n");

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_ERASE_IMAGE: {
        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("NOT IMPLEMENTED\r\n"), (uint8_t *)"NOT IMPLEMENTED\r\n");

        // TODO: Erase entire application space

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_RUN_APP: {
        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Running application\r\n"), (uint8_t *)"Running application\r\n");

        // Go to the application's entry point
        uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
        ((appStartFunc_t)appStartAddress)();

        bl_uart_writeBytes(BL_UART_SCIREG_1, strlen("Failed to run application\r\n"),
                           (uint8_t *)"Failed to run application\r\n");

        // TODO: Restart device if application fails to run or returns

        break;
      }
    }
  }
}
