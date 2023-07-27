#include "bl_config.h"

#include "sys_core.h"
#include "sci.h"

#include "F021.h"

extern uint32_t __flashApiLoadStart__;
extern uint32_t __flashApiSize__;

extern uint32_t __flashApiRunStart__;
extern uint32_t __flashApiRunEnd__;

typedef void (*appStartFunc_t)(void);

// TODO List:
// - [ ] Mechanism to get the application image from the host
// - [ ] Mechanism to verify the application image
// - [ ] Mechanism to write the application image to flash
// - [ ] State machine to handle the above
// - [ ] Ensure application memory space does not conflict with the bootloader

int main(void) {
  _coreDisableFlashEcc_();

  sciInit();

  uint32_t flashApiSize = (uint32_t)&__flashApiRunEnd__ - (uint32_t)&__flashApiRunStart__;
  if (flashApiSize != __flashApiSize__) {
    while (1)
      ;
  }

  if (0) {
    // Flash API cannot be executed from the same flash bank it is modifying. So
    // copy it to RAM and execute it from there.
    memcpy((void *)&__flashApiRunStart__, (void *)&__flashApiLoadStart__, flashApiSize);
  }

  char str[] = "Hello, BL\r\n";
  sciSend(scilinREG, sizeof(str), (uint8 *)str);

  // Go to the application's entry point
  uint32_t appStartAddress = (uint32_t)APP_START_ADDRESS;
  ((appStartFunc_t)appStartAddress)();

  // TODO: restart the device if the application returns

  /* Should never get here, but just in case ... */
  while (1) {
  }
}
