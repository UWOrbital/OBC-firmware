#include "logging.h"
#include "gs_errors.h"

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define LOG_FILE_NAME "gs_log.log"

#define MAX_UINT32_STRING_SIZE 11U
#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
// Extra 10 for the small extra pieces in "%s - %s\r\n"
#define MAX_LOG_SIZE (MAX_MSG_SIZE + MAX_FNAME_LINENUM_SIZE + 10U)

static const char *LEVEL_STRINGS[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static log_level_t logLevel;

/**
 * @brief Write a message/error to a log file
 *
 * @param logBuf Buffer containing data to write to log file
 * @param logBufLen Length of buffer
 * @return gs_error_code_t - indicating whether the logging was successful.
 */
static gs_error_code_t writeToLogFile(const char logBuf[], size_t logBufLen);

void initLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

static gs_error_code_t writeToLogFile(const char logBuf[], size_t logBufLen) {
  FILE *fpointer = fopen(LOG_FILE_NAME, "a");
  if (fpointer == NULL) {
    return GS_ERR_CODE_FAILED_FILE_OPEN;
  }

  if (fwrite(logBuf, sizeof(char), logBufLen, fpointer) != logBufLen) {
    fclose(fpointer);
    return GS_ERR_CODE_FAILED_FILE_WRITE;
  }

  if (fclose(fpointer) != 0) {
    return GS_ERR_CODE_FAILED_FILE_CLOSE;
  }

  return GS_ERR_CODE_SUCCESS;
}

logger_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  logger_error_code_t returnCode = {.type = GS_ERROR_CODE};

  // Convert error code to string
  char errCodeStr[MAX_UINT32_STRING_SIZE] = {0};
  int32_t errCodeStrLen = snprintf(errCodeStr, MAX_UINT32_STRING_SIZE, "%lu", (unsigned long)errCode);
  if (errCodeStrLen < 0) {
    returnCode.gsError = GS_ERR_CODE_INVALID_ARG;
    return returnCode;
  }

  if (errCodeStrLen >= MAX_UINT32_STRING_SIZE) {
    returnCode.gsError = GS_ERR_CODE_BUFF_TOO_SMALL;
    return returnCode;
  }

  return logMsg(msgLevel, file, line, errCodeStr);
}

logger_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg) {
  logger_error_code_t returnCode = {.type = GS_ERROR_CODE};

  if (msgLevel < logLevel) {
    returnCode.gsError = GS_ERR_CODE_LOG_MSG_SILENCED;
    return returnCode;
  }

  if (file == NULL) {
    returnCode.gsError = GS_ERR_CODE_INVALID_ARG;
    return returnCode;
  }

  if (msg == NULL) {
    returnCode.gsError = GS_ERR_CODE_INVALID_ARG;
    return returnCode;
  }

  // Construct log entry
  char logBuf[MAX_LOG_SIZE] = {0};
  int32_t logBufLen = snprintf(logBuf, MAX_LOG_SIZE, "%-5s -> %s:%lu - %s\r\n", LEVEL_STRINGS[msgLevel], file,
                               (unsigned long)line, msg);
  if (logBufLen < 0) {
    returnCode.gsError = GS_ERR_CODE_INVALID_ARG;
    return returnCode;
  }

  if (logBufLen >= MAX_LOG_SIZE) {
    returnCode.gsError = GS_ERR_CODE_BUFF_TOO_SMALL;
    return returnCode;
  }

  returnCode.gsError = writeToLogFile(logBuf, logBufLen);
  return returnCode;
}
