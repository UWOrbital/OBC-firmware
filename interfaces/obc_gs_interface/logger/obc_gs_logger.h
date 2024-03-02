#pragma once

#include "obc_gs_errors.h"

#include <stdint.h>
#include <stdarg.h>

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

#define LOG_ERROR_CODE(errCode) logErrorCode(LOG_ERROR, __FILE__, __LINE__, errCode)

#define RETURN_IF_ERROR_CODE(_ret)            \
  do {                                        \
    errCode = _ret;                           \
    if (errCode != OBC_GS_ERR_CODE_SUCCESS) { \
      LOG_ERROR_CODE(errCode);                \
      return errCode;                         \
    }                                         \
  } while (0)

#define LOG_IF_ERROR_CODE(_ret)               \
  do {                                        \
    errCode = _ret;                           \
    if (errCode != OBC_GS_ERR_CODE_SUCCESS) { \
      LOG_ERROR_CODE(errCode);                \
    }                                         \
  } while (0)

/**
 * @brief Initialize logger with default logging level
 */
void obcGsInitLogger(void);

/**
 * @brief Set the logging level
 *
 * @param newLogLevel The new logging level
 */
void logSetLevel(log_level_t newLogLevel);

/**
 * @brief Set the logging level
 *
 * @param logBuf Buffer containing data to write to log file
 * @param logBufLen Length of buffer
 * @return obc_gs_error_code_t		OBC_GS_ERR_CODE_UNKNOWN 	if error when logging to file
 * 								OBC_GS_ERR_CODE_SUCCESS			  if message is successfully logged to file
 */
obc_gs_error_code_t writeToLogFile(char logBuf[], int logBufLen);

/**
 * @brief Log an error code
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode       the error code that needs to be logged
 * @return obc_gs_error_code_t		OBC_GS_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_GS_ERR_CODE_INVALID_ARG		if file is null
 *                OBC_GS_ERR_CODE_UNKNOWN       if error logging to file
 * 								OBC_GS_ERR_CODE_SUCCESS			  if message is successfully logged
 *
 */
obc_gs_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           the message that should be logged (MUST BE STATIC)
 * @return obc_gs_error_code_t		OBC_GS_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_GS_ERR_CODE_INVALID_ARG		if file or msg are null
 *                OBC_GS_ERR_CODE_UNKNOWN       if error logging to file
 * 								OBC_GS_ERR_CODE_SUCCESS			if message is successfully logged
 *
 */
obc_gs_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);
