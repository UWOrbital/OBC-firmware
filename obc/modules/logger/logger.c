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

static uint8_t currentLogFileNum = 0;
#define LOG_FILE_NAME "log_file0.log"

#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
#define SD_CARD_LOG_MAX_FILE_MSG 20U
// Extra 10 for the small extra pieces in "%s - %s\r\n"
#define MAX_LOG_SIZE (MAX_MSG_SIZE + MAX_FNAME_LINENUM_SIZE + 10U)
#define MAX_SD_CARD_LOG_SIZE (MAX_MSG_SIZE + SD_CARD_LOG_MAX_FILE_MSG + 3U)

#define UART_MUTEX_BLOCK_TIME portMAX_DELAY

static const char *LEVEL_STRINGS[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static log_output_location_t outputLocation;

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
 * @brief logs a received message to log
 *
 * @param logMsg string that needs to be logged
 * @param fname name of the file to write to
 */
static obc_error_code_t logMsg(const char *logMsg, const char *fname);

/**
 * @brief formats
 */

/**
 * @brief Initialize the logger task for logging to the sd card
 *
 * @param currentNumLogFiles pointer to current number of existing log files
 */
void initLoggerTask(uint8_t *currentNumLogFiles) {
  ASSERT((loggerTaskStack != NULL) && (&loggerTaskBuffer != NULL));
  if (loggerTaskHandle == NULL) {
    loggerTaskHandle =
        xTaskCreateStatic(vLoggerTask, LOGGER_TASK_NAME, LOGGER_TASK_STACK_SIZE, (void *)currentNumLogFiles,
                          LOGGER_TASK_PRIORITY, loggerTaskStack, &loggerTaskBuffer);
  }

  ASSERT((loggerQueueStack != NULL) && (&loggerQueue != NULL));
  if (loggerQueueHandle == NULL) {
    loggerQueueHandle = xQueueCreateStatic(LOGGER_QUEUE_LENGTH, LOGGER_QUEUE_ITEM_SIZE, loggerQueueStack, &loggerQueue);
  }
}

static void vLoggerTask(void *pvParameters) {
  uint8_t currentNumLogFiles = *((uint8_t *)pvParameters);
  char *fname = LOG_FILE_NAME;
  while (1) {
    logger_event_t queueMsg;
    if (xQueueReceive(loggerQueueHandle, &queueMsg, LOGGER_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
      continue;
    }
    if (queueMsg.logType > LOG_FATAL) {
      LOG_ERROR(OBC_ERR_CODE_UNSUPPORTED_EVENT);
      continue;
    }
    if (queueMsg.file == NULL) {
      LOG_ERROR(OBC_ERR_CODE_UNSUPPORTED_EVENT);
      continue;
    }

    char buf[MAX_LOG_SIZE] = {0};
    int bufLen = 0;
    switch (queueMsg.logType) {
      case LOG_ERROR:
        // File & line number
        char infobuf[MAX_FNAME_LINENUM_SIZE] = {0};
        int ret = snprintf(infobuf, MAX_FNAME_LINENUM_SIZE, "%-5s -> %s:%lu", LEVEL_STRINGS[queueMsg.logType],
                           queueMsg.file, queueMsg.line);
        if (ret < 0) {
          LOG_ERROR(OBC_ERR_CODE_INVALID_ARG);
          continue;
        }
        if ((uint32_t)ret >= MAX_FNAME_LINENUM_SIZE) {
          LOG_ERROR(OBC_ERR_CODE_BUFF_TOO_SMALL);
          continuel
        }

        // Prepare entire output
        bufLen = snprintf(buf, MAX_LOG_SIZE, "%s - %u\r\n", infobuf, queueMsg.errCode);
        if (bufLen < 0) {
          LOG_ERROR(OBC_ERR_CODE_INVALID_ARG);
          continue;
        }
        if ((uint32_t)bufLen >= MAX_LOG_SIZE) {
          LOG_ERROR(OBC_ERR_CODE_BUFF_TOO_SMALL);
          continue;
        }
        break;
      case LOG_DEBUG:
        if (queueMsg.msg == NULL) {
          LOG_ERROR(OBC_ERR_CODE_INVALID_ARG);
          continue;
        }
        // File & line number
        char infobuf[MAX_FNAME_LINENUM_SIZE] = {0};
        int ret = snprintf(infobuf, MAX_FNAME_LINENUM_SIZE, "%-5s -> %s:%lu", LEVEL_STRINGS[queueMsg.logType],
                           queueMsg.file, queueMsg.line);
        if (ret < 0) {
          LOG_ERROR(OBC_ERR_CODE_INVALID_ARG);
          continue;
        }
        if ((uint32_t)ret >= MAX_FNAME_LINENUM_SIZE) {
          LOG_ERROR(OBC_ERR_CODE_BUFF_TOO_SMALL);
          continuel
        }

        // Prepare entire output
        bufLen = snprintf(buf, MAX_LOG_SIZE, "%s - %s\r\n", infobuf, queueMsg.msg);
        if (bufLen < 0) {
          LOG_ERROR(OBC_ERR_CODE_INVALID_ARG);
          continue;
        }
        if ((uint32_t)bufLen >= MAX_LOG_SIZE) {
          LOG_ERROR(OBC_ERR_CODE_BUFF_TOO_SMALL);
          continue;
        }
        break;
      default:
        LOG_ERROR(OBC_ERR_CODE_UNSUPPORTED_EVENT);
    }
    if (outputLocation == LOG_TO_SDCARD) {
      int32_t fdescriptor = red_open(fname, RED_O_WRONLY | RED_O_APPEND | RED_O_CREAT);
      if (fdescriptor == -1) {
        continue;
      }
      if (red_write(fdescriptor, buf, bufLen) == -1) {
        continue;
      }
      if (red_close(fdescriptor) == -1) {
        continue;
      }
    } else {
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

/**
 * @brief Sends an event to the logger queue from an ISR
 *
 * @param event Pointer to the event to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToLoggerQueueFromISR(logger_event_t *event) {
  if (loggerQueueHandle == NULL) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (event == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  BaseType_t xHigherPriorityTaskAwoken = pdFALSE;

  xQueueSendFromISR(loggerQueueHandle, (void *)event, xHigherPriorityTaskAwoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskAwoken);

  return OBC_ERR_CODE_QUEUE_FULL;
}

void logSetOutputLocation(log_output_location_t newOutputLocation) { outputLocation = newOutputLocation; }
