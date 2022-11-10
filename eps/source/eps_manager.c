#include "eps_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t epsTaskHandle = NULL;
static StaticTask_t epsTaskBuffer;
static StackType_t epsTaskStack[EPS_MANAGER_STACK_SIZE];

static QueueHandle_t epsQueueHandle = NULL;
static StaticQueue_t epsQueue;
static uint8_t epsQueueStack[EPS_MANAGER_QUEUE_LENGTH*EPS_MANAGER_QUEUE_ITEM_SIZE];

/**
 * @brief	EPS Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vEPSManagerTask(void * pvParameters);

void initEPS(void) {
    ASSERT( (epsTaskStack != NULL) && (&epsTaskBuffer != NULL) );
    if (epsTaskHandle == NULL) {
        epsTaskHandle = xTaskCreateStatic(vEPSManagerTask, EPS_MANAGER_NAME, EPS_MANAGER_STACK_SIZE, NULL, EPS_MANAGER_PRIORITY, epsTaskStack, &epsTaskBuffer);
    }

    ASSERT( (epsQueueStack != NULL) && (&epsQueue != NULL) );
    if (epsQueueHandle == NULL) {
        epsQueueHandle = xQueueCreateStatic(EPS_MANAGER_QUEUE_LENGTH, EPS_MANAGER_QUEUE_ITEM_SIZE, epsQueueStack, &epsQueue);
    }
}

uint8_t sendToEPSQueue(eps_event_t *event) {
    if (epsQueueHandle == NULL || event == NULL) {
        return 0;
    }
    if ( xQueueSend(epsQueueHandle, (void *) event, portMAX_DELAY) == pdPASS ) {
        return 1;
    }
    return 0;
}

static void vEPSManagerTask(void * pvParameters) {
    while(1){
        eps_event_t queueMsg;
        if(xQueueReceive(epsQueueHandle, &queueMsg, EPS_MANAGER_QUEUE_WAIT_PERIOD) != pdPASS) {
            queueMsg.eventID = EPS_MANAGER_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID) {
            default:
                ;
        }
    }
}
