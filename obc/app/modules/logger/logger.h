#pragma once

#include "obc_errors.h"
#include "logging.h"

#include <stdio.h>

// Define a bit field structure for the log type and log level
typedef struct {
  unsigned int logType : 1;   // 1 bit for log type (0 for LOG_TYPE_ERROR_CODE, 1 for LOG_TYPE_MSG)
  unsigned int logLevel : 3;  // 3 bits for log level
} log_entry_t;

// Define the log enum based on the bit field structure
typedef enum { LOG_TYPE_ERROR_CODE = 0, LOG_TYPE_MSG = 1 } log_type_t;

// Define a structure for the OBC event logger
typedef struct {
  log_entry_t logEntry;
  const char *file;
  uint32_t line;
  union {
    uint32_t errCode;
    const char *msg;
  };
} logger_event_t;

/**
 * @enum log_output_location_t
 * @brief Log output location enum.
 *
 * Enum containing all locations to output logs to.
 */
typedef enum { LOG_TO_SDCARD, LOG_TO_UART } log_output_location_t;

#ifndef LOG_DEFAULT_OUTPUT_LOCATION
#define LOG_DEFAULT_OUTPUT_LOCATION LOG_TO_UART
#endif

/**
 * @brief Set the output location
 *
 * @param newOutputLocation The new output location
 */
void logSetOutputLocation(log_output_location_t newOutputLocation);
