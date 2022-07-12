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
static StaticQueue_t staticSupervisorQueue;
static supervisorQueueBlock[SUPERVISOR_QUEUE_LENGTH*sizeof(supervisor_event_t)];

extern QueueHandle_t telemetryQueue;
static StaticQueue_t staticTelemetryQueue;
static telemetryQueueBlock[TELEMETRY_QUEUE_LENGTH*sizeof(telemetry_event_t)];

static StackType_t telemetryTaskStack[TELEMETRY_STACK_SIZE];
static StaticTask_t telemetryTaskBuffer;

bool doOnce = true;
void vSupervisorTask(void * pvParameters){
    supervisorQueue = xQueueCreateStatic(SUPERVISOR_QUEUE_LENGTH, sizeof(supervisor_event_t), supervisorQueueBlock, &staticSupervisorQueue);    // initialize queue
    telemetryQueue = xQueueCreateStatic(TELEMETRY_QUEUE_LENGTH, sizeof(telemetry_event_t), telemetryQueueBlock, &staticTelemetryQueue);    // initialize queue

    xTaskHandle xTelemetryTaskHandle;
    xTelemetryTaskHandle = xTaskCreateStatic(vTelemetryTask, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, telemetryTaskStack, &telemetryTaskBuffer);

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

