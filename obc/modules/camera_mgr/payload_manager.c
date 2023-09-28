#include "payload_manager.h"
#include "obc_errors.h"
#include "obc_task_config.h"

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
static uint8_t payloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH * PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

static TaskHandle_t secondaryPayloadTaskHandle = NULL;
static StaticTask_t secondaryPayloadTaskBuffer;
static StackType_t secondaryPayloadTaskStack[PAYLOAD_MANAGER_STACK_SIZE];

static QueueHandle_t secondaryPayloadQueueHandle = NULL;
static StaticQueue_t secondaryPayloadQueue;
static uint8_t secondaryPayloadQueueStack[PAYLOAD_MANAGER_QUEUE_LENGTH * PAYLOAD_MANAGER_QUEUE_ITEM_SIZE];

static SemaphoreHandle_t xPayloadMutex; 



/**
 * @brief	Payload Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vPayloadManagerTask(void *pvParameters);

void initPayloadManager(void) {
  ASSERT((payloadTaskStack != NULL) && (&payloadTaskBuffer != NULL));
  if (payloadTaskHandle == NULL) {
    payloadTaskHandle = xTaskCreateStatic(vPayloadManagerTask, PAYLOAD_MANAGER_NAME, PAYLOAD_MANAGER_STACK_SIZE, NULL,
                                          PAYLOAD_MANAGER_PRIORITY, payloadTaskStack, &payloadTaskBuffer);
  }

  ASSERT((payloadQueueStack != NULL) && (&payloadQueue != NULL));
  if (payloadQueueHandle == NULL) {
    payloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE,
                                            payloadQueueStack, &payloadQueue);
  }

  //Created a separate queue for the secondary payload
  ASSERT((secondaryPayloadTaskStack != NULL) && (&secondaryPayloadTaskBuffer != NULL));
  xPayloadMutex = xSemaphoreCreateMutex();
  if (xPayloadMutex != NULL) {
    secondaryPayloadTaskHandle = xTaskCreateStatic(secondaryPayloadManagerTask, PAYLOAD_MANAGER_NAME, PAYLOAD_MANAGER_STACK_SIZE, NULL,
                                          PAYLOAD_MANAGER_PRIORITY, secondaryPayloadTaskStack, &secondaryPayloadTaskBuffer);
  }

  ASSERT((secondaryPayloadQueueStack != NULL) && (&secondaryPayloadQueue != NULL));
  if (secondaryPayloadQueueHandle == NULL) {
    secondaryPayloadQueueHandle = xQueueCreateStatic(PAYLOAD_MANAGER_QUEUE_LENGTH, PAYLOAD_MANAGER_QUEUE_ITEM_SIZE,
                                            secondaryPayloadQueueStack, &secondaryPayloadQueue);
  }
  
}

obc_error_code_t sendToPayloadQueue(payload_event_t *event) {
  ASSERT(payloadQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(payloadQueueHandle, (void *)event, PAYLOAD_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS)
    return OBC_ERR_CODE_SUCCESS;

  return OBC_ERR_CODE_QUEUE_FULL;
}

static void vPayloadManagerTask(void *pvParameters) {
  ASSERT(payloadQueueHandle != NULL);

  while (1) {
    payload_event_t queueMsg;
    if (xQueueReceive(payloadQueueHandle, &queueMsg, PAYLOAD_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS)
      queueMsg.eventID = PAYLOAD_MANAGER_NULL_EVENT_ID;

    switch (queueMsg.eventID) {
      case PAYLOAD_MANAGER_NULL_EVENT_ID:
        break;
    }
  }
}

static void secondaryPayloadManagerTask(void *pvParameters){
  ASSERT(secondaryPayloadQueueHandle != NULL);
  
  while(1){
    if (xSemaphoreTake(xPayloadMutex, portMAX_DELAY) == pdTRUE){
      // ADD SECONDARY PAYLOAD COMMAND HANDLER

      //releases Payload mutex when secondary payload functionality has been executed
      xSemaphoreGive(xPayloadMutex);
    }
    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}