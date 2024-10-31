#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stdarg.h>

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
 * @enum log_level_t
 * @brief Log levels enum.
 *
 * Enum containing all log levels.
 */
typedef enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL, LOG_OFF } log_level_t;

// Define a bit field structure for the log type and log level
typedef struct {
  unsigned int logType : 1;   // 1 bit for log type (0 for LOG_TYPE_ERROR_CODE, 1 for LOG_TYPE_MSG)
  unsigned int logLevel : 3;  // 3 bits for log level
} log_entry_t;

// Define the log enum based on the bit field structure
typedef enum { LOG_TYPE_ERROR_CODE = 0, LOG_TYPE_MSG = 1 } log_type_t;

#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL LOG_TRACE
#endif

#define LOG_TRACE(msg)
#define LOG_DEBUG(msg)
#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)
#define LOG_FATAL(msg)

#define LOG_TRACE_FROM_ISR(msg)
#define LOG_DEBUG_FROM_ISR(msg)
#define LOG_INFO_FROM_ISR(msg)
#define LOG_WARN_FROM_ISR(msg)
#define LOG_ERROR_FROM_ISR(msg)
#define LOG_FATAL_FROM_ISR(msg)

#define LOG_ERROR_CODE(errCode)
