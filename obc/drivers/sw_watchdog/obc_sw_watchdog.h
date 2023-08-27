#pragma once

#include <stdint.h>
#include <os_portmacro.h>
/**
 * Once the task start, a corresponding bit is set to 1.
 * After tasks check in, the bits will be cleared and set to 0.
 * */

typedef struct{
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
 */
void taskCheckIn(uint32_t taskNum);
