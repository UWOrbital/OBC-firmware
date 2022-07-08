#include "posix_example.h"
#include "console.h"

#include "FreeRTOS.h"
#include "os_task.h"

xTaskHandle dummyTaskHandle;

void main(void) {
    console_init();

    console_print("%d\n", add(1, 2));
    console_print("%d\n", add(1, 1));
    console_print("%d\n", add(1, 0));

    xTaskCreate(vDummyTask, "dummyTaskA", 1024, NULL, 1, &dummyTaskHandle);
    
    vTaskStartScheduler();
}

