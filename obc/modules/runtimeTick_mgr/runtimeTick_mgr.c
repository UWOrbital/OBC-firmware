#include "runtimeTick_mgr.h"

#include <obc_errors.h>

#include <rti.h>
#include <stdint.h>

static uint32_t counter = 0;
static uint32_t period = 0;

obc_error_code_t setRtiCounter(uint32_t counterBlock, uint32_t period) {
  if (counter > 1) return OBC_ERR_CODE_INVALID_ARG;
  counter = counterBlock;
}

void vConfigureRuntimeStats(void) {
  rtiInit();
  rtiStartCounter(counter);
  rtiSetPeriod(counter, period);
}

uint64_t vSystemTickGet(void) {
  return rtiGetCurrentTick(counter);
}