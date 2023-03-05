#include <system.h>
#include "obc_watchdog.h"

#define RTIWDKEY (*(uint32_t *) 0xFFFFFC9C)
#define RTIDWDPRLD (* (uint32_t *) 0xFFFFFFC94)
#define RTIWWDSIZECTRL (* (uint32_t *) 0xFFFFFCA8)

uint32_t startDWD = 0x53C8;
uint32_t enableDWD = 0x190D;
uint32_t resetDWD = 0x0000;
uint32_t DWDPRLD = 0xFFF;      //texp = 0.305 seconds
uint32_t fullSizeWindow = 0x00000005;   //Windowsize = 100%

TickType_t delayTime = 100;
 
static TaskHandle_t DWWDTaskHandle = NULL;

static void DWWDTask(void * pvParameters);

void initDWWD(){

    RTIDWDPRLD |= DWDPRLD;
    RTIWWDSIZECTRL = fullSizeWindow;
    RTIWDKEY ^= startDWD;
    RTIWDKEY ^= enableDWD;

}

uint8_t initDWWDTask(){

    BaseType_t xReturned = pdFAIL;

    if(DWWDTaskHandle == NULL){

        xReturned = xTaskCreate(DWWDTask,
                                DWWD_NAME,
                                DWWD_STACK_SIZE,
                                NULL,
                                DWWD_PRIORITY,
                                &DWWDTaskHandle);
    }

    if(xReturned == pdFAIL){
        return 0;
    }

    return 1;
}

static void DWWDTask(void * pvParameters){
    vTaskStartScheduler();
    TickType_t lastTime = 0;
    TickType_t currentTime = 0;
    while(1){
        currentTime = xTaskGetTickCount();
        if(currentTime - lastTime > 10000){
            RTIWDKEY ^= resetDWD;
        }else{
            RTIWDKEY ^= startDWD;
            RTIWDKEY ^= enableDWD;
        }
        vTaskDelay(delayTime);
        lastTime = delayTime + currentTime;
    }
}