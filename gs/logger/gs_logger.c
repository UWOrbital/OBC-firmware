#include "gs_logger.h"
#include "gs_errors.h"

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_UINT32_STRING_SIZE 11U

static const char *LEVEL_STRINGS[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static log_level_t logLevel;

void gsInitLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

gs_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  // Convert error code to string
  char errCodeStr[MAX_UINT32_STRING_SIZE] = {0};
  snprintf(errCodeStr, MAX_UINT32_STRING_SIZE, "%lu", (unsigned long)errCode);

  return logMsg(msgLevel, file, line, errCodeStr);
}

gs_error_code_t logMsg(log_level_t msgLevel, const char *file, uint32_t line, const char *msg) {
  if (msgLevel < logLevel) {
    return GS_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return GS_ERR_CODE_INVALID_ARG;
  }

  if (msg == NULL) {
    return GS_ERR_CODE_INVALID_ARG;
  }

  printf("%-5s -> %s:%lu - %s\r\n", LEVEL_STRINGS[msgLevel], file, (unsigned long)line, msg);

  return GS_ERR_CODE_SUCCESS;
}
