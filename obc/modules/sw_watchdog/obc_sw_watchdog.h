#pragma once

#include "obc_scheduler_config.h"

#include <stdint.h>
#include <os_portmacro.h>

/**
 * @brief Initialize Digital Windowed Watchdog
 */
void initSwWatchdog(void);

/**
 * @brief Check in to ensure the watchdog knows the task has not timed out
 * @param taskNum number to be registered
 */
void taskCheckInToWatchdog(obc_scheduler_config_id_t taskNum);
