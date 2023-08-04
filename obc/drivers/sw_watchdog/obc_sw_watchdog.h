#pragma once

#include <stdint.h>
/**
 * Once the task start, a corresponding bit is set to 1.
 * After tasks check in, the bits will be cleared and set to 0.
 * */
// To-do: Register some critical tasks instead of exa
#define TASK1 0x01U
#define TASK2 0x02U
#define TASK3 0x04U
#define TASK4 0x08U

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
