#ifndef CDH_INCLUDE_HEALTH_COLLECTOR_H_
#define CDH_INCLUDE_HEALTH_COLLECTOR_H_

#include "obc_errors.h"

/**
 * @brief Initialize the health collector task. This collects health data
 * periodically and sends it to the telemetry manager.
 */
void initHealthCollector(void);

#endif /* CDH_INCLUDE_HEALTH_COLLECTOR_H_ */
