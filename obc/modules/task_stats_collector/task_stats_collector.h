#pragma once
#include <FreeRTOSConfig.h>

#if (configGENERATE_RUN_TIME_STATS == 1)
/**
 * @brief Initialize the task stats collector for debug
 */
void initTaskStatsCollector(void);
#endif
