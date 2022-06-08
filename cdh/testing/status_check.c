#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "console.h"

#define BIT_0	(1<< 0)
#define BIT_1	(1<< 1)

static void collect();

void status_check_main() {
    xTaskCreate(collect, "collect", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
}

void collect() {
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    for(;;) {
        console_print("COLLECT\n");
        vTaskDelay(xDelay);
    }
}
