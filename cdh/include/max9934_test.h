#ifndef CDH_INCLUDE_TEST_H_
#define CDH_INCLUDE_TEST_H_

#include <sys_common.h>

/* Test task config */
#define MAX9934_STACK_SIZE     1024
#define MAX9934_NAME           "max9934"
#define MAX9934_PRIORITY       1
#define MAX9934_DELAY_TICKS    1000/portTICK_PERIOD_MS

/**
 * @brief Initialize the max9934 task and assosiated FreeRTOS constructs (queues, timers, etc.)
 */ 
void initMAX9934(void);

#endif