#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "subsystemA.h"
#include "console.h"

static void collect();

static TaskHandle_t Asender;
static QueueHandle_t Aqueue;

void start_collection() {
    Aqueue = xQueueCreate(1, sizeof(int));
    xTaskCreate(collect, "tmCollect", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    xTaskCreate(subsystemA_sender, "Asender", configMINIMAL_STACK_SIZE, (void*)&Aqueue, tskIDLE_PRIORITY+2, &Asender);
}

static void collect() {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    int x = 0;
    for(;;) {
        xTaskNotifyGive(Asender);
        xQueueReceive(Aqueue, (void*) &x, 10);
        console_print("Subsystem A telemetry: %d\n", x);
        vTaskDelay(xDelay);
    }
}
