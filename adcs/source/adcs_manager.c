#include "adcs_manager.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t adcsTaskHandle = NULL;
static StaticTask_t adcsTaskBuffer;
static StackType_t adcsTaskStack[ADCS_MANAGER_STACK_SIZE];

static QueueHandle_t adcsQueueHandle = NULL;
static StaticQueue_t adcsQueue;
static uint8_t adcsQueueStack[ADCS_MANAGER_QUEUE_LENGTH*ADCS_MANAGER_QUEUE_ITEM_SIZE];

/*Boolean values related to the state of the program*/
uint16_t isDetumbling = 0;
uint16_t hasAttitudeError = 0;

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
    ASSERT( (adcsTaskStack != NULL) && (&adcsTaskBuffer != NULL) );
    if (adcsTaskHandle == NULL) {
        adcsTaskHandle = xTaskCreateStatic(vADCSManagerTask, ADCS_MANAGER_NAME, ADCS_MANAGER_STACK_SIZE, NULL, ADCS_MANAGER_PRIORITY, adcsTaskStack, &adcsTaskBuffer);
    }

    ASSERT( (adcsQueueStack != NULL) && (&adcsQueue != NULL) );
    if (adcsQueueHandle == NULL) {
        adcsQueueHandle = xQueueCreateStatic(ADCS_MANAGER_QUEUE_LENGTH, ADCS_MANAGER_QUEUE_ITEM_SIZE, adcsQueueStack, &adcsQueue);
    }
}

obc_error_code_t sendToADCSQueue(adcs_event_t *event) {
    ASSERT(adcsQueueHandle != NULL);

    if (event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if ( xQueueSend(adcsQueueHandle, (void *) event, ADCS_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS )
        return OBC_ERR_CODE_SUCCESS;

    return OBC_ERR_CODE_QUEUE_FULL;
}

static void vADCSManagerTask(void * pvParameters) {
    ASSERT(adcsQueueHandle != NULL);

    while(1) {
        adcs_event_t queueMsg;
        if(xQueueReceive(adcsQueueHandle, &queueMsg, ADCS_MANAGER_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            queueMsg.eventID = ADCS_MANAGER_NULL_EVENT_ID;

        switch(queueMsg.eventID) {
            case ADCS_MANAGER_NULL_EVENT_ID:
                break;
            case ADCS_MANAGER_LOW_POWER_EVENT_ID:
                if(queueMsg.data.i) {
                    vTaskResume(lowPowerHandle);
                } else {
                    vTaskSuspend(lowPowerHandle);
                }
                break;
        }
    }
}

/*Algorithrm functions*/

void lowPower(void * pvParameters){
    while(1) {
        // Insert Code here
    }
}

void detumblingMonitor(void * pvParameter)
{
    while (1)
    {
        /*If the satellite is detumbling then set isDetumbling=1 (true)*/
        /*If the satellite is NOT detumbling then set isDetumbling=0 (false)*/

        /*If the satellite is detumbling then resume the detumbling control task*/
        if (isDetumbling) {   
            vTaskResume(detumblingHandle);
        } else {
            vTaskResume(reactionWheelHandle);
            vTaskResume(altitudeTrackingHandle);
            vTaskResume(orbitalDeterminationHandle);
            vTaskResume(momentumDumpingHandle);
        }

        /*Main code will go here*/
    }
}

void questAlgorithm(void * pvParameter)
{
    while (1) {
        /*Main code will go here*/
    }
}

void detumblingControl(void * pvParameter)
{
    while (1) {
        /*Suspends itself when the satellite is not detumbling*/
        if (!isDetumbling) {
            vTaskSuspend(NULL);
        }
        
        /*Main code will go here*/
    }
}

void reactionWheelControl(void * pvParameter)
{
    while (1)
    {
        /*Suspends itself when the satellite is detumbling or doesn't have an altitude error*/
        if (isDetumbling || !hasAttitudeError) {
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
    }
}

void attitudeTracking(void * pvParameter)
{
    while (1){
        /*If the satellite's error is LESS than the error bounds then set hasAltitudeError=0 (false)*/
        /*If the satellite's error is GREATER than or equal to the error bounds then set hasAltitudeError=1 (true)*/

        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling) {
            vTaskSuspend(NULL);
        }
        if (hasAttitudeError) {
            vTaskResume(reactionWheelHandle);
        }

        /*Main code will go here*/

    }
}

void orbitalDetermination(void * pvParameter)
{
    while (1) {
        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling) {
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
    }
}

void momentumDumping(void * pvParameter) {
    while (1) {
        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling) {
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
    }
}

int initSupervisorTask(void) {
    /* Initialize the functions*/
    /*xTaskCreate(func, name, size, parameters, priority, handle)*/
    initADCSManager();
    xTaskCreate(detumblingMonitor, "Detumbling Monitor", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(questAlgorithm, "Quest Algorithm", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL);
    xTaskCreate(detumblingControl, "Detumbling Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &detumblingHandle);
    xTaskCreate(reactionWheelControl, "Reaction Wheel Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &reactionWheelHandle);
    xTaskCreate(attitudeTracking, "Attitude Tracking", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &altitudeTrackingHandle);
    xTaskCreate(orbitalDetermination, "Orbital Determination", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &orbitalDeterminationHandle);
    xTaskCreate(momentumDumping, "Momentum Dumping", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &momentumDumpingHandle);
    xTaskCreate(lowPower, "Low Power", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, &lowPowerHandle);

    /*Start scheduler*/
    vTaskStartScheduler();

    while (1) {};
    return 0;
}
