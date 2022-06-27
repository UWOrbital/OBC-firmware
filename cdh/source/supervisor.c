/*
 * supervisor.c
 *
 * May 30, 2022
 * kiransuren
 *
 */

#include "supervisor.h"
#include "telemetry.h"

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_queue.h"
#include "os_task.h"

#include "sys_common.h"
#include "gio.h"
#include "hal_stdtypes.h"

QueueHandle_t supervisorQueue;
extern QueueHandle_t telemetryQueue;

bool doOnce = true;
void vSupervisorTask(void * pvParameters){

    supervisorQueue = xQueueCreate(SUPERVISOR_QUEUE_LENGTH, sizeof(supervisor_event_t));    // initialize queue
    telemetryQueue = xQueueCreate(TELEMETRY_QUEUE_LENGTH, sizeof(telemetry_event_t));    // initialize queue

    xTaskHandle xTelemetryTaskHandle;
    xTaskCreate(vTelemetryTask, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, &xTelemetryTaskHandle);

    while(1){
        supervisor_event_t queueMsg;
        if(!xQueueReceive(supervisorQueue, (void *) &queueMsg, SUPERVISOR_QUEUE_WAIT_PERIOD)){
            queueMsg.eventID = SUPERVISOR_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID)
        {
            case TURN_OFF_LED_EVENT_ID:
                gioToggleBit(gioPORTB, 0);
                telemetry_event_t newMsg;
                newMsg.eventID = TURN_ON_LED_EVENT_ID;
                newMsg.data.delayPeriod = TELEMETRY_DELAY_TICKS;
                xQueueSend(telemetryQueue, (void *) &newMsg, TELEMETRY_QUEUE_WAIT_PERIOD);
                break;
            default:
                if(doOnce){
                    telemetry_event_t newMsg;
                    newMsg.eventID = TURN_ON_LED_EVENT_ID;
                    newMsg.data.delayPeriod = TELEMETRY_DELAY_TICKS;
                    xQueueSend(telemetryQueue, (void *) &newMsg, TELEMETRY_QUEUE_WAIT_PERIOD);
                    doOnce = false;
                }
        }
    }
}

