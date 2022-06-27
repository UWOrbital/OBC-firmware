#include "FreeRTOS.h"
#include "os_task.h"

#include "console.h"
#include "param_manager.h"

void dummy_taskA();
void dummy_taskB();

int main( void ) {
    console_init();
    param_manager_init();
    console_print("Starting\n");
    xTaskCreate(dummy_taskA, "taskA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    xTaskCreate(dummy_taskB, "taskB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    vTaskStartScheduler();
    for(;;) {}
    return 0;
}

void dummy_taskA( void ) {
    const TickType_t xDelay = 200/portTICK_PERIOD_MS;
    int8_t altitude;
    for(;;) {
        get_param_val(ALTITUDE, INT8_PARAM, &altitude);
        if (altitude == 30) {
            altitude = 40;
            set_param_val(ALTITUDE, INT8_PARAM, &altitude);
        }
        console_print("Task A: %d\n", altitude);
        vTaskDelay(xDelay);
    }
}

void dummy_taskB( void ) {
    const TickType_t xDelay = 200/portTICK_PERIOD_MS;
    int8_t altitude;
    for(;;) {
        get_param_val(ALTITUDE, INT8_PARAM, &altitude);
        if (altitude == 40) {
            altitude = 30;
            set_param_val(ALTITUDE, INT8_PARAM, &altitude);
        }
        console_print("Task B: %d\n", altitude);
        vTaskDelay(xDelay);
    }
}
