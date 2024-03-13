#pragma once

#include "obc_errors.h"
#include "obc_gs_errors.h"
#include "gs_errors.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

/**
 * @enum error_type_t
 * @brief Error code types enum.
 *
 * Enum containing all error code types.
 */
typedef enum {
  OBC_ERROR_CODE,
  OBC_GS_ERROR_CODE,
  GS_ERROR_CODE,
} error_type_t;

// Define a general structure to hold the error code and its type
typedef struct {
  error_type_t type;
  union {
    obc_error_code_t obcError;
    obc_gs_error_code_t obcGsError;
    gs_error_code_t gsError;
  };
} error_code_t;

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

#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL LOG_TRACE
#endif

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

#define LOG_TRACE(msg) logMsg(LOG_TRACE, __FILE__, __LINE__, msg)
#define LOG_DEBUG(msg) logMsg(LOG_DEBUG, __FILE__, __LINE__, msg)
#define LOG_INFO(msg) logMsg(LOG_INFO, __FILE__, __LINE__, msg)
#define LOG_WARN(msg) logMsg(LOG_WARN, __FILE__, __LINE__, msg)
#define LOG_ERROR(msg) logMsg(LOG_ERROR, __FILE__, __LINE__, msg)
#define LOG_FATAL(msg) logMsg(LOG_FATAL, __FILE__, __LINE__, msg)

#define LOG_TRACE_FROM_ISR(msg) logMsgFromISR(LOG_TRACE, __FILE__, __LINE__, msg)
#define LOG_DEBUG_FROM_ISR(msg) logMsgFromISR(LOG_DEBUG, __FILE__, __LINE__, msg)
#define LOG_INFO_FROM_ISR(msg) logMsgFromISR(LOG_INFO, __FILE__, __LINE__, msg)
#define LOG_WARN_FROM_ISR(msg) logMsgFromISR(LOG_WARN, __FILE__, __LINE__, msg)
#define LOG_ERROR_FROM_ISR(msg) logMsgFromISR(LOG_ERROR, __FILE__, __LINE__, msg)
#define LOG_FATAL_FROM_ISR(msg) logMsgFromISR(LOG_FATAL, __FILE__, __LINE__, msg)

#define LOG_ERROR_CODE(errCode) logErrorCode(LOG_ERROR, __FILE__, __LINE__, errCode)
#define LOG_ERROR_CODE_FROM_ISR(errCode) logErrorCodeFromISR(LOG_ERROR, __FILE__, __LINE__, errCode)

#define RETURN_IF_ERROR_CODE(_ret) \
  do {                             \
    errCode = _ret;                \
    if (errCode != 0) {            \
      LOG_ERROR_CODE(errCode);     \
      return errCode;              \
    }                              \
  } while (0)

#define LOG_IF_ERROR_CODE(_ret) \
  do {                          \
    errCode = _ret;             \
    if (errCode != 0) {         \
      LOG_ERROR_CODE(errCode);  \
    }                           \
  } while (0)

/**
 * @brief Initialize the logger
 */
void initLogger(void);

/**
 * @brief Set the output location
 *
 * @param newOutputLocation The new output location
 */
void logSetOutputLocation(log_output_location_t newOutputLocation);

/**
 * @brief Set the logging level
 *
 * @param newLogLevel The new logging level
 */
void logSetLevel(log_level_t newLogLevel);

/**
 * @brief Log an error code
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode       The error code that needs to be logged
 * @return error_code_t with success code if log is successful.
 *
 */
error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           The message that should be logged (MUST BE STATIC)
 * @return error_code_t with success code if log is successful.
 *
 */
error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

/**
 * @brief Log an error code from ISR
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode       The error code that needs to be logged
 * @return error_code_t with success code if log is successful.
 *
 */
error_code_t logErrorCodeFromISR(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message from ISR
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           The message that should be logged (MUST BE STATIC)
 * @return error_code_t with success code if log is successful.
 *
 */
error_code_t logMsgFromISR(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

/**
 * @brief Sends an event to the logger queue
 *
 * @param event Pointer to the event to send
 * @param blockTimeTicks Maximum time to wait for available space in queue
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the packet was sent to the queue.
 */
obc_error_code_t sendToLoggerQueue(logger_event_t *event, size_t blockTimeTicks);
