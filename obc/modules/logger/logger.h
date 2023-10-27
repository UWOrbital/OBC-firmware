#pragma once

#include "obc_errors.h"
#include "obc_logging.h"

#include <stdio.h>

typedef struct {
  log_entry_t logEntry;
  const char *file;
  uint32_t line;
  union {
    uint32_t errCode;
    const char *msg;
  };
} logger_event_t;

/**
 * @brief Set the output location
 *
 * @param newOutputLocation The new output location
 */
void logSetOutputLocation(log_output_location_t newOutputLocation);
