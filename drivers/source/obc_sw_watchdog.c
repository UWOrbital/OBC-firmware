#include <system.h>
#include "obc_sw_watchdog.h"
#include "obc_assert.h"
#include "reg_rti.h"
#include "obc_privilege.h"
#include "obc_task_config.h"

#define RESET_DWD_CMD1 0x53C8       //Timer is reset by wrting RESET_DWD_CMD1
#define RESET_DWD_CMD2 0x190D       // and RESET_DWD_CMD2 as a sequence

#define MIN_DWD_PRELOAD_VAL 0
#define MAX_DWD_PRELOAD_VAL 0xFFF

#define DWD_FULL_SIZE_WINDOW 0x5    //Window size 100%; This watchdog is timeout-only
#define DWD_FEEDING_PERIOD pdMS_TO_TICKS(300)
#define DWWD_NAME "Digital Windowed Watchdog"

STATIC_ASSERT(MIN_DWD_PRELOAD_VAL<=DWD_PRELOAD_VAL && DWD_PRELOAD_VAL<=MAX_DWD_PRELOAD_VAL,
                "Watchdog requires the preload value to be within minimum and maximum value.");

static watchdogStack[DWWD_STACK_SIZE];
static xWatchdogTaskBuffer;

static TaskHandle_t watchdogTaskHandle = NULL;

static void swWatcdogFeeder(void * pvParameters);

static feedSwWatchdog(void){
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    rtiREG1->WDKEY ^= RESET_DWD_CMD1;
    rtiREG1->WDKEY ^= RESET_DWD_CMD2;
    portRESET_PRIVILEGE(xRunningPrivileged);
}

void initDWWDTask(void){

    ASSERT(watchdogStack && &xWatchdogTaskBuffer);
    if(watchdogTaskHandle == NULL){

        watchdogTaskHandle = xTaskCreateStatic(swWatcdogFeeder,
                                                DWWD_NAME,
                                                DWWD_STACK_SIZE,
                                                NULL,
                                                DWWD_PRIORITY,
                                                watchdogStack,
                                                &xWatchdogTaskBuffer);
    }

    ASSERT(watchdogTaskHandle);
}

static void swWatcdogFeeder(void * pvParameters){
    
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    rtiREG1->DWDPRLD = DWD_PRELOAD_VAL;
    rtiREG1->WWDSIZECTRL = DWD_FULL_SIZE_WINDOW;
    
    portRESET_PRIVILEGE(xRunningPrivileged);
    while(1){
        feedSwWatchdog();
        vTaskDelay(DWD_FEEDING_PERIOD);
    }
}