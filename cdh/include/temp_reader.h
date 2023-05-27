#ifndef CDH_INCLUDE_TEMP_READER_H_
#define CDH_INCLUDE_TEMP_READER_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include <os_timer.h>
#include <sys_common.h>
#include <obc_time.h>

/**
 * @brief Initialize the temp read handler handler task and associated FreeRTOS constructs (queues, timers, etc.)
 * 
 */
void initTempReaderHandler(void);

/**
 * @brief Call back function for the freeRTOS timer created to read temperature periodically
 * 
 */
void vTimerCallback( TimerHandle_t xTimer );

#endif /* CDH_INCLUDE_TEMP_READER_H_ */