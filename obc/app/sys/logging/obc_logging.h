#pragma once

#include "obc_errors.h"
#include "obc_gs_errors.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#define __FILE_FROM_REPO_ROOT__ \
  (strstr(__FILE__, SOURCE_PATH) ? strstr(__FILE__, SOURCE_PATH) + sizeof(SOURCE_PATH) - 1 : __FILE__)

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

#ifdef BUILD_TYPE_OBC_FW

#define LOG_TRACE(msg) logMsg(LOG_TRACE, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_DEBUG(msg) logMsg(LOG_DEBUG, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_INFO(msg) logMsg(LOG_INFO, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_WARN(msg) logMsg(LOG_WARN, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_ERROR(msg) logMsg(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_FATAL(msg) logMsg(LOG_FATAL, __FILE_FROM_REPO_ROOT__, __LINE__, msg)

#define LOG_TRACE_FROM_ISR(msg) logMsgFromISR(LOG_TRACE, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_DEBUG_FROM_ISR(msg) logMsgFromISR(LOG_DEBUG, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_INFO_FROM_ISR(msg) logMsgFromISR(LOG_INFO, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_WARN_FROM_ISR(msg) logMsgFromISR(LOG_WARN, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_ERROR_FROM_ISR(msg) logMsgFromISR(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_FATAL_FROM_ISR(msg) logMsgFromISR(LOG_FATAL, __FILE_FROM_REPO_ROOT__, __LINE__, msg)

#define LOG_ERROR_CODE(errCode) logErrorCode(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, errCode)
#define LOG_ERROR_CODE_FROM_ISR(errCode) logErrorCodeFromISR(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, errCode)

#define RETURN_IF_ERROR_CODE(_ret)         \
  do {                                     \
    errCode = _ret;                        \
    if (errCode != OBC_ERR_CODE_SUCCESS) { \
      LOG_ERROR_CODE(errCode);             \
      return errCode;                      \
    }                                      \
  } while (0)

#define LOG_IF_ERROR_CODE(_ret)            \
  do {                                     \
    errCode = _ret;                        \
    if (errCode != OBC_ERR_CODE_SUCCESS) { \
      LOG_ERROR_CODE(errCode);             \
    }                                      \
  } while (0)

/**
 * @brief Set the logging level
 *
 * @param newLogLevel The new logging level
 */
void logSetLevel(log_level_t newLogLevel);

/**
 * @brief Log an error code
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode       the error code that needs to be logged
 * @return obc_error_code_t		OBC_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_ERR_CODE_INVALID_ARG		if file or s are null or if there is an encoding error
 * 								OBC_ERR_CODE_SUCCESS			if message is successfully logged
 * 								OBC_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           the message that should be logged (MUST BE STATIC)
 * @return obc_error_code_t		OBC_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_ERR_CODE_INVALID_ARG		if file or s are null or if there is an encoding error
 * 								OBC_ERR_CODE_SUCCESS			if message is successfully logged
 * 								OBC_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

/**
 * @brief Log an error code from ISR
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode       the error code that needs to be logged
 * @return obc_error_code_t		OBC_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_ERR_CODE_INVALID_ARG		if file or s are null or if there is an encoding error
 * 								OBC_ERR_CODE_SUCCESS			if message is successfully logged
 * 								OBC_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_error_code_t logErrorCodeFromISR(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message from ISR
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg           the message that should be logged (MUST BE STATIC)
 * @return obc_error_code_t		OBC_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_ERR_CODE_INVALID_ARG		if file or s are null or if there is an encoding error
 * 								OBC_ERR_CODE_SUCCESS			if message is successfully logged
 * 								OBC_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_error_code_t logMsgFromISR(log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

#else

#ifdef BUILD_TYPE_OBC_GS

#define LOG_TRACE(msg) gsLogMsg(LOG_TRACE, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_DEBUG(msg) gsLogMsg(LOG_DEBUG, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_INFO(msg) gsLogMsg(LOG_INFO, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_WARN(msg) gsLogMsg(LOG_WARN, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_ERROR(msg) gsLogMsg(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define LOG_FATAL(msg) gsLogMsg(LOG_FATAL, __FILE_FROM_REPO_ROOT__, __LINE__, msg)

#define LOG_TRACE_FROM_ISR(msg) LOG_TRACE(msg)
#define LOG_DEBUG_FROM_ISR(msg) LOG_DEBUG(msg)
#define LOG_INFO_FROM_ISR(msg) LOG_INFO(msg)
#define LOG_WARN_FROM_ISR(msg) LOG_WARN(msg)
#define LOG_ERROR_FROM_ISR(msg) LOG_ERROR(msg)
#define LOG_FATAL_FROM_ISR(msg) LOG_FATAL(msg)

#define LOG_ERROR_CODE(errCode) gsLogErrorCode(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, errCode)
#define LOG_ERROR_CODE_FROM_ISR(errCode) gsLogErrorCode(LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, errCode)

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
 * @brief Log an error code
 *
 * @param msgLevel Level of the message
 * @param file File of message
 * @param line Line of message
 * @param errCode the error code that needs to be logged
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_LOG_MSG_SILENCED if msgLevel is lower than logging level
 *                              OBC_GS_ERR_CODE_BUFF_TOO_SMALL if logged message is too long
 *                              OBC_GS_ERR_CODE_INVALID_ARG if file or s are null or if there is an encoding error
 *                              OBC_GS_ERR_CODE_SUCCESS if message is successfully logged
 *                              OBC_GS_ERR_CODE_UNKNOWN otherwise
 */
obc_gs_error_code_t gsLogErrorCode(gs_log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message
 *
 * @param msgLevel Level of the message
 * @param file File of message
 * @param line Line of message
 * @param msg the message that should be logged (MUST BE STATIC)
 * @return obc_gs_error_code_t OBC_GS_ERR_CODE_LOG_MSG_SILENCED if msgLevel is lower than logging level
 *                              OBC_GS_ERR_CODE_BUFF_TOO_SMALL if logged message is too long
 *                              OBC_GS_ERR_CODE_INVALID_ARG if file or s are null or if there is an encoding error
 *                              OBC_GS_ERR_CODE_SUCCESS if message is successfully logged
 *                              OBC_GS_ERR_CODE_UNKNOWN otherwise
 */
obc_gs_error_code_t gsLogMsg(gs_log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

  #else
  
  #define LOG_DEBUG(msg) ((void)0)
  #define LOG_INFO(msg) ((void)0)
  #define LOG_WARN(msg) ((void)0)
  #define LOG_ERROR(msg) ((void)0)
  #define LOG_FATAL(msg) ((void)0)
  #define LOG_TRACE_FROM_ISR(msg) ((void)0)
  #define LOG_DEBUG_FROM_ISR(msg) ((void)0)
  #define LOG_INFO_FROM_ISR(msg) ((void)0)
  #define LOG_WARN_FROM_ISR(msg) ((void)0)
  #define LOG_ERROR_FROM_ISR(msg) ((void)0)
  #define LOG_FATAL_FROM_ISR(msg) ((void)0)
  #define LOG_ERROR_CODE(errCode) ((void)0)
  #define LOG_ERROR_CODE_FROM_ISR(errCode) ((void)0)
  
  #define RETURN_IF_ERROR_CODE(_ret)            \
    do {                                        \
      (void)(_ret);                             \
      return _ret;                              \
    } while (0)

  #define LOG_IF_ERROR_CODE(_ret)               \
    do {                                        \
      (void)(_ret);                             \
    } while (0)
  #endif
  
  #endif