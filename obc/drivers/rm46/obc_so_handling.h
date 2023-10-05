#pragma once

#include "os_task.h"
#include "obc_reset.h"

#include <FreeRTOS.h>

/**
 * @brief Stack overflow hook.
 * @param xTask Task reference handle.
 * @param pcTaskName Task name.
 * @returns Void.
 */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
