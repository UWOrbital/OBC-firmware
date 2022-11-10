#include "comms_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t commsTaskHandle = NULL;
static StaticTask_t commsTaskBuffer;
static StackType_t commsTaskStack[COMMS_MANAGER_STACK_SIZE];

static QueueHandle_t commsQueueHandle = NULL;
static StaticQueue_t commsQueue;
static uint8_t commsQueueStack[COMMS_MANAGER_QUEUE_LENGTH*COMMS_MANAGER_QUEUE_ITEM_SIZE];

/**
 * @brief	Comms Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vCommsManagerTask(void * pvParameters);

void initCommsManager(void) {
    ASSERT( (commsTaskStack != NULL) && (&commsTaskBuffer != NULL) );
    if (commsTaskHandle == NULL) {
        commsTaskHandle = xTaskCreateStatic(vCommsManagerTask, COMMS_MANAGER_NAME, COMMS_MANAGER_STACK_SIZE, NULL, COMMS_MANAGER_PRIORITY, commsTaskStack, &commsTaskBuffer);
    }

    ASSERT( (commsQueueStack != NULL) && (&commsQueue != NULL) );
    if (commsQueueHandle == NULL) {
        commsQueueHandle = xQueueCreateStatic(COMMS_MANAGER_QUEUE_LENGTH, COMMS_MANAGER_QUEUE_ITEM_SIZE, commsQueueStack, &commsQueue);
    }
}

uint8_t sendToCommsQueue(comms_event_t *event) {
    if (commsQueueHandle == NULL || event == NULL) {
        return 0;
    }
    if ( xQueueSend(commsQueueHandle, (void *) event, portMAX_DELAY) == pdPASS ) {
        return 1;
    }
    return 0;
}

static void vCommsManagerTask(void * pvParameters) {
    while(1){
        comms_event_t queueMsg;
        if(xQueueReceive(commsQueueHandle, &queueMsg, COMMS_MANAGER_QUEUE_WAIT_PERIOD) != pdPASS) {
            queueMsg.eventID = COMMS_MANAGER_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID) {
            default:
                ;
        }
    }
}
