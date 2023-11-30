#include "bl_config.h"
#include "bl_flash.h"
#include "bl_uart.h"

#include "sys_core.h"
#include "sci.h"
#include "gio.h"

#include "F021.h"

#include <stdio.h>

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

// Goal: Have a bootloader that can be used to update the application image via UART
// We can update this for OTA updates in the future

// TODO List:
// - [ ] Write small application to convert .out to .bin and send it to the RM46 (with header data)
//        - https://software-dl.ti.com/ccs/non-esd/releases/other/applications_packages/cg_xml/index.htm
// - [ ] Single linker script for all boards (I've only updated the launchpad one)
// - [ ] Remove BL HAL (we only need the bl linker script).
// - [ ] Clean up the build process (it's a mess right now)
// - [ ] Mechanism to get the application image from the host (for now, just send it over UART)
// - [ ] Mechanism to write the application image to flash)
// - [ ] Mechanism to verify the application image
// - [ ] State machine to handle the above
// - [ ] Ensure application memory space does not conflict with the bootloader
// - [ ] Abstract away the PHY layer used to communicate with the host (UART or SPI)
// - [ ] Store image header in flash (or maybe external storage)
// - [ ] Clean up the code
// - [ ] Handle warnings related to Flash API lib

// The following functions are already in obc/drivers, but I don't wanna update the
// the build process rn, so here are minimal implementations of them
void resetDevice(void) {
  // TODO: Reset the device
  while (1)
    ;
}

void bootloader(void) {
  bl_error_code_t errCode = BL_ERR_CODE_SUCCESS;
  (void)errCode;
  // TODO: Remove later
  char str[] = "Hello from BL\r\n";
  sciSend(sciREG, sizeof(str), (uint8_t *)str);

  bl_state_t state = BL_STATE_IDLE;

  while (1) {
    switch (state) {
      case BL_STATE_IDLE: {
        sciSend(sciREG, 18U, (uint8_t *)"Waiting for input\r\n");

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
        sciSend(sciREG, 26U, (uint8_t *)"Downloading application\r\n");

        uint8_t recvBuffer[sizeof(app_header_t)] = {0U};

        if (bl_uart_readBytes(scilinREG, recvBuffer, sizeof(app_header_t)) != BL_ERR_CODE_SUCCESS) {
          sciSend(sciREG, 22U, (uint8_t *)"Failed to read header\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        app_header_t appHeader = {0};
        memcpy((void *)&appHeader, (void *)recvBuffer, sizeof(app_header_t));

        if (!bl_flash_isStartAddrValid(APP_START_ADDRESS, appHeader.size)) {
          sciSend(sciREG, 22U, (uint8_t *)"Invalid start address\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        // char headerOutStr[100] = {'0'};
        // sprintf(headerOutStr, "Version: %lu\r\nSize: %lu\r\n", appHeader.version, appHeader.size);
        // sciSend(sciREG, strlen(headerOutStr), (uint8_t *)headerOutStr);

        // Check that the application image is not too large
        // if (appHeader.size > MAX_APP_SIZE_BYTES) {
        //   state = BL_STATE_IDLE;
        //   break;
        // }

        const char *ackStr = "ACK\r\n";
        sciSend(sciREG, strlen(ackStr), (uint8_t *)ackStr);

        errCode = bl_flash_FapiInitBank(0U);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          sciSend(sciREG, 22U, (uint8_t *)"Failed to init flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        errCode = bl_flash_FapiBlockErase(APP_START_ADDRESS, appHeader.size);
        if (errCode != BL_ERR_CODE_SUCCESS) {
          sciSend(sciREG, 22U, (uint8_t *)"Failed to erase flash\r\n");
          state = BL_STATE_IDLE;
          break;
        }

        char waitChar = '0';
        while (1) {
          if (bl_uart_readBytes(scilinREG, (uint8_t *)&waitChar, 1U) != BL_ERR_CODE_SUCCESS) {
            continue;
          }

          if (waitChar == 'D') {
            break;
          }
        }

        bl_flash_FapiInitBank(0U);

        // for (uint32_t i = 0; i < appHeader.size; i++) {
        for (uint32_t i = 0; i < 12; i++) {
          uint8_t rxChar;
          if (bl_uart_readBytes(scilinREG, &rxChar, 1U) != BL_ERR_CODE_SUCCESS) {
            state = BL_STATE_IDLE;
            break;
          }

          // print rxchar in hex
          char hexStr[5] = {'0'};
          sprintf(hexStr, "%02X\r\n", rxChar);
          sciSend(sciREG, 5U, (uint8_t *)hexStr);

          // TODO: Maybe flash in larger chunks
          bl_flash_FapiBlockWrite(APP_START_ADDRESS + i, (uint32_t)&rxChar, 1);

          if (i % 1000 == 0) {
            gioToggleBit(gioPORTB, 1);
          }
        }

        sciSend(sciREG, 22U, (uint8_t *)"Verifying application\r\n");

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_ERASE_IMAGE: {
        sciSend(sciREG, 22U, (uint8_t *)"Erasing application\r\n");

        // TODO: Get the header from flash and erase the image
        // Fapi_BlockErase(APP_START_ADDRESS, MAX_APP_SIZE_BYTES);
        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_RUN_APP: {
        sciSend(sciREG, 18U, (uint8_t *)"Running application\r\n");

        // Go to the application's entry point
        uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
        ((appStartFunc_t)appStartAddress)();

        // If we get here, the application has returned. Reset the device.
        resetDevice();

        /* Should never get here, but just in case ... */
        while (1)
          ;
        break;
      }
    }
  }
}

extern BaseType_t prvRaisePrivilege(void);

int main(void) {
  // _coreDisableFlashEcc_();  // TODO: Remove if unnecessary
  // _coreDisableRamEcc_(); // TODO: Remove if unnecessary

  bl_uart_init();
  gioInit();

  prvRaisePrivilege();

  // Flash API cannot be executed from the same flash bank it is modifying. So
  // copy it to RAM and execute it from there.
  memcpy(&__flashApiRunStart__, &__flashApiLoadStart__, (uint32_t)&__flashApiSize__);

  bootloader();

  while (1)
    ;
}
