#pragma once

#include "obc_errors.h"

/**
 * @brief Initialize the health collector task. This collects health data
 * periodically and sends it to the telemetry manager.
 */
void initHealthCollector(void);
