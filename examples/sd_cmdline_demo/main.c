#include "uartstdio.h"
#include "cmdline.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sci.h>
#include "spi.h"

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

void vTask1(void *pvParameters) {
    UARTprintf((unsigned char *)"Type \'help\' for help.\r\n");
    SD_Test();
    while(1);
}

int main(void) {
    // Run hardware initialization code (TODO: refactor all this into one function call)
    sciInit();
    spiInit();

    xTaskCreateStatic(vTask1, "SD_Test", 1024, NULL, 1, taskStack, &taskBuffer);

    vTaskStartScheduler();

    while (1);
}
