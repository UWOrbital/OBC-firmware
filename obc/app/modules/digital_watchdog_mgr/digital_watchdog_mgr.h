#pragma once

#include "obc_scheduler_config.h"

/**
 * @brief Check in to ensure the watchdog knows the task has not timed out
 * @param taskNum number to be registered
 */
void digitalWatchdogTaskCheckIn(obc_scheduler_config_id_t taskNum);
