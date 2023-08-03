#pragma once

/**
 * Once a task starts, the corresponding event bit will sync to 0.
 * Otherwise, it should stay as 1.
 *
 * Syncing can be done as the following:
 *      xEventGroupSetBits(watchdogTaskHandle, TASK1)
 * */
#define TASK1 0x01
#define TASK2 0x02
#define TASK3 0x04
#define TASK4 0x08

#define EvenGroupHandle_t watchdogTaskHandle
/**
 * @brief Initialize Digital Windowed Watchdog
 */
void initSwWatchdog(void);
