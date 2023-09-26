#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_sci_io.h"
#include "logger.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static log_level_t logLevel;

void initLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

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
obc_error_code_t logError(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  if (msgLevel < logLevel) {
    return OBC_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  logger_event_t logEvent = {.logType = msgLevel, .file = file, .line = line, .errCode = errCode};

  // send the event to the logger queue and don't try to log any error that occurs
  return sendToLoggerQueue(&logEvent);
}

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
obc_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg) {
  if (msgLevel < logLevel) {
    return OBC_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (msg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  logger_event_t logEvent = {.logType = msgLevel, .file = file, .line = line, .msg = msg};

  return sendToLoggerQueue(&logEvent);
}

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
obc_error_code_t logErrorFromISR(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  if (msgLevel < logLevel) {
    return OBC_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  logger_event_t logEvent = {.logType = msgLevel, .file = file, .line = line, .errCode = errCode};

  // send the event to the logger queue and don't try to log any error that occurs
  return sendToLoggerQueueFromISR(&logEvent);
}

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
obc_error_code_t logMsgFromISR(log_level_t msgLevel, const char *file, uint32_t line, const char *msg) {
  if (msgLevel < logLevel) {
    return OBC_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (msg == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  logger_event_t logEvent = {.logType = msgLevel, .file = file, .line = line, .msg = msg};

  return sendToLoggerQueueFromISR(&logEvent);
}
