#ifndef _DRIVERS_INCLUDE_OBC_WATCHDOG_H_
#define _DRIVERS_INCLUDE_OBC_WATCHDOG_H_

#define DWWD_NAME "Digital Windowed Watchdog"
#define DWWD_STACK_SIZE 128
#define DWWD_PRIORITY 1

/**
 * @brief Initialize Digital Windowed Watchdog
*/
void initDWWD();

/**
 * @brief Initialize DWWDTask
*/
uint8_t initDWWDTask();

#endif //_DRIVERS_INCLUDE_OBC_WATCHDOG_H_