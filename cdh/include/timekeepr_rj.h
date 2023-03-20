#ifndef CDH_INCLUDE_TIMEKEEPER_RJ_H_
#define CDH_INCLUDE_TIMEKEEPER_RJ_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"
#include <time.h>

time_t currentTimekeepingTask(void *pvParameters);

#endif /*CDH_INCLUDE_TIMEKEEPER_RJ_H_*/