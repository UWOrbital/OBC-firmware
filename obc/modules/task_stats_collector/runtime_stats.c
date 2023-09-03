#if (configGENERATE_RUN_TIME_STATS == 1)

#include "runtime_stats.h"

#include <rti_weak.h>
#include <rti.h>
#include <stdint.h>


/**
 * @brief Helper function to configure the stats collecter.
 * @return Returns void 
 */
void vConfigureRuntimeStats(void) {
  rtiInit();
  rtiStartCounter(rtiCOUNTER_BLOCK1);
}

/**
 * @brief Returns the tick count.  
 * @return Returns the configured timer tick. 
 */
uint32_t systemTickGet(void) {
  return rtiGetCounterTick();
}


#endif