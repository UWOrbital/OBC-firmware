#include <stddef.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"

#include "subsystemA.h"
#include "console.h"
#include "param_manager.h"

static int c = 0;
static SemaphoreHandle_t xSemaphore = NULL;

void update_data() {
    set_param_val(ALTITUDE, UINT8_PARAM, &c);
}

void subsystemA_main() {
    xSemaphore = xSemaphoreCreateMutex();    
    xTaskCreate(update_data, "updateData", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
}

void subsystemA_sender(void *Aqueue) {
    QueueHandle_t *queue = (QueueHandle_t *) Aqueue;
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //portMAX_DELAY should block indefinitely assuming the INCLUDE_vTaskSuspend flag is 1 in the config
        if(xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {
            xQueueSend(*queue, (void*) &c, 10);
            xSemaphoreGive(xSemaphore);
        }
    }
}

