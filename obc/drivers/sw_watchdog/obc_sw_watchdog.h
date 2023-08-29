#pragma once

#include <stdint.h>
#include <os_portmacro.h>
/**
 * Once the task start, a corresponding bit is set to 1.
 * After tasks check in, the bits will be cleared and set to 0.
 * */

// Task will register with Timeout Tick and their task number.
typedef struct {
  TickType_t taskTimeOut;
  TickType_t taskLastCheckIn;
  uint8_t taskNum;
} watchdog_task_t;

#define EvenGroupHandle_t watchdogTaskHandle
/**
 * @brief Initialize Digital Windowed Watchdog
 */
void initSwWatchdog(void);

/**
 * @brief Clear the taskNum bit to indicate the task is done/not running
 * @param task number to be registered
 */
void taskCheckIn(uint32_t taskNum);

/**
 * @brief Register a task that watchdog will look over
 * @param taskNum task number to be registered
 * @param taskTimeOut a number of tick counts that the task should respond
 */
void taskRegister(uint32_t taskNum, TickType_t taskTimeOut);
