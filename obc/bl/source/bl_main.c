#include "bl_config.h"
#include "bl_flash.h"

#include "sys_core.h"
#include "sci.h"

#include "F021.h"

extern uint32_t __flashApiLoadStart__;
extern uint32_t __flashApiSize__;

extern uint32_t __flashApiRunStart__;
extern uint32_t __flashApiRunEnd__;

typedef void (*appStartFunc_t)(void);

typedef struct {
  uint32_t version;
  uint32_t size;
  uint32_t checksum;
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

char readChar(void) { return sciReceiveByte(scilinREG); }

int main(void) {
  _coreDisableFlashEcc_();  // TODO: Remove if unnecessary

  sciInit();

  uint32_t flashApiSize = (uint32_t)&__flashApiRunEnd__ - (uint32_t)&__flashApiRunStart__;
  if (flashApiSize != __flashApiSize__) {
    resetDevice();
  }

  // Flash API cannot be executed from the same flash bank it is modifying. So
  // copy it to RAM and execute it from there.
  memcpy((void *)&__flashApiRunStart__, (void *)&__flashApiLoadStart__, flashApiSize);

  // TODO: Remove later
  char str[] = "Hello from BL\r\n";
  sciSend(scilinREG, sizeof(str), (uint8_t *)str);

  bl_state_t state = BL_STATE_IDLE;

  while (1) {
    switch (state) {
      case BL_STATE_IDLE: {
        char c = readChar();

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
        uint8_t recvBuffer[sizeof(app_header_t)] = {0};

        for (uint32_t i = 0; i < sizeof(app_header_t); i++) {
          recvBuffer[i] = (uint8_t)readChar();
        }

        app_header_t appHeader = {0};
        memcpy((void *)&appHeader, (void *)recvBuffer, sizeof(app_header_t));

        // Check that the application image is not too large
        if (appHeader.size > MAX_APP_SIZE_BYTES) {
          state = BL_STATE_IDLE;
          break;
        }

        // TODO: Store app header in persistent storage
        // FApi_BlockErase(currAppHeader.startAddr, currAppHeader.size);
        Fapi_BlockErase(APP_START_ADDRESS, MAX_APP_SIZE_BYTES);

        for (uint32_t i = 0; i < appHeader.size; i++) {
          // TODO: Write application to external storage so we can verify it
          // before writing it to flash

          uint32_t rxChar = (uint32_t)readChar();
          // TODO: Maybe flash in larger chunks
          Fapi_BlockProgram(APP_START_ADDRESS + i, (uint32_t)&rxChar, 1);
        }

        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_ERASE_IMAGE: {
        // TODO: Get the header from flash and erase the image
        Fapi_BlockErase(APP_START_ADDRESS, MAX_APP_SIZE_BYTES);
        state = BL_STATE_IDLE;
        break;
      }
      case BL_STATE_RUN_APP: {
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
