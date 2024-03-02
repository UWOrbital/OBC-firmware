#include "obc_gs_logger.h"
#include "obc_gs_errors.h"

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define LOG_FILE_NAME "obc_gs_log.log"

#define MAX_MSG_SIZE 128U
#define MAX_FNAME_LINENUM_SIZE 128U
// Extra 10 for the small extra pieces in "%s - %s\r\n"
#define MAX_LOG_SIZE (MAX_MSG_SIZE + MAX_FNAME_LINENUM_SIZE + 10U)

static const char *LEVEL_STRINGS[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static log_level_t logLevel;

void obcGsInitLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

obc_gs_error_code_t writeToLogFile(char logBuf[], int logBufLen) {
  FILE *fpointer = fopen(LOG_FILE_NAME, "a");
  if (fpointer == NULL) {
    printf("opening file\n");
    return OBC_GS_ERR_CODE_UNKNOWN;
  }
  if (fwrite(logBuf, sizeof(char), logBufLen, fpointer) != (size_t)logBufLen) {
    return OBC_GS_ERR_CODE_UNKNOWN;
  }
  if (fclose(fpointer) != 0) {
    return OBC_GS_ERR_CODE_UNKNOWN;
  }

  return OBC_GS_ERR_CODE_SUCCESS;
}

obc_gs_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  if (msgLevel < logLevel) {
    return OBC_GS_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  // Construct log entry
  char logBuf[MAX_LOG_SIZE] = {0};
  int logBufLen = 0;
  logBufLen = snprintf(logBuf, MAX_LOG_SIZE, "%-5s -> %s:%lu - %lu\r\n", LEVEL_STRINGS[msgLevel], file,
                       (unsigned long)line, (unsigned long)errCode);
  if (logBufLen < 0) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  if ((uint32_t)logBufLen >= MAX_LOG_SIZE) {
    return OBC_GS_ERR_CODE_BUFF_TOO_SMALL;
  }

  return writeToLogFile(logBuf, logBufLen);
}

obc_gs_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg) {
  if (msgLevel < logLevel) {
    return OBC_GS_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  if (msg == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  // Construct log entry
  char logBuf[MAX_LOG_SIZE] = {0};
  int logBufLen = 0;
  logBufLen = snprintf(logBuf, MAX_LOG_SIZE, "%-5s -> %s:%lu - %s\r\n", LEVEL_STRINGS[msgLevel], file,
                       (unsigned long)line, msg);
  if (logBufLen < 0) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }
  if ((uint32_t)logBufLen >= MAX_LOG_SIZE) {
    return OBC_GS_ERR_CODE_BUFF_TOO_SMALL;
  }

  return writeToLogFile(logBuf, logBufLen);
}
