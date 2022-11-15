#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sci.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX_PRINTF_SIZE 128U

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

uint8_t sciPrintText(unsigned char *text, uint32_t length) {
    /* initSciMutex must be called before printing is allowed */
    ASSERT(sciMutex != NULL && sciLinMutex != NULL);

    if (UART_PRINT_REG == scilinREG) {
        if (sciLinMutex != NULL) {
            if (xSemaphoreTake(sciLinMutex, portMAX_DELAY) == pdTRUE) {
                sciSendBytes(text, length);
                xSemaphoreGive(sciLinMutex);
                return 1;
            }
        }
    } else if (UART_PRINT_REG == sciREG) {
        if (sciMutex != NULL) {
            if (xSemaphoreTake(sciMutex, portMAX_DELAY) == pdTRUE) {
                sciSendBytes(text, length);
                xSemaphoreGive(sciMutex);
                return 1;
            }
        }
    }
    return 0;
}

static void sciSendBytes(unsigned char *bytes, uint32_t length) {
    for (int i = 0; i < length; i++) {
        if (bytes[i] == '\0')
            return;
        // sciSendByte waits for the transmit buffer to be empty before sending
        sciSendByte(UART_PRINT_REG, bytes[i]);
    }
}

uint8_t sciPrintf(const char *s, ...){
    if (s == NULL){
        return 0;
    }

    va_list args;
    va_start(args, s);

    char buf[MAX_PRINTF_SIZE];
    int8_t n = vsnprintf(buf, MAX_PRINTF_SIZE, s, args);

    // n == MAX_PRINTF_SIZE invalid because null character isn't included in count
    if (n < 0 || n >= MAX_PRINTF_SIZE){
        // log an error message here
        return 0;
    }

    uint8_t ret = sciPrintText((unsigned char *)buf, MAX_PRINTF_SIZE);
    return ret;
}

void uartAssertFailed(char *file, int line, char *expr){
    sciPrintf("ASSERTION FAILED: %s, file %s, line %d\r\n",
                            expr, file, line);
}
