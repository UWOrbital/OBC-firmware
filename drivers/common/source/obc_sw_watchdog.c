#include <system.h>
#include "obc_sw_watchdog.h"
#include "obc_assert.h"
#include "reg_rti.h"
#include "obc_privilege.h"
#include "obc_task_config.h"

// Watchdog is fed by writing these two values to the WDKEY register
#define RESET_DWD_CMD1 0xE51AUL
#define RESET_DWD_CMD2 0xA35CUL

// Possible reset actions
#define DWD_RXN_CTRL_RESET 0x5UL
#define DWD_RXN_CTRL_NMI   0xAUL

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
STATIC_ASSERT((uint32_t)RTI_FREQ == 73 , "RTI frequency is not 73.333 MHz");
STATIC_ASSERT(PRELOAD_VAL >= MIN_PRELOAD_VAL && PRELOAD_VAL <= MAX_PRELOAD_VAL, "Preload value is out of range");

// Window size 100%; This watchdog is timeout-only
#define FULL_SIZE_WINDOW 0x5UL

static StackType_t watchdogStack[SW_WATCHDOG_STACK_SIZE];
static StaticTask_t watchdogTaskBuffer;
static TaskHandle_t watchdogTaskHandle;

/**
 * @brief Software watchdog task
 */
static void swWatcdogFeeder(void * pvParameters);

/**
 * @brief Feed the software watchdog
 */
static void feedSwWatchdog(void);

void initSwWatchdog(void){
    ASSERT( (watchdogStack != NULL) && (&watchdogTaskBuffer != NULL) );
    watchdogTaskHandle = xTaskCreateStatic(swWatcdogFeeder,
        SW_WATCHDOG_NAME,
        SW_WATCHDOG_STACK_SIZE,
        NULL,
        SW_WATCHDOG_PRIORITY,
        watchdogStack,
        &watchdogTaskBuffer);
}

static void swWatcdogFeeder(void * pvParameters){
    
    // Set up the watchdog
    BaseType_t xRunningPrivileged = prvRaisePrivilege();

    rtiREG1->DWDPRLD = PRELOAD_VAL;
    rtiREG1->WWDSIZECTRL = FULL_SIZE_WINDOW;
    rtiREG1->WWDRXNCTRL = DWD_RXN_CTRL_RESET;
    rtiREG1->DWDCTRL = DWD_CTRL_ENABLE;
    
    portRESET_PRIVILEGE(xRunningPrivileged);

    while (1) {
        feedSwWatchdog();
        vTaskDelay(FEEDING_PERIOD);
    }
}

static void feedSwWatchdog(void) {
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    rtiREG1->WDKEY = RESET_DWD_CMD1;
    rtiREG1->WDKEY = RESET_DWD_CMD2;
    portRESET_PRIVILEGE(xRunningPrivileged);
}
