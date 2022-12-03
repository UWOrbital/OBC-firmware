#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sci.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX_PRINTF_SIZE 128U
#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static SemaphoreHandle_t sciMutex = NULL;
static StaticSemaphore_t sciMutexBuffer;
static SemaphoreHandle_t sciLinMutex = NULL;
static StaticSemaphore_t sciLinMutexBuffer;

/**
 * @brief Iterate through an array of bytes and transmit them via UART_PRINT_REG.
 * 
 * @param bytes The array of bytes to transmit.
 * @param length The length of the array of bytes to transmit.
 */
static void sciSendBytes(unsigned char *bytes, uint32_t length);

void initSciMutex(void) {
    if (sciMutex == NULL) {
        sciMutex = xSemaphoreCreateMutexStatic(&sciMutexBuffer);
    }
    if (sciLinMutex == NULL) {
        sciLinMutex = xSemaphoreCreateMutexStatic(&sciLinMutexBuffer);
    }

    configASSERT(sciMutex);
    configASSERT(sciLinMutex);
}

obc_error_code_t sciPrintText(unsigned char *text, uint32_t length) {
    configASSERT((UART_PRINT_REG == sciREG) || (UART_PRINT_REG == scilinREG));

    if (text == NULL || length == 0)
        return OBC_ERR_CODE_INVALID_ARG;

    SemaphoreHandle_t mutex = (UART_PRINT_REG == sciREG) ? sciMutex : sciLinMutex;
    configASSERT(mutex);

    if (xSemaphoreTake(mutex, UART_MUTEX_BLOCK_TIME) == pdTRUE){
        sciSendBytes(text, length);
        xSemaphoreGive(mutex);
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_MUTEX_TIMEOUT;
}

static void sciSendBytes(unsigned char *bytes, uint32_t length) {
    if (bytes == NULL || length == 0)
        return; // Private function, so we can assume that the arguments are valid
    
    for (int i = 0; i < length; i++) {
        if (bytes[i] == '\0')
            return;
        // sciSendByte waits for the transmit buffer to be empty before sending
        sciSendByte(UART_PRINT_REG, bytes[i]);
    }
}

obc_error_code_t sciPrintf(const char *s, ...){
    if (s == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    va_list args;
    va_start(args, s);

    char buf[MAX_PRINTF_SIZE];
    int8_t n = vsnprintf(buf, MAX_PRINTF_SIZE, s, args);

    // n == MAX_PRINTF_SIZE invalid because null character isn't included in count
    if (n < 0 || n >= MAX_PRINTF_SIZE)
        return OBC_ERR_CODE_INVALID_ARG;

    return sciPrintText((unsigned char *)buf, MAX_PRINTF_SIZE);
}

void uartAssertFailed(char *file, int line, char *expr) {
    if (file == NULL || line < 0 || expr == NULL)
        return; // Only called by assert, so we can assume that the arguments are valid
    
    sciPrintf("ASSERTION FAILED: %s, file %s, line %d\r\n",
                            expr, file, line);
}
