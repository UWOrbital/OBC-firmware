#include "obc_sci_io.h"
#include "obc_print.h"
#include "obc_errors.h"

#include <gio.h>
#include <sci.h>

#define NUM_CHARS_TO_READ 20U

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

int main(void) {
  // Initialize hardware.
  gioInit();
  sciInit();

  // Initialize the SCI mutex.
  initSciPrint();

  sciPrintf("Demo started\r\n");

  while (1) {
    unsigned char buffer[NUM_CHARS_TO_READ] = {'\0'};
    obc_error_code_t error =
        sciReadBytes(buffer, NUM_CHARS_TO_READ, UART_MUTEX_BLOCK_TIME, pdMS_TO_TICKS(10), UART_PRINT_REG);
    if (error != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error reading from SCI! - %d\r\n", (int)error);
      continue;
    }

    sciPrintText(buffer, NUM_CHARS_TO_READ, UART_MUTEX_BLOCK_TIME);
    sciPrintf("\r\n");

    // Toggle the LED.
    gioToggleBit(gioPORTB, 1);

    // Simple delay.
    for (int i = 0; i < 1000000; i++) {
      // Do nothing.
    }
  }

  return 0;
}
