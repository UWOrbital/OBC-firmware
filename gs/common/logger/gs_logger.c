#include "gs_logger.h"
#include "gs_errors.h"

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static log_level_t logLevel;

void gsInitLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

gs_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  if (msgLevel < logLevel) {
    return GS_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return GS_ERR_CODE_INVALID_ARG;
  }

  printf("Log Level %d:  %s:%lu:    Error Code %lu\n", msgLevel, file, (unsigned long)line, (unsigned long)errCode);

  return GS_ERR_CODE_SUCCESS;
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

  printf("Log Level %d:  %s:%lu:    %s\n", msgLevel, file, (unsigned long)line, msg);

  return GS_ERR_CODE_SUCCESS;
}
