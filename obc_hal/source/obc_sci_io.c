#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_semphr.h>

#include <sci.h>

static SemaphoreHandle_t sciMutex = NULL;
static SemaphoreHandle_t sciLinMutex = NULL;

void initSciMutex(void) {
    if (sciMutex == NULL) {
        sciLinMutex = xSemaphoreCreateMutex();
    }
    if (sciLinMutex == NULL) {
        sciLinMutex = xSemaphoreCreateMutex();
    }
}

uint8_t printTextSci(sciBASE_t *sci, unsigned char *text, uint32_t length) {
    if (sci == scilinREG) {
        if (sciLinMutex != NULL) {
            if (xSemaphoreTake(sciLinMutex, portMAX_DELAY) == pdTRUE) {
                while (length--) {
                    while ((sci->FLR & 0x4) == 4);
                    sciSendByte(sci, *text++);
                }
                xSemaphoreGive(sciLinMutex);
                return 1;
            }
        }
    } else if (sci == sciREG) {
        if (sciMutex != NULL) {
            if (xSemaphoreTake(sciMutex, portMAX_DELAY) == pdTRUE) {
                while (length--) {
                    while ((sci->FLR & 0x4) == 4);
                    sciSendByte(sci, *text++);
                }
                xSemaphoreGive(sciMutex);
                return 1;
            }
        }
    }
    return 0;
}