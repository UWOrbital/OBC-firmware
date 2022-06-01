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

#include "sys_common.h"
#include "gio.h"
#include "eventManager.h"

void vSupervisorTask(void * pvParameters){
    initializeEventQueues();        // MUST be completed before any other tasks are initialized

    while(1){
        event_id eventID = receiveEventQueue(SUPERVISOR_QUEUE_ID);

        switch(eventID){
            case TEST_EVENT1_ID:
                gioToggleBit(gioPORTB, 1);
                break;
            default:
                gioToggleBit(gioPORTB, 0);
                vTaskDelay(SUPERVISOR_DELAY_TICKS);
                addEventQueue(SUPERVISOR_QUEUE_ID, TEST_EVENT1_ID);
        }
    }
}

