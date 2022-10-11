#include "max9934_test.h"
#include "telemetry.h"
#include "supervisor.h"
#include "max9934.h"
#include "obc_sci_io.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static TaskHandle_t max9934TaskHandler = NULL;
static StaticTask_t max9934Taskbuffer;
static StackType_t max9934TaskStack[MAX9934_STACK_SIZE];

/**
 * @brief	MAX9934 test task.
 * @param	pvParameters	Task parameters.
 */
static void vMAX9934Task(void * pvParameters);

void initMAX9934(void) {
    ASSERT((max9934TaskStack != NULL) && (&max9934Taskbuffer != NULL));
    if(max9934TaskHandler == NULL) {
        max9934TaskHandler = xTaskCreateStatic(vMAX9934Task, MAX9934_NAME, MAX9934_STACK_SIZE, NULL, MAX9934_PRIORITY, max9934TaskStack, &max9934Taskbuffer);
    }
}

static void vMAX9934Task(void * pvParamters) {
    float current = 0;
    while(1) {
        if(getCurrentMAX9934(&current) == 1) {
            unsigned char value[100];
            gcvt(current, 6, value);
            unsigned char msg[] = "Value: ";
            unsigned char unit[] = " uA\n";

            printTextSci(sciREG, msg, strlen((const char*) msg));
            printTextSci(sciREG, value, strlen((const char*) value));
            printTextSci(sciREG, unit, strlen((const char*) unit));
            vTaskDelay(500);
        }
    }
}