#include "runtime_stats.h"

#include <rti.h>
#include <stdint.h>

void vConfigureRuntimeStats(void) {
  rtiInit();
  rtiStartCounter(rtiCOUNTER_BLOCK1);
}

uint32_t ulSystemTickGet(void) { return rtiGetCounterOneTick(); }
