#pragma once

#if (configGENERATE_RUN_TIME_STATS == 1)

#include <stdint.h>
#include <obc_errors.h>

#ifndef portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
  #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureRuntimeStats()
#endif

#ifndef portGET_RUN_TIME_COUNTER_VALUE()
  #define portGET_RUN_TIME_COUNTER_VALUE() systemTickGet()
#endif

void vConfigureRuntimeStats(void);
uint32_t systemTickGet(void);

#endif