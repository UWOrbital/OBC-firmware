#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "os_task.h"

#include "console.h"
#include "param_manager.h"

void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char * pcTaskName );
void dummy_taskA();
void dummy_taskB();

int main( void ) {
    console_init();
    param_manager_init();
    console_print("Starting\n");
    xTaskCreate(dummy_taskA, "taskA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    xTaskCreate(dummy_taskB, "taskB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
    vTaskStartScheduler();
    for(;;) {}
    return 0;
}

void dummy_taskA( void ) {
    const TickType_t xDelay = 200/portTICK_PERIOD_MS;
    int8_t altitude;
    for(;;) {
        get_param_val(ALTITUDE, INT8_PARAM, &altitude);
        if (altitude == 30) {
            altitude = 40;
            set_param_val(ALTITUDE, INT8_PARAM, &altitude);
        }
        console_print("Task A: %d\n", altitude);
        vTaskDelay(xDelay);
    }
}

void dummy_taskB( void ) {
    const TickType_t xDelay = 200/portTICK_PERIOD_MS;
    int8_t altitude;
    for(;;) {
        get_param_val(ALTITUDE, INT8_PARAM, &altitude);
        if (altitude == 40) {
            altitude = 30;
            set_param_val(ALTITUDE, INT8_PARAM, &altitude);
        }
        console_print("Task B: %d\n", altitude);
        vTaskDelay(xDelay);
    }
}

void vApplicationMallocFailedHook( void ) {
    vAssertCalled( __FILE__, __LINE__ );
}

void vApplicationIdleHook( void ) {
    usleep( 15000 );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char * pcTaskName ) {
    ( void ) pcTaskName;
    ( void ) pxTask;
    vAssertCalled( __FILE__, __LINE__ );
}

void vAssertCalled( const char * const pcFileName, unsigned long ulLine )
{
    static BaseType_t xPrinted = pdFALSE;
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;
    ( void ) ulLine;
    ( void ) pcFileName;
    taskENTER_CRITICAL(); {
        if( xPrinted == pdFALSE ) {
            xPrinted = pdTRUE;
        }
        while( ulSetToNonZeroInDebuggerToContinue == 0 ) {
            __asm volatile ( "NOP" );
            __asm volatile ( "NOP" );
        }
    }
    taskEXIT_CRITICAL();
}
