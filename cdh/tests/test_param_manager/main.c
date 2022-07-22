#include "param_manager.h"
#include "console.h"

#include <FreeRTOS.h>
#include <os_task.h>


void dummyTaskA();
void dummyTaskB();

int main( void ) {
    initConsole();
    initParamManager();
    printConsole("Starting\n");
    xTaskCreate(dummyTaskA, "taskA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    xTaskCreate(dummyTaskB, "taskB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    vTaskStartScheduler();
    for(;;) {}
    return 0;
}

void dummyTaskA( void ) {
    const TickType_t xDelay = 200/portTICK_PERIOD_MS;
    int8_t altitude;
    for(;;) {
        getParamVal(ALTITUDE, INT8_PARAM, &altitude);
        if (altitude == 30) {
            altitude = 40;
            setParamVal(ALTITUDE, INT8_PARAM, &altitude);
        }
        printConsole("Task A: %d\n", altitude);
        vTaskDelay(xDelay);
    }
}

void dummyTaskB( void ) {
    const TickType_t xDelay = 200/portTICK_PERIOD_MS;
    int8_t altitude;
    for(;;) {
        getParamVal(ALTITUDE, INT8_PARAM, &altitude);
        if (altitude == 40) {
            altitude = 30;
            setParamVal(ALTITUDE, INT8_PARAM, &altitude);
        }
        printConsole("Task B: %d\n", altitude);
        vTaskDelay(xDelay);
    }
}
