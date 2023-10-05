#pragma once

#include <FreeRTOS.h>

#include "os_task.h"
#include "obc_reset.h"

/**
 * @brief Stack overflow hook.
 * @param xTask Task reference handle.
 * @param pcTaskName Task name.
 */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
