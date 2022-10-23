#include "uartstdio.h"
#include "cmdline.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include <spi.h>

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
    UARTprintf((unsigned char *)"Type \'help\' for help.\r\n");
    testCmdLineSDC();
    while(1);
}

int main(void) {
    sciInit();
    spiInit();

    xTaskCreateStatic(vTask1, "testCmdLineSDC", 1024, NULL, 1, taskStack, &taskBuffer);

    vTaskStartScheduler();

    while (1);
}
