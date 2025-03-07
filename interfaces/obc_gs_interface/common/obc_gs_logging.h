#pragma once 

#include "obc_gs_errors.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#define __FILE_FROM_REPO_ROOT__ \
    (strstr(__FILE__, SOURCE_PATH) ? strstr(__FILE__, SOURCE_PATH) + sizeof(SOURCE_PATH) - 1 : __FILE__)

/**
 * @enum gs_log_output_location_t
 * @brief GS log output location enum.
 *
 * Enum containing all locations to output gs_logs to.
 */
typedef enum { GS_LOG_TO_SDCARD, GS_LOG_TO_UART } gs_log_output_location_t;

#ifndef GS_LOG_DEFAULT_OUTPUT_LOCATION
#define GS_LOG_DEFAULT_OUTPUT_LOCATION GS_LOG_TO_UART
#endif

/**
 * @enum gs_log_level_t
 * @brief GS log levels enum.
 * 
 * Enum containing all log levels
 */

typedef enum { GS_LOG_TRACE, GS_LOG_DEBUG, GS_LOG_INFO, GS_LOG_WARN, GS_LOG_ERROR, GS_LOG_FATAL, GS_LOG_OFF } gs_log_level_t;

//Define a bit field structure for the log type and log level
typedef struct {
    unsigned int logType : 1; // 1 bit for log type (0 for GS_LOG_TYPE_ERROR_CODE, 1 for LOG_TYPE_MSG)
    unsigned int logLevel : 3; // 3 bits for log level 
} gs_log_entry_t;

//Define the log enum based on the bit field structure
typedef enum { GS_LOG_TYPE_ERROR_CODE = 0, GS_LOG_TYPE_MSG = 1 } gs_log_type_t;

#ifndef GS_LOG_DEFAULT_LEVEL
#define GS_LOG_DEFAULT_LEVEL GS_LOG_TRACE
#endif

//To use the correct logging for OBC_FIRMWARE and OBC_GS builds, 
//there needs to be a specific defintion for the build in the file that includes this header
// #define BUILD_TYPE_OBC_FIRMWARE or #define BUILD_TYPE_OBC_GS or nothing if not defined


//Logging condtionally, based on if the build type is OBC_FIRMWARE or OBC_GS
#ifdef BUILD_TYPE_OBC_FIRMWARE
  //Forward GS logging to the firmware logging module
  #include "obc_logging.h"

  #define GS_LOG_TRACE(msg)    LOG_TRACE(msg)
  #define GS_LOG_DEBUG(msg)    LOG_DEBUG(msg)
  #define GS_LOG_INFO(msg)     LOG_INFO(msg)
  #define GS_LOG_WARN(msg)     LOG_WARN(msg)
  #define GS_LOG_ERROR(msg)    LOG_ERROR(msg)
  #define GS_LOG_FATAL(msg)    LOG_FATAL(msg)

  #define GS_LOG_TRACE_FROM_ISR(msg)    LOG_TRACE_FROM_ISR(msg)
  #define GS_LOG_DEBUG_FROM_ISR(msg)    LOG_DEBUG_FROM_ISR(msg)
  #define GS_LOG_INFO_FROM_ISR(msg)     LOG_INFO_FROM_ISR(msg)
  #define GS_LOG_WARN_FROM_ISR(msg)     LOG_WARN_FROM_ISR(msg)
  #define GS_LOG_ERROR_FROM_ISR(msg)    LOG_ERROR_FROM_ISR(msg)
  #define GS_LOG_FATAL_FROM_ISR(msg)    LOG_FATAL_FROM_ISR(msg)

  #define GS_LOG_ERROR_CODE(errCode)    LOG_ERROR_CODE(errCode)
  #define GS_LOG_ERROR_CODE_FROM_ISR(errCode) LOG_ERROR_CODE_FROM_ISR(errCode)

#define GS_RETURN_IF_ERROR_CODE(_ret)          \
    do {                                       \
      obc_gs_error_code_t errCode = _ret;        \
      if (errCode != OBC_GS_ERR_CODE_SUCCESS) {  \
        GS_LOG_ERROR_CODE(errCode);            \
        return errCode;                        \
  }                                            \
} while (0)

#define GS_LOG_IF_ERROR_CODE(_ret)              \
    do {                                        \
      obc_gs_error_code_t errCode = _ret;       \
      if (errCode != OBC_GS_ERR_CODE_SUCCESS) { \
        GS_LOG_ERROR_CODE(errCode);             \
      }                                         \
    } while (0)

#else 

#ifdef BUILD_TYPE_OBC_GS
//these functions are for when the buildtype is defined to be BUILD_TYPE_OBC_GS

/**
 * @brief Set the logging level
 *
 * @param newLogLevel The new logging level
 */
void gsLogSetLevel(gs_log_level_t newLogLevel);

