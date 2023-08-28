#pragma once

#if (DEBUG == 1)
#include <stdint.h>
#include <obc_errors.h>

void vConfigureRuntimeStats(void);
uint32_t vSystemTickGet(void);

#endif