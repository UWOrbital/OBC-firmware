#pragma once

#include "os_task.h"

#include <FreeRTOS.h>

/**
 * @brief Hook that logs stack overflow occurrences.
 * @param xTask Task reference handle.
 * @param pcTaskName Task name.
 */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
