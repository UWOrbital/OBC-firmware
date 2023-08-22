#pragma once

#include "obc_errors.h"

/**
 * @brief Initialize HW Watchdog driver
 */
void tpl5010Init(void);

/**
 * @brief Reset the done signal on interrupt
 */
void feedHardwareWatchdog(void);
