#include "supervisor.h"
#include "telemetry.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

TaskHandle_t supervisorTaskHandle = NULL;
StaticTask_t supervisorTaskBuffer;
StackType_t supervisorTaskStack[SUPERVISOR_STACK_SIZE];

QueueHandle_t supervisorQueueHandle = NULL;
static StaticQueue_t supervisorQueue;
uint8_t supervisorQueueStack[SUPERVISOR_QUEUE_LENGTH*SUPERVISOR_QUEUE_ITEM_SIZE];

bool doOnce = true;

void initSupervisor(void) {
    supervisorTaskHandle = xTaskCreateStatic(vSupervisorTask, SUPERVISOR_NAME, SUPERVISOR_STACK_SIZE, NULL, SUPERVISOR_PRIORITY, supervisorTaskStack, &supervisorTaskBuffer);
    supervisorQueueHandle = xQueueCreateStatic(SUPERVISOR_QUEUE_LENGTH, SUPERVISOR_QUEUE_ITEM_SIZE, supervisorQueueStack, &supervisorQueue);
}

void vSupervisorTask(void * pvParameters) {
    initTelemetry();

    while(1){
        supervisor_event_t queueMsg;
        if(!xQueueReceive(supervisorQueueHandle, &queueMsg, SUPERVISOR_QUEUE_WAIT_PERIOD)){
            queueMsg.eventID = SUPERVISOR_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID)
        {
            case TURN_OFF_LED_EVENT_ID:
                gioToggleBit(gioPORTB, 0);
                telemetry_event_t newMsg;
                newMsg.eventID = TURN_ON_LED_EVENT_ID;
                newMsg.data.i = TELEMETRY_DELAY_TICKS;
                sendToTelemetryQueue(&newMsg);
                break;
            default:
                if(doOnce){
                    telemetry_event_t newMsg;
                    newMsg.eventID = TURN_ON_LED_EVENT_ID;
                    newMsg.data.i = TELEMETRY_DELAY_TICKS;
                    sendToTelemetryQueue(&newMsg);
                    doOnce = false;
                }
        }
    }
}

uint8_t sendToSupervisorQueue(supervisor_event_t *event) {
    if (supervisorQueueHandle == NULL) {
        return 0;
    }
    return xQueueSend(supervisorQueueHandle, (void *) event, portMAX_DELAY);
}
