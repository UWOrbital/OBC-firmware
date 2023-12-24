#include "bl_config.h"
#include "bl_flash.h"
#include "bl_uart.h"

#include "sci.h"  // TODO: Remove once sciSend is moved to bl_uart

#include <stdio.h>
#include <string.h>

extern uint32_t __flashApiLoadStart__;
extern uint32_t __flashApiSize__;

extern uint32_t __flashApiRunStart__;
extern uint32_t __flashApiRunEnd__;

typedef void (*appStartFunc_t)(void);

typedef struct {
  uint32_t version;
  uint32_t size;
  // uint32_t checksum;
  // uint32_t startAddr;
} app_header_t;

typedef enum {
  BL_STATE_IDLE,
  BL_STATE_DOWNLOAD_IMAGE,
  BL_STATE_ERASE_IMAGE,
  BL_STATE_RUN_APP,
} bl_state_t;

// TODO List:
// - [ ] Mechanism to verify the application image before/after writing it to flash
// - [ ] Abstract away the PHY layer used to communicate with the host (UART or SPI)
// - [ ] Store app header in flash before app
// - [ ] Improve transfer protocol between host and bootloader (speed and reliability)

void bootloader(void) {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;

  sciSend(sciREG, strlen("Hello from BL\r\n"), (uint8_t *)"Hello from BL\r\n");

  bl_state_t state = BL_STATE_IDLE;

  while (1) {
    switch (state) {
      case BL_STATE_IDLE: {
        sciSend(sciREG, strlen("Waiting for input\r\n"), (uint8_t *)"Waiting for input\r\n");

        char c = '0';
        bl_uart_readBytes(scilinREG, (uint8_t *)&c, 1U);

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
        sciSend(sciREG, strlen("Downloading application\r\n"), (uint8_t *)"Downloading application\r\n");

        uint8_t recvBuffer[sizeof(app_header_t)] = {0U};

        if (bl_uart_readBytes(scilinREG, recvBuffer, sizeof(app_header_t)) != BL_ERR_CODE_SUCCESS) {
          sciSend(sciREG, strlen("Failed to read header\r\n"), (uint8_t *)"Failed to read header\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        app_header_t appHeader = {0};
        memcpy((void *)&appHeader, (void *)recvBuffer, sizeof(app_header_t));

        if (appHeader.size == 0U) {
          sciSend(sciREG, strlen("Invalid image size\r\n"), (uint8_t *)"Invalid image size\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        if (!bl_flash_isStartAddrValid(APP_START_ADDRESS, appHeader.size)) {
          sciSend(sciREG, strlen("Invalid start address\r\n"), (uint8_t *)"Invalid start address\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        sciSend(sciREG, strlen("Received header\r\n"), (uint8_t *)"Received header\r\n");

        errCode = bl_flash_FapiInitBank(0U);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          sciSend(sciREG, strlen("Failed to init flash\r\n"), (uint8_t *)"Failed to init flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        errCode = bl_flash_FapiBlockErase(APP_START_ADDRESS, appHeader.size);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          sciSend(sciREG, strlen("Failed to erase flash\r\n"), (uint8_t *)"Failed to erase flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        sciSend(sciREG, strlen("Erased flash\r\n"), (uint8_t *)"Erased flash\r\n");

        // Host will send a 'D' before sending the image
        while (1) {
          char waitChar = '0';

          if (bl_uart_readBytes(scilinREG, (uint8_t *)&waitChar, 1U) != BL_ERR_CODE_SUCCESS) {
            continue;
          }

          if (waitChar == 'D') {
            break;
          }
        }

        bl_flash_FapiInitBank(0U);

        // Receive image in chunks of 128 bytes and write to flash. Then flash the remaining bytes
        uint32_t numBytesToFlash = appHeader.size;
        while (numBytesToFlash > 0) {
          uint8_t recvBuffer[128] = {0U};

          uint32_t numBytesToRead = (numBytesToFlash > 128) ? 128 : numBytesToFlash;

          if (bl_uart_readBytes(scilinREG, recvBuffer, numBytesToRead) != BL_ERR_CODE_SUCCESS) {
            sciSend(sciREG, strlen("Failed to read bytes\r\n"), (uint8_t *)"Failed to read bytes\r\n");
            state = BL_STATE_IDLE;
            break;
          }

          bl_flash_FapiBlockWrite(APP_START_ADDRESS + (appHeader.size - numBytesToFlash), (uint32_t)recvBuffer,
                                  numBytesToRead);

          numBytesToFlash -= numBytesToRead;
        }

        sciSend(sciREG, strlen("Wrote application\r\n"), (uint8_t *)"Wrote application\r\n");

        // Write msg that we're fixing incorrect ECC due to erase
        sciSend(sciREG, strlen("Fixing ECC\r\n"), (uint8_t *)"Fixing ECC\r\n");

        // Go through the rest of the final flash section we erased and write 0xFF in blocks of 128 with ECC
        // auto-calculation
        uint8_t eccFixBuf[128] = {0U};
        memset(eccFixBuf, 0xFFU, sizeof(eccFixBuf));

        const uint32_t numBytesToFix =
            bl_flash_sectorEndAddr(bl_flash_sectorOfAddr(APP_START_ADDRESS + appHeader.size)) -
            (APP_START_ADDRESS + appHeader.size);
        numBytesToFlash = numBytesToFix;
        while (numBytesToFlash > 0) {
          uint32_t numBytesToWrite = (numBytesToFlash > 128U) ? 128U : numBytesToFlash;

          const uint32_t baseAddr = APP_START_ADDRESS + appHeader.size + 1U;
          const uint32_t addr = baseAddr + (numBytesToFix - numBytesToFlash);

          bl_flash_FapiBlockWrite(addr, (uint32_t)eccFixBuf, numBytesToWrite);

          numBytesToFlash -= numBytesToWrite;
        }

        if (state == BL_STATE_IDLE) {
          break;
        }

        sciSend(sciREG, strlen("Finished writing to flash\r\n"), (uint8_t *)"Finished writing to flash\r\n");

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_ERASE_IMAGE: {
        sciSend(sciREG, strlen("Erasing application\r\n"), (uint8_t *)"Erasing application\r\n");

        // TODO: Erase entire application space
        // Fapi_BlockErase(APP_START_ADDRESS, MAX_APP_SIZE_BYTES);
        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_RUN_APP: {
        sciSend(sciREG, strlen("Running application\r\n"), (uint8_t *)"Running application\r\n");

        // Go to the application's entry point
        uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
        ((appStartFunc_t)appStartAddress)();

        sciSend(sciREG, strlen("Failed to run application\r\n"), (uint8_t *)"Failed to run application\r\n");

        // resetDevice();

        /* Should never get here, but just in case ... */
        while (1) {
          // Do nothing
        }

        break;
      }
    }
  }
}

int main(void) {
  bl_uart_init();

  // TODO: Rename .flashApi section since I also put bl_flash in there

  // Flash API cannot be executed from the same flash bank it is modifying. So
  // copy it to RAM and execute it from there.
  memcpy(&__flashApiRunStart__, &__flashApiLoadStart__, (uint32_t)&__flashApiSize__);

  bootloader();

  while (1) {
    // Should never get here
  }
}
