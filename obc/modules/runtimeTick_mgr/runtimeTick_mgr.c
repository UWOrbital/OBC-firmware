#include "runtimeTick_mgr.h"

#include <rti_weak.h>
#include <rti.h>
#include <stdint.h>

void vConfigureRuntimeStats(void) {
  rtiInit();
  rtiStartCounter(rtiCOUNTER_BLOCK1);
}

uint32_t vSystemTickGet(void) {
  return rtiGetCounterTick();
}