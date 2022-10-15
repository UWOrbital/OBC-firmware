#include "propagator/propagator.h"
#include "console.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <os_message_buffer.h>

#include <string.h>

xTaskHandle dummyTaskHandle;
xTaskHandle orbitPropagatorTaskHandle;

MessageBufferHandle_t tlePropagatorMsgBuffer = NULL;

void vSendTestTLE(void * pvParameters){
    vTaskDelay(3000);

    tle_msg_t test_data;

    // ISS (ZARYA)
    char line1[] = "1 25544U 98067A   22283.86844078  .00016335  00000+0  29444-3 0  9995";
    char line2[] = "2 25544  51.6418 123.4651 0003105 285.2032 202.0123 15.49917691363127";

    // char line1[] = "1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753";
    // char line2[] = "2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667";


    memcpy(test_data.line_1, line1, 69);
    memcpy(test_data.line_2, line2, 69);

    xMessageBufferSend(tlePropagatorMsgBuffer, &test_data, sizeof(tle_msg_t), 5);

    printConsole("Done sending...\n");
    while(1){}
}


void main(void){
    // Initialize the mutex used to protect the console.
    initConsole();


    // Allocates the message buffer for receiving updates to a TLE
    tlePropagatorMsgBuffer = xMessageBufferCreate(TLE_PROPAGATOR_MSG_BUFFER_SIZE);




    // Create a dummy task.
    BaseType_t xReturned;
    xReturned = xTaskCreate(vOrbitPropagatorTask, "orbitalPropagatorTask", 1024, NULL, 1, &orbitPropagatorTaskHandle);

    xTaskCreate(vSendTestTLE, "SendTestTLETask", 1024, NULL, 1, &dummyTaskHandle);
    
    if (xReturned == pdPASS) {
        // Start the scheduler if the task was created successfully.
        vTaskStartScheduler();
    } else {
        printConsole("Failed to create orbit propagator task\n");
    }
}