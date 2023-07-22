#include "sys_core.h"
#include "bl_config.h"
#include "sci.h"

uint32_t g_ulTransferAddress;
uint32_t g_ulTransferSize;

typedef void (*appStartFunc_t)(void);

int main(void) {
  _coreDisableFlashEcc_();

  sciInit();

  char str[] = "Hello, BL\r\n";
  sciSend(scilinREG, sizeof(str), (uint8 *)str);

  g_ulTransferAddress = (uint32_t)APP_START_ADDRESS;
  ((appStartFunc_t)g_ulTransferAddress)();

  /* Should never get here, but just in case ... */

  while (1)
    ;
}
