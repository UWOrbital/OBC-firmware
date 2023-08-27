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

static StackType_t watchdogStack[SW_WATCHDOG_STACK_SIZE];
static StaticTask_t watchdogTaskBuffer;
static TaskHandle_t watchdogTaskHandle;

// Enable event flag and checks each crital task's status
static EventGroupHandle_t watchdogEventHandle;
static StaticEventGroup_t watchdogEventGroup;

static watchdog_task_t taskArray[8];

/**
 * @brief Software watchdog task
 */
static void swWatcdogFeeder(void* pvParameters);

/**
 * @brief Feed the software watchdog
 */
static void feedSwWatchdog(void);

void initSwWatchdog(void) {
  ASSERT((watchdogStack != NULL) && (&watchdogTaskBuffer != NULL));
  ASSERT(&watchdogEventGroup != NULL);
  watchdogEventHandle = xEventGroupCreateStatic(&watchdogEventGroup);
  watchdogTaskHandle = xTaskCreateStatic(swWatcdogFeeder, SW_WATCHDOG_NAME, SW_WATCHDOG_STACK_SIZE, NULL,
                                         SW_WATCHDOG_PRIORITY, watchdogStack, &watchdogTaskBuffer);
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
    uint32_t taskCheckinTrue = 0x1111;
    uint32_t result = xEventGroupWaitBits(watchdogEventHandle, taskCheckinTrue, pdTRUE, pdTRUE, FEEDING_PERIOD);
    for(uint8_t i = 0; i<(sizeof(taskArray)/sizeof(taskArray[0])); i++){

      TickType_t currentTick = xTaskGetTickCount();
      TickType_t tickDiff = currentTick - taskArray[i].taskLastCheckIn;
      bool checkInStat = result & (1 << taskArray[i].taskNum);
      
      if(!(taskArray[i].taskTimeOut < tickDiff)){
        break;
      }

      if(checkInStat){
        taskArray[i].taskLastCheckIn = currentTick;
      }
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

void taskRegister(uint32_t taskNum, TickType_t taskTimeOut){  
  watchdog_task_t task;
  task.taskNum = taskNum;
  task.taskLastCheckIn = 0;
  task.taskTimeOut = taskTimeOut;
  taskArray[taskNum] = task;
}

void taskCheckIn(uint32_t taskNum) { xEventGroupSetBits(watchdogTaskHandle, taskNum); }
