#pragma once
#include <FreeRTOSConfig.h>

#if (ENABLE_TASK_STATS_COLLECTOR == 1)

#include <stdint.h>
#include <obc_errors.h>

void vConfigureRuntimeStats(void);
uint32_t systemTickGet(void);

#endif
