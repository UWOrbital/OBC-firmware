/*
 * telemetry.c
 *
 * May 30, 2022
 * kiransuren
 *
 */

#include "telemetry.h"
#include "supervisor.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_queue.h"
#include "os_task.h"
#include "os_timer.h"

#include "sys_common.h"
#include "gio.h"
#include "hal_stdtypes.h"

QueueHandle_t telemetryQueue;
extern QueueHandle_t supervisorQueue;

TimerHandle_t LED_timer;


void sendMessage(TimerHandle_t xTimer ){
    supervisor_event_t newMsg;
    newMsg.eventID = TURN_OFF_LED_EVENT_ID;
    xQueueSend(supervisorQueue, (void *) &newMsg, SUPERVISOR_QUEUE_WAIT_PERIOD);
}

void vTelemetryTask(void * pvParameters){
    LED_timer = xTimerCreate("LED_Timer", pdMS_TO_TICKS(1000), false, (void *) 0, sendMessage);
    while(1){
        telemetry_event_t queueMsg;
        if(!xQueueReceive(telemetryQueue, (void *) &queueMsg, TELEMETRY_QUEUE_WAIT_PERIOD)){
            queueMsg.eventID = TELEMETRY_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID)
        {
            case TURN_ON_LED_EVENT_ID:
                //vTaskDelay(queueMsg.data.delayPeriod);
                gioToggleBit(gioPORTB, 1);
                xTimerStart(LED_timer, SUPERVISOR_QUEUE_WAIT_PERIOD);
                break;
            default:
                ;
        }
    }
}

