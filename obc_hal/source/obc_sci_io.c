/**
 * @file obc_sci_io.c
 * @author Daniel Gobalakrishnan
 * @date 2022-07-01
 */
#include "obc_sci_io.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"

#include "sci.h"

static SemaphoreHandle_t sciCommMutex = NULL;

void sci_mutex_init(void) {
    if (sciCommMutex == NULL) {
        sciCommMutex = xSemaphoreCreateMutex();
    }
}

uint8_t sci_print_text(uint8_t *text, uint32_t length) {
    if (sciCommMutex != NULL) {
        if (xSemaphoreTake(sciCommMutex, portMAX_DELAY) == pdTRUE) {
            while (length--) {
                while ((scilinREG->FLR & 0x4) == 4);
                sciSendByte(scilinREG, *text++);
            }
            xSemaphoreGive(sciCommMutex);
            return 1;
        }
    }
    return 0;
}