#pragma once
#include <FreeRTOSConfig.h>

#if (configGENERATE_RUN_TIME_STATS == 1)

#include <stdint.h>
#include <obc_errors.h>

void vConfigureRuntimeStats(void);
uint32_t systemTickGet(void);

#endif