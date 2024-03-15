#include "logger.h"
#include "obc_gs_errors.h"

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define OBC_GS_LOG_FILE_NAME "obc_gs_log.log"

#define MAX_UINT32_STRING_SIZE 11U
#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
// Extra 10 for the small extra pieces in "%s - %s\r\n"
#define MAX_LOG_SIZE (MAX_MSG_SIZE + MAX_FNAME_LINENUM_SIZE + 10U)

static const char *LEVEL_STRINGS[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static log_level_t logLevel;

#define LOGGER_QUEUE_TX_WAIT_PERIOD 0

/**
 * @brief Write a message/error to a log file
 *
 * @param logBuf Buffer containing data to write to log file
 * @param logBufLen Length of buffer
 * @return obc_gs_error_code_t - indicating whether the logging was successful.
 */
static obc_gs_error_code_t writeToLogFile(char logBuf[], int logBufLen);

void initLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

static obc_gs_error_code_t writeToLogFile(char logBuf[], int logBufLen) {
  FILE *fpointer = fopen(OBC_GS_LOG_FILE_NAME, "a");
  if (fpointer == NULL) {
    return OBC_GS_ERR_CODE_FAILED_FILE_OPEN;
  }
  if (fwrite(logBuf, sizeof(char), logBufLen, fpointer) != (size_t)logBufLen) {
    fclose(fpointer);
    return OBC_GS_ERR_CODE_FAILED_FILE_WRITE;
  }
  if (fclose(fpointer) != 0) {
    return OBC_GS_ERR_CODE_FAILED_FILE_CLOSE;
  }

  return OBC_GS_ERR_CODE_SUCCESS;
}

error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  printf("OBC GS\n");
// If build type is OBC, send event to logger queue, otherwise log to file
#ifdef BOARD_TYPE
  if (msgLevel < logLevel) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_LOG_MSG_SILENCED};
    return returnCode;
  }

  if (file == NULL) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_INVALID_ARG};
    return returnCode;
  }

  logger_event_t logEvent = {.logEntry = {.logType = LOG_TYPE_ERROR_CODE, .logLevel = msgLevel},
                             .file = file,
                             .line = line,
                             .errCode = errCode};

  error_code_t returnCode = {.type = OBC_ERROR_CODE,
                             .obcError = sendToLoggerQueue(&logEvent, LOGGER_QUEUE_TX_WAIT_PERIOD)};
  return returnCode;
#else
  // Convert error code to string
  char errCodeStr[MAX_UINT32_STRING_SIZE] = {0};
  int errCodeStrLen = 0;
  errCodeStrLen = snprintf(errCodeStr, MAX_UINT32_STRING_SIZE, "%lu", (unsigned long)errCode);
  if (errCodeStrLen < 0) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_INVALID_ARG};
    return returnCode;
  }
  if ((uint32_t)errCodeStrLen >= MAX_UINT32_STRING_SIZE) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_BUFF_TOO_SMALL};
    return returnCode;
  }

  return logMsg(msgLevel, file, line, errCodeStr);
#endif
}

error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg) {
  if (msgLevel < logLevel) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_LOG_MSG_SILENCED};
    return returnCode;
  }

  if (file == NULL) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_INVALID_ARG};
    return returnCode;
  }

  if (msg == NULL) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_INVALID_ARG};
    return returnCode;
  }

// If build type is OBC, send event to logger queue, otherwise log to file
#ifdef BOARD_TYPE
  logger_event_t logEvent = {
      .logEntry = {.logType = LOG_TYPE_MSG, .logLevel = msgLevel}, .file = file, .line = line, .msg = msg};

  error_code_t returnCode = {.type = OBC_ERROR_CODE,
                             .obcError = sendToLoggerQueue(&logEvent, LOGGER_QUEUE_TX_WAIT_PERIOD)};
  return returnCode;
#else
  // Construct log entry
  char logBuf[MAX_LOG_SIZE] = {0};
  int logBufLen = 0;
  logBufLen = snprintf(logBuf, MAX_LOG_SIZE, "%-5s -> %s:%lu - %s\r\n", LEVEL_STRINGS[msgLevel], file,
                       (unsigned long)line, msg);
  if (logBufLen < 0) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_INVALID_ARG};
    return returnCode;
  }
  if ((uint32_t)logBufLen >= MAX_LOG_SIZE) {
    error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = OBC_GS_ERR_CODE_BUFF_TOO_SMALL};
    return returnCode;
  }

  error_code_t returnCode = {.type = OBC_GS_ERROR_CODE, .obcGsError = writeToLogFile(logBuf, logBufLen)};
  return returnCode;
#endif
}
