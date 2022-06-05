#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "console.h"

#define BIT_0	(1<< 0)
#define BIT_1	(1<< 1)

extern TaskHandle_t* start_A_collection();
extern TaskHandle_t* start_B_collection();

static void collect();

static EventGroupHandle_t sending_eventGroup;
static EventGroupHandle_t receiving_eventGroup;

void status_check_main() {
    sending_eventGroup = xEventGroupCreate();
    receiving_eventGroup = xEventGroupCreate();

    TaskHandle_t *Acollector = start_A_collection(&sending_eventGroup, &receiving_eventGroup);
    TaskHandle_t *Bcollector = start_B_collection(&sending_eventGroup, &receiving_eventGroup);

    xTaskCreate(collect, "collect", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
}

void collect() {
    const TickType_t xDelay = 5000/portTICK_PERIOD_MS;
    const TickType_t waitTicks = 10/portTICK_PERIOD_MS;
    for(;;) {
        xEventGroupSetBits(sending_eventGroup, BIT_0 | BIT_1);
        console_print("COLLECT\n");
        EventBits_t bits = xEventGroupWaitBits(receiving_eventGroup, BIT_0 | BIT_1, pdTRUE, pdTRUE, waitTicks);
        console_print("bits: %d\n", (int) bits);
        vTaskDelay(xDelay);
    }
}
