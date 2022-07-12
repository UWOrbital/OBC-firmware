#include "posix_example.h"
#include "console.h"

#include "FreeRTOS.h"
#include "os_task.h"

xTaskHandle dummyTaskHandle;

void main(void) {
    console_init();

    xTaskCreate(vDummyTask, "dummyTaskA", 1024, NULL, 1, &dummyTaskHandle);
    
    vTaskStartScheduler();
}

