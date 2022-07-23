#include "telemetry.h"
#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

TaskHandle_t telemetryTaskHandle = NULL;
StaticTask_t telemetryTaskBuffer;
StackType_t telemetryTaskStack[TELEMETRY_STACK_SIZE];

QueueHandle_t telemetryQueueHandle = NULL;
static StaticQueue_t telemetryQueue;
uint8_t telemetryQueueStack[TELEMETRY_QUEUE_LENGTH*TELEMETRY_QUEUE_ITEM_SIZE];

TimerHandle_t ledTimer;
StaticTimer_t ledTimerBuffer;

void timerCallback(TimerHandle_t xTimer) {
    supervisor_event_t newMsg;
    newMsg.eventID = TURN_OFF_LED_EVENT_ID;
    sendToSupervisorQueue(&newMsg);
}

void initTelemetry(void) {
    telemetryTaskHandle = xTaskCreateStatic(vTelemetryTask, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, telemetryTaskStack, &telemetryTaskBuffer);
    telemetryQueueHandle = xQueueCreateStatic(TELEMETRY_QUEUE_LENGTH, TELEMETRY_QUEUE_ITEM_SIZE, telemetryQueueStack, &telemetryQueue);
    ledTimer = xTimerCreateStatic("ledTimer", pdMS_TO_TICKS(1000), false, (void *) 0, timerCallback, &ledTimerBuffer);
}

void vTelemetryTask(void * pvParameters) {
    while(1){
        telemetry_event_t queueMsg;
        if(!xQueueReceive(telemetryQueueHandle, &queueMsg, TELEMETRY_QUEUE_WAIT_PERIOD)){
            queueMsg.eventID = TELEMETRY_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID)
        {
            case TURN_ON_LED_EVENT_ID:
                vTaskDelay(queueMsg.data.i);
                gioToggleBit(gioPORTB, 1);
                xTimerStart(ledTimer, SUPERVISOR_QUEUE_WAIT_PERIOD);
                break;
            default:
                ;
        }
    }
}

uint8_t sendToTelemetryQueue(telemetry_event_t *event) {
    if (telemetryQueueHandle == NULL) {
        return 0;
    }
    return xQueueSend(telemetryQueueHandle, (void *) event, portMAX_DELAY);
}