#include "FreeRTOS.h"
#include "task.h"

#include "subsystemA.h"
#include "console.h"

static void collect();

void start_collection(void) {
    xTaskCreate(collect, "tm_collection", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    vTaskStartScheduler();
    for(;;) {}
}

static void collect() {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    int x = 0;
    for( ; ; ) {
        console_print( "Got telemetry\n" );
        get_data(&x);
        console_print( "%d\n", x );
        vTaskDelay(xDelay);
    }
}
