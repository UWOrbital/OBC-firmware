#include "adcs_manager.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

// For testing purposes:
// #include <sys_common.h>
// #include <gio.h>

#include <stdio.h>
// #include <assert.h>

// All ASSERT() have been commented out

// undefined reference to `ASSERT' error

static TaskHandle_t adcsTaskHandle = NULL;
static StaticTask_t adcsTaskBuffer;
static StackType_t adcsTaskStack[ADCS_MANAGER_STACK_SIZE];

static QueueHandle_t adcsQueueHandle = NULL;
static StaticQueue_t adcsQueue;
static uint8_t adcsQueueStack[ADCS_MANAGER_QUEUE_LENGTH*ADCS_MANAGER_QUEUE_ITEM_SIZE];

/*Boolean values related to the state of the program*/
uint16_t isDetumbling = 0;
uint16_t hasAltitudeError = 0;

/*Task Handlers*/
static TaskHandle_t detumblingHandle = NULL;
static TaskHandle_t reactionWheelHandle = NULL;
static TaskHandle_t altitudeTrackingHandle = NULL;
static TaskHandle_t orbitalDeterminationHandle = NULL;
static TaskHandle_t momentumDumpingHandle = NULL;

/**
 * @brief	ADCS Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vADCSManagerTask(void * pvParameters);

void initADCSManager(void) {
    // ASSERT( (adcsTaskStack != NULL) && (&adcsTaskBuffer != NULL) );
    if (adcsTaskHandle == NULL) {
        adcsTaskHandle = xTaskCreateStatic(vADCSManagerTask, ADCS_MANAGER_NAME, ADCS_MANAGER_STACK_SIZE, NULL, ADCS_MANAGER_PRIORITY, adcsTaskStack, &adcsTaskBuffer);
    }

    // ASSERT( (adcsQueueStack != NULL) && (&adcsQueue != NULL) );
    if (adcsQueueHandle == NULL) {
        adcsQueueHandle = xQueueCreateStatic(ADCS_MANAGER_QUEUE_LENGTH, ADCS_MANAGER_QUEUE_ITEM_SIZE, adcsQueueStack, &adcsQueue);
    }
}

obc_error_code_t sendToADCSQueue(adcs_event_t *event) {
    // ASSERT(adcsQueueHandle != NULL);

    if (event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if ( xQueueSend(adcsQueueHandle, (void *) event, ADCS_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS )
        return OBC_ERR_CODE_SUCCESS;

    return OBC_ERR_CODE_QUEUE_FULL;
}

static void vADCSManagerTask(void * pvParameters) {
    // ASSERT(adcsQueueHandle != NULL);

    while(1){
        adcs_event_t queueMsg;
        if(xQueueReceive(adcsQueueHandle, &queueMsg, ADCS_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            queueMsg.eventID = ADCS_MANAGER_NULL_EVENT_ID;

        switch(queueMsg.eventID) {
            case ADCS_MANAGER_NULL_EVENT_ID:
                break;
        }
    }
}

// ONLY FOR TESTING:
void timer(int delay){
    for(int i=0; i < delay; i++)
    {
        printf("");
    }
}

/*Algorithrm functions*/

void detumblingMonitor(void * pvParameter)
{
    while (1)
    {
        static int detumblingRun = 0;
        /*If the satellite is detumbling then set isDetumbling=1 (true)*/
        /*If the satellite is NOT detumbling then set isDetumbling=0 (false)*/

        int satelliteDetumbling = detumblingRun % 10;
        printf("satelliteDetumbling: %d\n", satelliteDetumbling);
        if (satelliteDetumbling > 6 && satelliteDetumbling < 9) {
            isDetumbling=1; 
        } // change satellite_is_detumbling to boolean expression or value  
        else{
            isDetumbling=0;
        }
            
        detumblingRun++;  
        printf("detumblingMonitor \n");
        printf("Detumbling run %d \n", detumblingRun);

        if (isDetumbling)
        {   
            printf("EMERGENCY\n");
            vTaskResume(detumblingHandle);
        }
        else
        {
            printf("EVERYTHING IS FINE\n");
            vTaskResume(reactionWheelHandle);
            vTaskResume(altitudeTrackingHandle);
            vTaskResume(orbitalDeterminationHandle);
            vTaskResume(momentumDumpingHandle);
        }
        timer(1000000);
    }
}

void questAlgorithm(void * pvParameter)
{
    while (1)
    {
        /*Main code will go here*/
        printf("Quest\n");
        timer(1000000);
    }
}

void detumblingControl(void * pvParameter)
{
    while (1)
    {
        /*Suspends itself when the satellite is not detumbling*/
        if (!isDetumbling)
        {
            printf("Suspending detumblingControl\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        printf("detumblingControl\n");
        timer(1000000);
    }
}

void reactionWheelControl(void * pvParameter)
{
    while (1)
    {
        /*Suspends itself when the satellite is detumbling or doesn't have an altitude error*/
        if (isDetumbling || !hasAltitudeError)
        {
            printf("Suspending reactionWheelControl\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        printf("reactionWheelControl\n");
        timer(1000000);
    }
}

void altitudeTracking(void * pvParameter)
{
    while (1)
    {
        static int altitudeRun = 0;

        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling)
        {
            printf("Suspending altitudeTracking\n");
            vTaskSuspend(NULL);
        }
        if (hasAltitudeError)
        {
            printf("Has error\n");
            vTaskResume(reactionWheelHandle);
        }

        /*Main code will go here*/
        printf("altitudeTracking\n");
        /*If the satellite's error is LESS than the error bounds then set hasAltitudeError=0 (false)*/
        /*If the satellite's error is GREATER than or equal to the error bounds then set hasAltitudeError=1 (true)*/
        altitudeRun++;
        int satelliteAltitude = altitudeRun % 10;
        printf("satelliteAltitude: %d \n", satelliteAltitude);
        if(satelliteAltitude > 6 && satelliteAltitude < 9){
            hasAltitudeError = 1;
        }
        else{
            hasAltitudeError = 0;
        }
        
        timer(1000000);
    }
}

void orbitalDetermination(void * pvParameter)
{
    while (1)
    {
        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling)
        {
            printf("Suspending orbitalDetermination\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        printf("orbitalDetermination\n");
        timer(1000000);
    }
}

void momentumDumping(void * pvParameter)
{
    while (1)
    {
        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling)
        {
            printf("Suspending momentumDumping\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        printf("momentumDumping\n");
        timer(1000000);
    }
}

int initSupervisorTask(void)
{
    printf("Initialized ADCS\n");
    /* Initialize the functions*/
    /*xTaskCreate(func, name, size, parameters, priorite, handler)*/
    xTaskCreate(detumblingMonitor, "Detumbling Monitor", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(questAlgorithm, "Quest Algorithm", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(detumblingControl, "Detumbling Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &detumblingHandle);
    xTaskCreate(reactionWheelControl, "Reaction Wheel Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &reactionWheelHandle);
    xTaskCreate(altitudeTracking, "Altitude Tracking", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &altitudeTrackingHandle);
    xTaskCreate(orbitalDetermination, "Orbital Determination", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &orbitalDeterminationHandle);
    xTaskCreate(momentumDumping, "Momentum Dumping", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &momentumDumpingHandle);
    
    /*Start scheduler*/
    vTaskStartScheduler();

    while (1) {};
    return 0;
}

/**
 * @brief REMOVE AFTER TESTING
*/
int main(void){
    initSupervisorTask();
    return 0;
}

