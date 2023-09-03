#pragma once

#if (configGENERATE_RUN_TIME_STATS == 1)

#include <stdint.h>

/**
 * @brief Returns the free counter tick for RTI 1
 *
 * @return A 32-bit tick value for the Free Counter RTI 1
 */
uint32_t rtiGetCounterTick();

#endif