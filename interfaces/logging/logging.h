#pragma once

#include "obc_errors.h"
#include "gs_errors.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

/**
 * @enum logger_error_type_t
 * @brief Logger error code types enum.
 *
 * Enum containing all error code types that logger can return.
 */
typedef enum {
  OBC_ERROR_CODE,
  GS_ERROR_CODE,
} logger_error_type_t;

// Define a general structure to hold the logger error code and its type
typedef struct {
  logger_error_type_t type;
  union {
    obc_error_code_t obcError;
    gs_error_code_t gsError;
  };
} logger_error_code_t;

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
 * @return logger_error_code_t with success code if log is successful.
 *
 */
logger_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           The message that should be logged (MUST BE STATIC)
 * @return logger_error_code_t with success code if log is successful.
 *
 */
logger_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

/**
 * @brief Log an error code from ISR
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode       The error code that needs to be logged
 * @return logger_error_code_t with success code if log is successful.
 *
 */
logger_error_code_t logErrorCodeFromISR(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message from ISR
 *
 * @param msgLevel			Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           The message that should be logged (MUST BE STATIC)
 * @return logger_error_code_t with success code if log is successful.
 *
 */
logger_error_code_t logMsgFromISR(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);
