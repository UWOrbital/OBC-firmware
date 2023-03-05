#ifndef _DRIVERS_INCLUDE_OBC_WATCHDOG_H_
#define _DRIVERS_INCLUDE_OBC_WATCHDOG_H_

#include "obc_errors.h"
#include <math.h>

/**
 * @brief Initialize Digital Windowed Watchdog
*/
obc_error_code_t initDWWD(float tExp);

/**
 * @brief Initialize DWWDTask
*/
obc_error_code_t initDWWDTask(void);

#endif //_DRIVERS_INCLUDE_OBC_WATCHDOG_H_