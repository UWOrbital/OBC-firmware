#include <stddef.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"

#include "subsystemA.h"
#include "console.h"
#include "param_manager.h"

static uint8_t x = 0;

void update_data() {
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    uint8_t a;
    for(;;) {
        vTaskDelay(xDelay);
        ++x;
        a = set_param_val(TEST_PARAM1, UINT8_PARAM, &x);
        if (a == 0) {
            console_print("Error setting TEST_PARAM1\n");
        } else {
            console_print("Set TEST_PARAM %d\n", x);
        }
    }
}

void subsystemA_main() {
    xTaskCreate(update_data, "updateData", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
}
