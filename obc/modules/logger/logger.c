#include "logger.h"
#include "obc_task_config.h"
#include "obc_logging.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <sys_common.h>
#include <os_queue.h>
#include <redposix.h>

#include <string.h>

#define ERROR_LOG_FILE_NAME "error_file"
/* Comms Manager event queue config */
#define LOGGER_QUEUE_LENGTH 10U
#define LOGGER_QUEUE_ITEM_SIZE sizeof(logger_event_t)
#define LOGGER_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define LOGGER_QUEUE_TX_WAIT_PERIOD 0

static TaskHandle_t loggerTaskHandle = NULL;
static StaticTask_t loggerTaskBuffer;
static StackType_t loggerTaskStack[LOGGER_STACK_SIZE];

static QueueHandle_t loggerQueueHandle = NULL;
static StaticQueue_t loggerQueue;
static uint8_t loggerQueueStack[LOGGER_QUEUE_LENGTH * LOGGER_QUEUE_ITEM_SIZE];

/**
 * @brief	Logger task
 * @param	pvParameters Task parameters.
 */
static void vLoggerTask(void *pvParameters);

/**
 * @brief Initialize the logger task for logging to the sd card
 */
void initLoggerTask(void) {
  ASSERT((loggerTaskStack != NULL) && (&loggerTaskBuffer != NULL));
  if (loggerTaskHandle == NULL) {
    loggerTaskHandle = xTaskCreateStatic(vLoggerTask, LOGGER_NAME, LOGGER_STACK_SIZE, NULL, LOGGER_PRIORITY,
                                         loggerTaskStack, &loggerTaskBuffer);
  }

  ASSERT((loggerQueueStack != NULL) && (&loggerQueue != NULL));
  if (loggerQueueHandle == NULL) {
    loggerQueueHandle = xQueueCreateStatic(LOGGER_QUEUE_LENGTH, LOGGER_QUEUE_ITEM_SIZE, loggerQueueStack, &loggerQueue);
  }
}

static void vLoggerTask(void *pvParameters) {
  const char *fname = ERROR_LOG_FILE_NAME;
  while (1) {
    logger_event_t queueMsg;
    if (xQueueReceive(loggerQueueHandle, &queueMsg, LOGGER_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
      LOG_ERROR_CODE(OBC_ERR_CODE_QUEUE_EMPTY);
      continue;
    }
    switch (queueMsg.logType) {
      case LOG_TYPE_ERROR:
        int32_t fdescriptor = red_open(fname, RED_O_RDWR | RED_O_APPEND | RED_O_CREAT);
        if (fdescriptor == -1) {
          // TODO: figure out best way to deal with this
          // If we tried logging the error code we coule just end up in an infinite loop where we keep failing and
          // trying to log the previous fail
          continue;
        }
        if (red_write(fdescriptor, queueMsg.msg, strlen((char *)queueMsg.msg)) == -1) {
          // TODO: figure out best way to deal with this
          // If we tried logging the error code we coule just end up in an infinite loop where we keep failing and
          // trying to log the previous fail
          continue;
        }
        if (red_close(fdescriptor) == -1) {
          // TODO: figure out best way to deal with this
          // If we tried logging the error code we coule just end up in an infinite loop where we keep failing and
          // trying to log the previous fail
          continue;
        }
        break;
      default:
        LOG_ERROR_CODE(OBC_ERR_CODE_INVALID_ARG);
    }
  }
}

/**
 * @brief Sends an event to the logger queue
 *
 * @param event Pointer to the event to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToLoggerQueue(logger_event_t *event) {
  ASSERT(loggerQueueHandle != NULL);

  if (event == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (xQueueSend(loggerQueueHandle, (void *)event, LOGGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}
