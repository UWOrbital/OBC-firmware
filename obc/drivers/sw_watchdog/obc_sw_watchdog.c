#include "obc_sw_watchdog.h"
#include "obc_assert.h"
#include "obc_privilege.h"
#include "obc_task_config.h"

#include <os_event_groups.h>
#include <system.h>
#include <reg_rti.h>
#include <rti.h>

#include <stdint.h>

// Watchdog is fed by writing these two values to the WDKEY register
#define RESET_DWD_CMD1 0xE51AUL
#define RESET_DWD_CMD2 0xA35CUL

#define DWD_CTRL_ENABLE 0xA98559DAUL

// When all task checked in
#define taskCheckinTrue (TASK1 | TASK2 | TASK3 | TASK4)

// Preload value is used to set the timeout period
#define MIN_PRELOAD_VAL 0
#define MAX_PRELOAD_VAL 0xFFF

// Assuming RTI frequency of 73.333 MHz, this
// corresponds to a timeout of ~0.45 seconds
// based on the formula: timeout = (PRELOAD_VAL + 1) * 2^13 / RTI_FREQ
#define PRELOAD_VAL 0xFBB

// Must feed the watchdog before the timeout period expires
// This value should provide some margin
#define FEEDING_PERIOD pdMS_TO_TICKS(300)

// This check does not explicitly check for 73.333 MHz, but it will fail if the RTI frequency changed too much
STATIC_ASSERT((uint32_t)RTI_FREQ == 73, "RTI frequency is not 73.333 MHz");
STATIC_ASSERT(PRELOAD_VAL >= MIN_PRELOAD_VAL && PRELOAD_VAL <= MAX_PRELOAD_VAL, "Preload value is out of range");

// Enable event flag and checks each crital task's status
EventGroupHandle_t watchdogTaskHandle;
StaticEventGroup_t watchdogEventGroup;

/**
 * @brief Software watchdog task
 */
static void swWatcdogFeeder(void* pvParameters);

/**
 * @brief Feed the software watchdog
 */
static void feedSwWatchdog(void);

void initSwWatchdog(void) {
  ASSERT(&watchdogEventGroup);
  watchdogTaskHandle = xEventGroupCreateStatic(&watchdogEventGroup);
}

static void swWatcdogFeeder(void* pvParameters) {
  // Set up the watchdog
  BaseType_t xRunningPrivileged = prvRaisePrivilege();

  rtiREG1->DWDPRLD = PRELOAD_VAL;
  rtiREG1->WWDSIZECTRL = Size_100_Percent;
  rtiREG1->WWDRXNCTRL = Generate_Reset;
  rtiREG1->DWDCTRL = DWD_CTRL_ENABLE;

  portRESET_PRIVILEGE(xRunningPrivileged);

  while (1) {
    // Check if all tasks has checked in
    uint32_t result = xEventGroupWaitBits(watchdogTaskHandle, taskCheckinTrue, pdTRUE, pdTRUE, FEEDING_PERIOD);
    if ((result & taskCheckinTrue) == taskCheckinTrue) {
      feedSwWatchdog();
    }
  }
}

static void feedSwWatchdog(void) {
  BaseType_t xRunningPrivileged = prvRaisePrivilege();
  rtiREG1->WDKEY = RESET_DWD_CMD1;
  rtiREG1->WDKEY = RESET_DWD_CMD2;
  portRESET_PRIVILEGE(xRunningPrivileged);
}
