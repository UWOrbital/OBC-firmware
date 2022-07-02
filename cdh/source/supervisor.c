/*
 * supervisor.c
 *
 * May 30, 2022
 * kiransuren
 *
 */

#include "supervisor.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_task.h"

#ifndef POSIX_BUILD
#include "sys_common.h"
#include "gio.h"
#else
#include "console.h"
#endif


void vSupervisorTask(void * pvParameters){

    while(1){
        #ifndef POSIX_BUILD
        gioToggleBit(gioPORTB, 1);
        #else
        console_print("LED Toggled\n");
        #endif

        vTaskDelay(SUPERVISOR_DELAY_TICKS);
    }
}

