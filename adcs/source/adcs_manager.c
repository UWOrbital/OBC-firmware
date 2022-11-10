#include "adcs_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t adcsTaskHandle = NULL;
static StaticTask_t adcsTaskBuffer;
static StackType_t adcsTaskStack[ADCS_MANAGER_STACK_SIZE];

static QueueHandle_t adcsQueueHandle = NULL;
static StaticQueue_t adcsQueue;
static uint8_t adcsQueueStack[ADCS_MANAGER_QUEUE_LENGTH*ADCS_MANAGER_QUEUE_ITEM_SIZE];

/**
 * @brief	ADCS Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vADCSManagerTask(void * pvParameters);

void initADCS(void) {
    ASSERT( (adcsTaskStack != NULL) && (&adcsTaskBuffer != NULL) );
    if (adcsTaskHandle == NULL) {
        adcsTaskHandle = xTaskCreateStatic(vADCSManagerTask, ADCS_MANAGER_NAME, ADCS_MANAGER_STACK_SIZE, NULL, ADCS_MANAGER_PRIORITY, adcsTaskStack, &adcsTaskBuffer);
    }

    ASSERT( (adcsQueueStack != NULL) && (&adcsQueue != NULL) );
    if (adcsQueueHandle == NULL) {
        adcsQueueHandle = xQueueCreateStatic(ADCS_MANAGER_QUEUE_LENGTH, ADCS_MANAGER_QUEUE_ITEM_SIZE, adcsQueueStack, &adcsQueue);
    }
}

uint8_t sendToADCSQueue(adcs_event_t *event) {
    if (adcsQueueHandle == NULL || event == NULL) {
        return 0;
    }
    if ( xQueueSend(adcsQueueHandle, (void *) event, portMAX_DELAY) == pdPASS ) {
        return 1;
    }
    return 0;
}

static void vADCSManagerTask(void * pvParameters) {
    while(1){
        adcs_event_t queueMsg;
        if(xQueueReceive(adcsQueueHandle, &queueMsg, ADCS_MANAGER_QUEUE_WAIT_PERIOD) != pdPASS) {
            queueMsg.eventID = ADCS_MANAGER_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID) {
            default:
                ;
        }
    }
}
