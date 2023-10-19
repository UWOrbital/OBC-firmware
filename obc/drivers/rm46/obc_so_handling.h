#pragma once

#include <FreeRTOS.h>

#include "os_task.h"

/**
 * @brief Hook that logs stack overflow occurrences.
 * @param xTask Task reference handle.
 * @param pcTaskName Task name.
 */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
