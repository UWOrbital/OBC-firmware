#include "FreeRTOS.h"
#include "os_task.h"

#include "console.h"

#ifdef BUILD_DIR
    #define BUILD         BUILD_DIR
#else
    #define BUILD         "./"
#endif

void dummy_task();

int main( void ) {
    console_init();
    console_print("Starting\n");
    xTaskCreate(dummy_task, "mytask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    vTaskStartScheduler();
    for(;;) {}
    return 0;
}

void dummy_task() {
    const TickType_t xDelay = 500/portTICK_PERIOD_MS;
    for(;;) {
        vTaskDelay(xDelay);
        console_print("Hello World!\n");
    }
}
