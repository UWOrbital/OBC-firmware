#include "payload_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t payloadTaskHandle = NULL;
static StaticTask_t payloadTaskBuffer;
static StackType_t payloadTaskStack[PAYLOAD_MANAGER_STACK_SIZE];

static QueueHandle_t payloadQueueHandle = NULL;
static StaticQueue_t payloadQueue;
static uint8_t payloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH*PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

/**
 * @brief	Payload Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vPayloadManagerTask(void * pvParameters);

void initPayloadManager(void) {
    ASSERT( (payloadTaskStack != NULL) && (&payloadTaskBuffer != NULL) );
    if (payloadTaskHandle == NULL) {
        payloadTaskHandle = xTaskCreateStatic(vPayloadManagerTask, PAYLOAD_MANAGER_NAME, PAYLOAD_MANAGER_STACK_SIZE, NULL, PAYLOAD_MANAGER_PRIORITY, payloadTaskStack, &payloadTaskBuffer);
    }

    ASSERT( (payloadQueueStack != NULL) && (&payloadQueue != NULL) );
    if (payloadQueueHandle == NULL) {
        payloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE, payloadQueueStack, &payloadQueue);
    }
}

uint8_t sendToPayloadQueue(payload_event_t *event) {
    if (payloadQueueHandle == NULL || event == NULL) {
        return 0;
    }
    if ( xQueueSend(payloadQueueHandle, (void *) event, portMAX_DELAY) == pdPASS ) {
        return 1;
    }
    return 0;
}

static void vPayloadManagerTask(void * pvParameters) {
    while(1){
        payload_event_t queueMsg;
        if(xQueueReceive(payloadQueueHandle, &queueMsg, PAYLOAD_MANAGER_QUEUE_WAIT_PERIOD) != pdPASS) {
            queueMsg.eventID = PAYLOAD_MANAGER_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID) {
            default:
                ;
        }
    }
}
