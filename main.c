#include "sys_common.h"
#include "gio.h"

/* FreeRTOS Kernel includes. */
#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#define BLINKY_STACK_SIZE 1024

#define BLINK_NAME "Blink"
// #define BLINK_PRIORITY 5
#define BLINK_DELAY_TICKS 800

xTaskHandle xTask1Handle;

void blinkLED(void)
{
    while (1)
    {
        gioToggleBit(gioPORTB, 1);
        vTaskDelay(BLINK_DELAY_TICKS);
    }
}

int main(void)
{
    gioInit(); // Initializes the gio Module
    BaseType_t retval1;

    retval1 = xTaskCreate(blinkLED, BLINK_NAME, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTask1Handle);

    if (retval1)
    {
        vTaskStartScheduler();
    }

    return 1;
}