#pragma once

#include "obc_errors.h"

#define LOG_MESSAGE_MAX_BYTES 28U

typedef enum {
  LOG_TYPE_ERROR,
} log_type_t;

typedef struct {
  log_type_t logType;
  unsigned char msg[LOG_MESSAGE_MAX_BYTES];
} logger_event_t;

/**
 * @brief Initialize the logger task for logging to the sd card
 */
void initLoggerTask(void);

/**
 * @brief Sends an event to the logger queue
 *
 * @param event Pointer to the event to send
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue
 */
obc_error_code_t sendToLoggerQueue(logger_event_t *event);
