#include "adcs_manager.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

// For testing purposes:
// #include <sys_common.h>
// #include <gio.h>
//#include <sys_common.h>
#include <stdio.h>
#include <assert.h>

// All ASSERT() have been commented out
// undefined reference to `ASSERT' error

// Remove after testing
int print_running = 0;

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
static TaskHandle_t lowPowerHandle = NULL;

/**
 * @brief	ADCS Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vADCSManagerTask(void * pvParameters);

void initADCSManager(void) {
    //ASSERT( (adcsTaskStack != NULL) && (&adcsTaskBuffer != NULL) );
    if (adcsTaskHandle == NULL) {
        adcsTaskHandle = xTaskCreateStatic(vADCSManagerTask, ADCS_MANAGER_NAME, ADCS_MANAGER_STACK_SIZE, NULL, ADCS_MANAGER_PRIORITY, adcsTaskStack, &adcsTaskBuffer);
    }

    // ASSERT( (adcsQueueStack != NULL) && (&adcsQueue != NULL) );
    if (adcsQueueHandle == NULL) {
        adcsQueueHandle = xQueueCreateStatic(ADCS_MANAGER_QUEUE_LENGTH, ADCS_MANAGER_QUEUE_ITEM_SIZE, adcsQueueStack, &adcsQueue);
    }
}

obc_error_code_t sendToADCSQueue(adcs_event_t *event) {
    //ASSERT(adcsQueueHandle != NULL);

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

        if (print_running){
            printf("vADCSManagerTask \n");
        }

        switch(queueMsg.eventID) {
            case ADCS_MANAGER_NULL_EVENT_ID:
                printf("NULL\n");
                break;
            case ADCS_MANAGER_LOW_POWER_EVENT_ID:
                if(queueMsg.data.i){
                    // Do something
                    printf("Resuming low power mode\n");
                    vTaskResume(lowPowerHandle);
                }
                else{
                    printf("Suspending low power mode\n");
                    vTaskSuspend(lowPowerHandle);
                }
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

// Remove after testing
void environment(void * pvParameter){
    int count = 0;
    while(1) {
        adcs_event_t event;
        event.eventID = ADCS_MANAGER_LOW_POWER_EVENT_ID;

        if (count % 2 == 0){
            printf("Sending low power ON\n");
            event.data.i = 1;
        } else {
            printf("Sending low power OFF\n");
            event.data.i = 0;
        }
        if (xQueueSend(adcsQueueHandle, &event, 0) != pdTRUE){
            printf("Queue is full\n");
        } 
        ++count;
        vTaskDelay(ADCS_MANAGER_QUEUE_RX_WAIT_PERIOD / 2);
    }
}

/*Algorithrm functions*/

void lowPower(void * pvParameters){
    while(1){
        // Insert Code here
        if(print_running){
            printf("Low Power Mode\n");
        }
        timer(1000000);
    }
}

void detumblingMonitor(void * pvParameter)
{
    while (1)
    {
        // Testing purposes
        static int detumblingRun = 0;
        int satelliteDetumbling = detumblingRun % 10;

        if (satelliteDetumbling > 6 && satelliteDetumbling < 9) {
            isDetumbling=1; 
        } 
        else{
            isDetumbling=0;
        }
            
        ++detumblingRun;  

        if (print_running){
            printf("satelliteDetumbling: %d\n", satelliteDetumbling);
            printf("detumblingMonitor \n");
            printf("Detumbling run %d \n", detumblingRun);
        }
        
        timer(1000000);

        /*If the satellite is detumbling then set isDetumbling=1 (true)*/
        /*If the satellite is NOT detumbling then set isDetumbling=0 (false)*/

        if (isDetumbling)
        {   
            // printf("\tEMERGENCY\n");
            vTaskResume(detumblingHandle);
        }
        else
        {
            // printf("\tEVERYTHING IS FINE\n");
            vTaskResume(reactionWheelHandle);
            vTaskResume(altitudeTrackingHandle);
            vTaskResume(orbitalDeterminationHandle);
            vTaskResume(momentumDumpingHandle);
        }
    }
}

void questAlgorithm(void * pvParameter)
{
    while (1)
    {
        /*Main code will go here*/
        if (print_running){
            printf("Quest\n");  
        }
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
            // printf("Suspending detumblingControl\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        if (print_running){
            printf("detumblingControl\n");
        }
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
            // printf("Suspending reactionWheelControl\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        if (print_running){
            printf("reactionWheelControl\n"); 
        }
        timer(1000000);
    }
}

void altitudeTracking(void * pvParameter)
{
    while (1)
    {
        /*If the satellite's error is LESS than the error bounds then set hasAltitudeError=0 (false)*/
        /*If the satellite's error is GREATER than or equal to the error bounds then set hasAltitudeError=1 (true)*/

        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling)
        {
            // printf("Suspending altitudeTracking\n");
            vTaskSuspend(NULL);
        }
        if (hasAltitudeError)
        {
            // printf("\tHAS ERROR\n");
            vTaskResume(reactionWheelHandle);
        }

        /*Main code will go here*/

        // Testing purposes
        static int altitudeRun = 0;
        ++altitudeRun;
        int satelliteAltitude = altitudeRun % 10;

        if (print_running){
            printf("altitudeTracking\n");
            printf("satelliteAltitude: %d \n", satelliteAltitude);
            printf("altitudeRun: %d \n", altitudeRun);
        }

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
            // printf("Suspending orbitalDetermination\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        if (print_running){
            printf("orbitalDetermination\n");  
        }
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
            // printf("Suspending momentumDumping\n");
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
        if (print_running){
            printf("momentumDumping\n");
        }
        timer(1000000);
    }
}

int initSupervisorTask(void)
{
    printf("Initialized ADCS\n");
    /* Initialize the functions*/
    /*xTaskCreate(func, name, size, parameters, priority, handle)*/
    initADCSManager();
    xTaskCreate(environment, "Environment", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(detumblingMonitor, "Detumbling Monitor", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(questAlgorithm, "Quest Algorithm", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(detumblingControl, "Detumbling Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &detumblingHandle);
    xTaskCreate(reactionWheelControl, "Reaction Wheel Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &reactionWheelHandle);
    xTaskCreate(altitudeTracking, "Altitude Tracking", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &altitudeTrackingHandle);
    xTaskCreate(orbitalDetermination, "Orbital Determination", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &orbitalDeterminationHandle);
    xTaskCreate(momentumDumping, "Momentum Dumping", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &momentumDumpingHandle);
    xTaskCreate(lowPower, "Low Power", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &lowPowerHandle);

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

