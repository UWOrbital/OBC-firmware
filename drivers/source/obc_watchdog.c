#include <stdio.h>
#include <system.h>
#include "obc_watchdog.h"

#define RTIWDKEY (*(unsigned char *) 0xFFFFFC9C)
#define RTIDWDPRLD (* (unsigned char *) 0xFFFFFFC94)

uint32_t startDWD = 0xA35C;
uint32_t resetDWD = 0xE51A;
uint32_t rtiFrequency = 0x6A;

static TaskHandle_t DWWDTaskHandle = NULL;

static void DWWDTask(void * pvParameters);

void initDWWD(){

    RTIDWDPRLD ^= rtiFrequency; 
    RTIWDKEY = startDWD;
    RTIWDKEY = resetDWD;

}

void initDWWDTask(){

    BaseType_t xReturned = pdFAIL;

    if(DWWDTaskHandle == NULL){

        xReturned = xTaskCreate(DWWDTask,
                                DWWD_NAME,
                                DWWD_STACK_SIZE,
                                NULL,
                                DWWD_PRIORITY,
                                &DWWDTaskHandle);
    }
}

static void DWWDTask(void * pvParameters){

    while(1){

    }
}