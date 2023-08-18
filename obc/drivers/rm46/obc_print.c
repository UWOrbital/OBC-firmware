#include "obc_sci_io.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"

#include <sci.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_PRINTF_SIZE 128U

obc_error_code_t sciPrintText(unsigned char *text, uint32_t length, TickType_t uartMutexTimeoutTicks) {
 
  if (text == NULL || length == 0) return OBC_ERR_CODE_INVALID_ARG;

    obc_error_code_t err = sciSendBytes(text, length, uartMutexTimeoutTicks, UART_PRINT_REG);
    if (err != OBC_ERR_CODE_SUCCESS) {
      LOG_ERROR_CODE(err);
    }
    return err;
}

obc_error_code_t sciPrintf(const char *s, ...) {
  if (s == NULL) return OBC_ERR_CODE_INVALID_ARG;

  char buf[MAX_PRINTF_SIZE] = {0};

  va_list args;
  va_start(args, s);
  int n = vsnprintf(buf, MAX_PRINTF_SIZE, s, args);
  va_end(args);

  if (n < 0) return OBC_ERR_CODE_INVALID_ARG;

  // n == MAX_PRINTF_SIZE invalid because null character isn't included in count
  if ((uint32_t)n >= MAX_PRINTF_SIZE) return OBC_ERR_CODE_INVALID_ARG;

  return sciPrintText((unsigned char *)buf, MAX_PRINTF_SIZE, UART_MUTEX_BLOCK_TIME);
}

void uartAssertFailed(char *file, int line, char *expr) {
  if (file == NULL || line < 0 || expr == NULL)
    return;  // Only called by assert, so we can assume that the arguments are valid

  sciPrintf("ASSERTION FAILED: %s, file %s, line %d\r\n", expr, file, line);
}