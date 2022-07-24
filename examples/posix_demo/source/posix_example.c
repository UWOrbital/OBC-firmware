#include "posix_example.h"
#include "console.h"

#include <FreeRTOS.h>
#include <os_task.h>

void vDummyTask(void * pvParameters) {
    while (1) {
        printConsole("Dummy task running\n");
        vTaskDelay(1000);
    }
}
