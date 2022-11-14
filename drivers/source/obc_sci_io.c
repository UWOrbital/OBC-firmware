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
 * @brief Iterate through an array of bytes and transmit them via SCI or SCI2.
 * 
 * @param sci The SCI register to use (sciREG or scilinREG).
 * @param bytes The array of bytes to transmit.
 * @param length The length of the array of bytes to transmit.
 */
static void sciSendBytes(sciBASE_t *sci, unsigned char *bytes, uint32_t length);

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

uint8_t printTextSci(sciBASE_t *sci, unsigned char *text, uint32_t length) {
    /* initSciMutex must be called before printing is allowed */
    ASSERT(sciMutex != NULL && sciLinMutex != NULL);

    if (sci == scilinREG) {
        if (sciLinMutex != NULL) {
            if (xSemaphoreTake(sciLinMutex, portMAX_DELAY) == pdTRUE) {
                sciSendBytes(sci, text, length);
                xSemaphoreGive(sciLinMutex);
                return 1;
            }
        }
    } else if (sci == sciREG) {
        if (sciMutex != NULL) {
            if (xSemaphoreTake(sciMutex, portMAX_DELAY) == pdTRUE) {
                sciSendBytes(sci, text, length);
                xSemaphoreGive(sciMutex);
                return 1;
            }
        }
    }
    return 0;
}

static void sciSendBytes(sciBASE_t *sci, unsigned char *bytes, uint32_t length) {
    for (int i = 0; i < length; i++) {
        if (bytes[i] == '\0')
            return;
        // sciSendByte waits for the transmit buffer to be empty before sending
        sciSendByte(sci, bytes[i]);
    }
}

uint8_t sciPrintf(sciBASE_t *sci, const char *s, ...){
    if (sci == NULL || s == NULL){
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

    uint8_t ret = printTextSci(sci, (unsigned char *)buf, MAX_PRINTF_SIZE);
    return ret;
}

void uartAssertFailed(char *file, int line, char *expr){
    sciPrintf(scilinREG, "ASSERTION FAILED: %s, file %s, line %d\r\n",
                            expr, file, line);
}
