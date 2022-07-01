/**
 * @file param_manager.c
 * @author Daniel Gobalakrishnan
 * @date 2022-07-01
 */
#include "obc_sci_comms.h"

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

uint8_t sci_send_text(sciBASE_t *sci, uint8_t *text, uint32_t length) {
    if (sciCommMutex != NULL) {
        if (xSemaphoreTake(sciCommMutex, portMAX_DELAY) == pdTRUE) {
            while (length--) {
                while ((sci->FLR & 0x4) == 4);
                sciSendByte(sci, *text++);
            }
            xSemaphoreGive(sciCommMutex);
            return 1;
        }
    }
    return 0;
}