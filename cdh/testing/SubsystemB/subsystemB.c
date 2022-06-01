#include <stddef.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"

#include "subsystemB.h"
#include "console.h"

static char c = 'a';
static SemaphoreHandle_t xSemaphore = NULL;

void update_Bdata() {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    for(;;) {
        console_print("Updating data\n" );
        if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {
            c = (c+1-'a') % 26 + 'a';
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(xDelay);
    }
}

void subsystemB_main() {
    xSemaphore = xSemaphoreCreateMutex();    
    xTaskCreate(update_Bdata, "updateData", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
}

void subsystemB_sender(void *Bqueue) {
    QueueHandle_t *queue = (QueueHandle_t *) Bqueue;
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //portMAX_DELAY should block indefinitely assuming the INCLUDE_vTaskSuspend flag is 1 in the config
        if(xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {
            xQueueSend(*queue, (void*) &c, 10);
            xSemaphoreGive(xSemaphore);
        }
    }
}

