#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_sci_io.h"
#include "logger.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static log_level_t logLevel;

void initLogger(void) {
  logLevel = LOG_DEFAULT_LEVEL;
  outputLocation = LOG_DEFAULT_OUTPUT_LOCATION;
}

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
  if (msgLevel < logLevel) return OBC_ERR_CODE_LOG_MSG_SILENCED;

  if (file == NULL || s == NULL) return OBC_ERR_CODE_INVALID_ARG;

  int ret = 0;

  // Message
  char msgbuf[MAX_MSG_SIZE] = {0};
  va_list args;
  va_start(args, s);
  ret = vsnprintf(msgbuf, MAX_MSG_SIZE, s, args);
  va_end(args);
  if (ret < 0) return OBC_ERR_CODE_INVALID_ARG;
  if ((uint32_t)ret >= MAX_MSG_SIZE) return OBC_ERR_CODE_BUFF_TOO_SMALL;

  if (outputLocation == LOG_TO_UART) {
    // File & line number
    char infobuf[MAX_FNAME_LINENUM_SIZE] = {0};
    ret = snprintf(infobuf, MAX_FNAME_LINENUM_SIZE, "%-5s -> %s:%lu", LEVEL_STRINGS[msgLevel], file, line);
    if (ret < 0) return OBC_ERR_CODE_INVALID_ARG;
    if ((uint32_t)ret >= MAX_FNAME_LINENUM_SIZE) return OBC_ERR_CODE_BUFF_TOO_SMALL;

    // Prepare entire output
    char buf[MAX_LOG_SIZE] = {0};
    ret = snprintf(buf, MAX_LOG_SIZE, "%s - %s\r\n", infobuf, msgbuf);
    if (ret < 0) return OBC_ERR_CODE_INVALID_ARG;
    if ((uint32_t)ret >= MAX_LOG_SIZE) return OBC_ERR_CODE_BUFF_TOO_SMALL;
    obc_error_code_t retSci = sciPrintText((unsigned char *)buf, sizeof(buf), UART_MUTEX_BLOCK_TIME);
    return retSci;
  } else if (outputLocation == LOG_TO_SDCARD) {
    // Prepare entire output
    logger_event_t logEvent = {.logType = LOG_TYPE_ERROR};
    ret = snprintf((char *)logEvent.msg, LOG_MESSAGE_MAX_BYTES, "%s:%s\r\n", file, msgbuf);
    if (ret < 0) return OBC_ERR_CODE_INVALID_ARG;
    if ((uint32_t)ret >= MAX_SD_CARD_LOG_SIZE - 1)
      return OBC_ERR_CODE_BUFF_TOO_SMALL;  // subtract 1 from max log size to account for null character
    sendToLoggerQueue(&logEvent);
  }

  return OBC_ERR_CODE_UNKNOWN;
}
