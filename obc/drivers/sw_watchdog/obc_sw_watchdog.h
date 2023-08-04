#pragma once

#include <stdint.h>
/**
 * Once the task start, a corresponding bit is set to 1.
 * After tasks check in, the bits will be cleared and set to 0.
 * */
// To-do: Register some critical tasks instead of exa
#define TASK1 0x01
#define TASK2 0x02
#define TASK3 0x04
#define TASK4 0x08

#define EvenGroupHandle_t watchdogTaskHandle
/**
 * @brief Initialize Digital Windowed Watchdog
 */
void initSwWatchdog(void);

/**
 * @brief Set the taskNum bit to indicate the task is running
 */
void taskCheckOut(uint32_t taskNum);

/**
 * @brief Clear the taskNum bit to indicate the task is done/not running
 */
void taskCheckIn(uint32_t taskNum);
