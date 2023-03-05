#ifndef _DRIVERS_INCLUDE_OBC_WATCHDOG_H_
#define _DRIVERS_INCLUDE_OBC_WATCHDOG_H_


/**
 * @brief Initialize Digital Windowed Watchdog
*/
obc_error_code_t initDWWD(double_t tExp);

/**
 * @brief Initialize DWWDTask
*/
obc_error_code_t initDWWDTask(void);

#endif //_DRIVERS_INCLUDE_OBC_WATCHDOG_H_