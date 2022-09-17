#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>

#include <sci.h>

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
        // sciSendByte waits for the transmit buffer to be empty before sending
        sciSendByte(sci, bytes[i]);
    }
}