/**
 * @brief Log an error code
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode               the error code that needs to be logged
 * @return obc_gs_error_code_t	OBC_GS_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_GS_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_GS_ERR_CODE_INVALID_ARG		    if file or s are null or if there is an encoding error
 * 								OBC_GS_ERR_CODE_SUCCESS			    if message is successfully logged
 * 								OBC_GS_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_gs_error_code_t gsLogErrorCode(gs_log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg                   the message that should be logged (MUST BE STATIC)
 * @return obc_gs_error_code_t	OBC_GS_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_GS_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_GS_ERR_CODE_INVALID_ARG		    if file or s are null or if there is an encoding error
 * 								OBC_GS_ERR_CODE_SUCCESS			    if message is successfully logged
 * 								OBC_GS_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_gs_error_code_t gsLogMsg(gs_log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

/**
 * @brief Log an error code from ISR
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param errCode               the error code that needs to be logged
 * @return obc_gs_error_code_t	OBC_GS_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_GS_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_GS_ERR_CODE_INVALID_ARG		    if file or s are null or if there is an encoding error
 * 								OBC_GS_ERR_CODE_SUCCESS			    if message is successfully logged
 * 								OBC_GS_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_gs_error_code_t gsLogErrorCodeFromISR(gs_log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode);

/**
 * @brief Log a message from ISR
 *
 * @param msgLevel				Level of the message
 * @param file					File of message
 * @param line					Line of message
 * @param msg                   the message that should be logged (MUST BE STATIC)
 * @return obc_gs_error_code_t	OBC_GS_ERR_CODE_LOG_MSG_SILENCED 	if msgLevel is lower than logging level
 * 								OBC_GS_ERR_CODE_BUFF_TOO_SMALL		if logged message is too long
 * 								OBC_GS_ERR_CODE_INVALID_ARG		    if file or s are null or if there is an encoding error
 * 								OBC_GS_ERR_CODE_SUCCESS			    if message is successfully logged
 * 								OBC_GS_ERR_CODE_UNKNOWN 			otherwise
 *
 */
obc_gs_error_code_t gsLogMsgFromISR(gs_log_level_t msgLevel, const char *file, uint32_t line, const char *msg);

#define GS_LOG_TRACE(msg)    gsLogMsg(GS_LOG_TRACE, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_DEBUG(msg)    gsLogMsg(GS_LOG_DEBUG, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_INFO(msg)     gsLogMsg(GS_LOG_INFO, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_WARN(msg)     gsLogMsg(GS_LOG_WARN, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_ERROR(msg)    gsLogMsg(GS_LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_FATAL(msg)    gsLogMsg(GS_LOG_FATAL, __FILE_FROM_REPO_ROOT__, __LINE__, msg)

#define GS_LOG_TRACE_FROM_ISR(msg)    gsLogMsgFromISR(GS_LOG_TRACE, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_DEBUG_FROM_ISR(msg)    gsLogMsgFromISR(GS_LOG_DEBUG, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_INFO_FROM_ISR(msg)     gsLogMsgFromISR(GS_LOG_INFO, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_WARN_FROM_ISR(msg)     gsLogMsgFromISR(GS_LOG_WARN, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_ERROR_FROM_ISR(msg)    gsLogMsgFromISR(GS_LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, msg)
#define GS_LOG_FATAL_FROM_ISR(msg)    gsLogMsgFromISR(GS_LOG_FATAL, __FILE_FROM_REPO_ROOT__, __LINE__, msg)

#define GS_LOG_ERROR_CODE(errCode)    gsLogErrorCode(GS_LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, errCode)
#define GS_LOG_ERROR_CODE_FROM_ISR(errCode) gsLogErrorCodeFromISR(GS_LOG_ERROR, __FILE_FROM_REPO_ROOT__, __LINE__, errCode)

#define GS_RETURN_IF_ERROR_CODE(_ret)        \
do {                                         \
  obc_gs_error_code_t errCode = _ret;        \
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {  \
    GS_LOG_ERROR_CODE(errCode);              \
    return errCode;                          \
  }                                          \
} while (0)

#define GS_LOG_IF_ERROR_CODE(_ret)           \
do {                                         \
  obc_gs_error_code_t errCode = _ret;        \
  if (errCode != OBC_GS_ERR_CODE_SUCCESS) {  \
    GS_LOG_ERROR_CODE(errCode);              \
  }                                          \
} while (0)

#else
  //when no build type is defined in the file

  #define GS_LOG_TRACE(msg)                     ((void)0)
  #define GS_LOG_DEBUG(msg)                     ((void)0)
  #define GS_LOG_INFO(msg)                      ((void)0)
  #define GS_LOG_WARN(msg)                      ((void)0)
  #define GS_LOG_ERROR(msg)                     ((void)0)
  #define GS_LOG_FATAL(msg)                     ((void)0)
  #define GS_LOG_TRACE_FROM_ISR(msg)            ((void)0)
  #define GS_LOG_DEBUG_FROM_ISR(msg)            ((void)0)
  #define GS_LOG_INFO_FROM_ISR(msg)             ((void)0)
  #define GS_LOG_WARN_FROM_ISR(msg)             ((void)0)
  #define GS_LOG_ERROR_FROM_ISR(msg)            ((void)0)
  #define GS_LOG_FATAL_FROM_ISR(msg)            ((void)0)
  #define GS_LOG_ERROR_CODE(errCode)            ((void)0)
  #define GS_LOG_ERROR_CODE_FROM_ISR(errCode)   ((void)0)

  #define GS_RETURN_IF_ERROR_CODE(_ret)          \
    do {                                         \
      obc_gs_error_code_t errCode = _ret;        \
      if (errCode != OBC_GS_ERR_CODE_SUCCESS) {  \
        return errCode;                          \
      }                                          \
    } while (0)
  #define GS_LOG_IF_ERROR_CODE(_ret)             \
    do {                                         \
      obc_gs_error_code_t errCode = _ret;        \
      if (errCode != OBC_GS_ERR_CODE_SUCCESS) {  \
      }                                          \
    } while (0)

#endif
#endif