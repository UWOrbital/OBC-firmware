#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include "subsystemB.h"
#include "console.h"

#define BIT_0	( 1 << 0 )
#define BIT_1	( 1 << 1 )

static void collectB();

static TaskHandle_t Bcollector;
static TaskHandle_t Bsender;
static QueueHandle_t Bqueue;
static EventGroupHandle_t *receiving_eventGroup;
static EventGroupHandle_t *sending_eventGroup;

TaskHandle_t* start_B_collection(EventGroupHandle_t *eventGroup1, EventGroupHandle_t *eventGroup2) {
    receiving_eventGroup = eventGroup1;
    sending_eventGroup = eventGroup2;
    Bqueue = xQueueCreate(1, sizeof(int));
    xTaskCreate(collectB, "tmCollect", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &Bcollector);
    xTaskCreate(subsystemB_sender, "Bsender", configMINIMAL_STACK_SIZE, (void*)&Bqueue, tskIDLE_PRIORITY+2, &Bsender);
    return &Bcollector;
}

static void collectB() {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    int x = 0;
    for(;;) {
        xEventGroupWaitBits(*receiving_eventGroup, BIT_1, pdTRUE, pdFALSE, portMAX_DELAY);
        xTaskNotifyGive(Bsender);
        xQueueReceive(Bqueue, (void*) &x, 10);
        xEventGroupSetBits(*sending_eventGroup, BIT_1);
        console_print("Subsystem B telemetry: %c\n", x);
    }
}
