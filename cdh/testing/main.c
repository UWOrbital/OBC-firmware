#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>

#include "FreeRTOS.h"
#include "task.h"

#include "console.h"

#ifdef BUILD_DIR
    #define BUILD         BUILD_DIR
#else
    #define BUILD         "./"
#endif

/* This demo uses heap_3.c (the libc provided malloc() and free()). */

extern void status_check_main(void);
extern void subsystemA_main(void);

void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char * pcTaskName );

int main( void ) {
    console_init();
    console_print("Starting\n");
    subsystemA_main();
    status_check_main();
    vTaskStartScheduler();
    for(;;) {}
    return 0;
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
