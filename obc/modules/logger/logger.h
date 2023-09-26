#pragma once

#include "obc_errors.h"
#include "obc_logging.h"

#define LOG_MESSAGE_MAX_BYTES 28U

typedef struct {
  log_level_t logType;
  const char *file;
  uint32_t line;
  union {
    uint32_t errCode;
    const char *msg;
  };
} logger_event_t;

/**
 * @brief Initialize the logger task for logging to the sd card
 *
 * @param currentNumLogFiles pointer to current number of existing log files
 */
void initLoggerTask(uint8_t *currentNumLogFiles);

/**
 * @brief Sends an event to the logger queue
 *
 * @param event Pointer to the event to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToLoggerQueue(logger_event_t *event);

/**
 * @brief Sends an event to the logger queue from an ISR
 *
 * @param event Pointer to the event to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToLoggerQueueFromISR(logger_event_t *event);

/**
 * @brief Set the output location
 *
 * @param newOutputLocation The new output location
 */
void logSetOutputLocation(log_output_location_t newOutputLocation);
