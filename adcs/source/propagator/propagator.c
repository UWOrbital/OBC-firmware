#include "propagator/propagator.h"
#include "console.h"

#include <FreeRTOS.h>
#include <os_task.h>

void vOrbitPropagatorTask(void * pvParameters){
    while (1) {
        printConsole("Orbital propagator task running...\n");
        vTaskDelay(500);
    }
}