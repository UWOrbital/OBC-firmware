#include "logger.h"
#include "obc_errors.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static log_level_t logLevel;
static log_output_location_t outputLocation;

void initLogger(void) {
  logLevel = LOG_DEFAULT_LEVEL;
  outputLocation = LOG_DEFAULT_OUTPUT_LOCATION;
}

void logSetLevel(log_level_t newLogLevel) { logLevel = newLogLevel; }

void logSetOutputLocation(log_output_location_t newOutputLocation) { outputLocation = newOutputLocation; }

error_code_t logErrorCode(log_level_t msgLevel, const char *file, uint32_t line, uint32_t errCode) {
  if (msgLevel < logLevel) {
    error_code_t returnCode = {.type = OBC_ERROR_CODE, .obcError = OBC_ERR_CODE_LOG_MSG_SILENCED};
    return returnCode;
  }

  if (file == NULL) {
    error_code_t returnCode = {.type = OBC_ERROR_CODE, .obcError = OBC_ERR_CODE_INVALID_ARG};
    return returnCode;
  }

  error_code_t returnCode = {.type = OBC_ERROR_CODE, .obcError = OBC_ERR_CODE_SUCCESS};
  return returnCode;
}
