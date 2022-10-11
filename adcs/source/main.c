#include "propagator/propagator.h"
#include "console.h"

#include <FreeRTOS.h>
#include <os_task.h>

xTaskHandle dummyTaskHandle;
xTaskHandle orbitPropagatorTaskHandle;

void main(void){
    // Initialize the mutex used to protect the console.
    initConsole();

    // Create a dummy task.
    BaseType_t xReturned;
    xReturned = xTaskCreate(vOrbitPropagatorTask, "orbitalPropagatorTask", 1024, NULL, 1, &orbitPropagatorTaskHandle);
    
    if (xReturned == pdPASS) {
        // Start the scheduler if the task was created successfully.
        vTaskStartScheduler();
    } else {
        printConsole("Failed to create orbit propagator task\n");
    }
}