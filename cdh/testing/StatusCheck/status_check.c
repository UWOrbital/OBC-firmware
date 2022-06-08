#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "console.h"
#include "param_manager.h"

static void collect();

void status_check_main() {
    xTaskCreate(collect, "collect", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
}

void collect() {
    const TickType_t xDelay = 5000/portTICK_PERIOD_MS;
    uint8_t x = 0;
    uint8_t a;
    TickType_t lastTick = xTaskGetTickCount();
    for(;;) {
        vTaskDelayUntil(&lastTick, xDelay);
        a = get_param_val(TEST_PARAM1, UINT8_PARAM, &x);
        if (a == 0) {
            console_print("Error getting TEST_PARAM1\n");
        } else {
            console_print("COLLECTED TEST_PARAM1: %d\n", x);
        }
    }
}
