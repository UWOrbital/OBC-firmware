#pragma once

#include "obc_errors.h"

#define TPL5010_WAKE_PIN 0
#define TPL5010_DONE_PIN 1

/**
 * @brief Initialize HW Watchdog driver
 */
void tpl5010Init(void);

/**
 * @brief Reset the done signal on interrupt
 */
void feedHardwareWatchdog(void);
