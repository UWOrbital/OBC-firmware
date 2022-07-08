#include "posix_example.h"
#include "console.h"

#include "FreeRTOS.h"
#include "os_task.h"

int add(int a, int b) {
    return a + b;
}

void vDummyTask(void * pvParameters) {
    vTaskDelay(1000);
    while (1) {
        console_print("Dummy task running\n");
        vTaskDelay(1000);
    }
}
