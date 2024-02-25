#include "obc_gs_logger.h"
#include "obc_gs_errors.h"

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static log_level_t logLevel;

void obcGsInitLogger(void) { logLevel = LOG_DEFAULT_LEVEL; }

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

obc_gs_error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  if (msgLevel < logLevel) {
    return OBC_GS_ERR_CODE_LOG_MSG_SILENCED;
  }

  if (file == NULL) {
    return OBC_GS_ERR_CODE_INVALID_ARG;
  }

  printf("Log Level %d:  %s:%d:    Error Code %d\n", msgLevel, file, line, errCode);

  return OBC_GS_ERR_CODE_SUCCESS;
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

  printf("Log Level %d:  %s:%d:    %s\n", msgLevel, file, line, msg);

  return OBC_GS_ERR_CODE_SUCCESS;
}
