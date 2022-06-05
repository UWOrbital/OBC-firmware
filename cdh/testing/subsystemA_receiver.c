#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include "subsystemA.h"
#include "console.h"

#define BIT_0	( 1 << 0 )

static void collectA();

static TaskHandle_t Acollector;
static TaskHandle_t Asender;
static QueueHandle_t Aqueue;
static EventGroupHandle_t *receiving_eventGroup;
static EventGroupHandle_t *sending_eventGroup;

TaskHandle_t* start_A_collection(EventGroupHandle_t *eventGroup1, EventGroupHandle_t *eventGroup2) {
    receiving_eventGroup = eventGroup1;
    sending_eventGroup = eventGroup2;
    Aqueue = xQueueCreate(1, sizeof(int));
    xTaskCreate(collectA, "tmCollect", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &Acollector);
    xTaskCreate(subsystemA_sender, "Asender", configMINIMAL_STACK_SIZE, (void*)&Aqueue, tskIDLE_PRIORITY+2, &Asender);
    return &Acollector;
}

static void collectA() {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    int x = 0;
    for(;;) {
        xEventGroupWaitBits(*receiving_eventGroup, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY);
        xTaskNotifyGive(Asender);
        xQueueReceive(Aqueue, (void*) &x, 10);
        xEventGroupSetBits(*sending_eventGroup, BIT_0);
        console_print("Subsystem A telemetry: %d\n", x);
    }
}
