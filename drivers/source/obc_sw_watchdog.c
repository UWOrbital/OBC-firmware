#include <system.h>
#include "obc_sw_watchdog.h"
#include "obc_errors.h"
#include <math.h>

#define RTIWDKEY (*(uint32_t *) 0xFFFFFC9C)
#define RTIDWDPRLD (* (uint32_t *) 0xFFFFFFC94)
#define RTIWWDSIZECTRL (* (uint32_t *) 0xFFFFFCA8)
#define STARTDWD 0x53C8
#define ENABLEDWD 0x190D
#define RESETDWD 0x0000

#define DWWD_NAME "Digital Windowed Watchdog"
#define DWWD_STACK_SIZE 128
#define DWWD_PRIORITY 1

float minTime = 0.0001117095987;
float maxTime = 0.4575625162;
uint32_t fullSizeWindow = 0x5;   //Windowsize = 100%
uint32_t DWDPRLD = 0;
TickType_t delayTime = 100;
 
StackType_t watchdogStack[DWWD_STACK_SIZE];
StaticTask_t xWatchdogTaskBuffer;

static TaskHandle_t watchdogTaskHandle = NULL;

static void swWatcdogFeeder(void * pvParameters);

void feedSwWatchdog(void){
    RTIWDKEY ^= STARTDWD;
    RTIWDKEY ^= ENABLEDWD;
}

obc_error_code_t initDWWD(float tExp){

    if(tExp <= minTime && tExp <= maxTime){

        DWDPRLD = (uint32_t)((tExp*RTI_FREQ*pow(10, 6))/(pow(2, 13))-1);
        RTIDWDPRLD = DWDPRLD;
        RTIWWDSIZECTRL = fullSizeWindow;
        feedSwWatchdog();

        return OBC_ERR_CODE_SUCCESS;
    }
    return OBC_ERR_CODE_WATCHDOG_INIT_FAILURE;
}

obc_error_code_t initDWWDTask(void){

    if(watchdogTaskHandle == NULL){

        watchdogTaskHandle = xTaskCreateStatic(swWatcdogFeeder,
                                                DWWD_NAME,
                                                DWWD_STACK_SIZE,
                                                NULL,
                                                DWWD_PRIORITY,
                                                watchdogStack,
                                                &xWatchdogTaskBuffer);
            }

    if(watchdogTaskHandle == NULL){
        return OBC_ERR_CODE_TASK_INIT_FAILURE;
    }

    return OBC_ERR_CODE_SUCCESS;
}



static void swWatcdogFeeder(void * pvParameters){
    TickType_t lastTime = 0;
    TickType_t currentTime = 0;

    uint32_t tExp = (uint32_t)((1+DWDPRLD)*pow(2,13)/RTI_FREQ*pow(10, 6));
    while(1){
        currentTime = xTaskGetTickCount();
        if(currentTime - lastTime > tExp){
            RTIWDKEY ^= RESETDWD;
        }else{
            feedSwWatchdog();
        }
        vTaskDelay(delayTime);
        lastTime = delayTime + currentTime;
    }
}