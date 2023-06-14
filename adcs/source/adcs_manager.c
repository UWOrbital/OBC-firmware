#include "adcs_manager.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

// #include <sys_common.h>
// #include <gio.h>

static TaskHandle_t adcsTaskHandle = NULL;
static StaticTask_t adcsTaskBuffer;
static StackType_t adcsTaskStack[ADCS_MANAGER_STACK_SIZE];

static QueueHandle_t adcsQueueHandle = NULL;
static StaticQueue_t adcsQueue;
static uint8_t adcsQueueStack[ADCS_MANAGER_QUEUE_LENGTH*ADCS_MANAGER_QUEUE_ITEM_SIZE];

/*Boolean values related to the state of the program*/
static int isDetumbling = 0;
static int hasAttitudeError = 0;

/*Detumbling */
static TaskHandle_t detumblingHandle = NULL;
static StaticTask_t detumblingTaskBuffer;
static StackType_t detumblingTaskStack[DEFAULT_STACK_SIZE];

/*Attitude Determination*/
static TaskHandle_t reactionWheelHandle = NULL;
static StaticTask_t reactionWheelTaskBuffer;
static StackType_t reactionWheelTaskStack[DEFAULT_STACK_SIZE];

/*Atitude Tracking*/
static TaskHandle_t attitudeTrackingHandle = NULL;
static StaticTask_t attitudeTrackingTaskBuffer;
static StackType_t attitudeTrackingTaskStack[DEFAULT_STACK_SIZE];

/*Orbital Determination*/
static TaskHandle_t orbitalDeterminationHandle = NULL;
static StaticTask_t orbitalDeterminationTaskBuffer;
static StackType_t orbitalDeterminationTaskStack[DEFAULT_STACK_SIZE];

/*Momentum Dumping*/
static TaskHandle_t momentumDumpingHandle = NULL;
static StaticTask_t momentumDumpingTaskBuffer;
static StackType_t momentumDumpingTaskStack[DEFAULT_STACK_SIZE];

/*Low Power*/
static TaskHandle_t lowPowerHandle = NULL;
static StaticTask_t lowPowerTaskBuffer;
static StackType_t lowPowerTaskStack[DEFAULT_STACK_SIZE];

/**
 * @brief	ADCS Manager task.
 * @param	pvParameters	Task parameters.
 */
static void vADCSManagerTask(void * pvParameters);

/**
 * @brief Initializes the ADCS supervisor task code
 */
static int initSupervisorTask(void);

void initADCSManager(void) {
    // ASSERT( (adcsTaskStack != NULL) && (&adcsTaskBuffer != NULL) );
    if (adcsTaskHandle == NULL) {
        adcsTaskHandle = xTaskCreateStatic(vADCSManagerTask, ADCS_MANAGER_NAME, ADCS_MANAGER_STACK_SIZE, NULL, ADCS_MANAGER_PRIORITY, adcsTaskStack, &adcsTaskBuffer);
    }

    // ASSERT( (adcsQueueStack != NULL) && (&adcsQueue != NULL) );
    if (adcsQueueHandle == NULL) {
        adcsQueueHandle = xQueueCreateStatic(ADCS_MANAGER_QUEUE_LENGTH, ADCS_MANAGER_QUEUE_ITEM_SIZE, adcsQueueStack, &adcsQueue);
    }

    initSupervisorTask();
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

/**
 * @brief Runs low power mode Algorithrm
 * @param pvParameter Task parameters.
 */
static void lowPower(void * pvParameters) {
    while(1) {
        // Insert Code here
    }
}

/**
 * @brief Changes the state of the appliation based on if the satellite is detumbling
 * @param pvParameter Task parameters.
 */
static void detumblingMonitor(void * pvParameter) {
    while (1) {
        /*If the satellite is detumbling then set isDetumbling=1 (true)*/
        /*If the satellite is NOT detumbling then set isDetumbling=0 (false)*/

        /*If the satellite is detumbling then resume the detumbling control task*/
        if (isDetumbling) {   
            vTaskResume(detumblingHandle);
        } else {
            vTaskResume(reactionWheelHandle);
            vTaskResume(attitudeTrackingHandle);
            vTaskResume(orbitalDeterminationHandle);
            vTaskResume(momentumDumpingHandle);
        }

        /*Main code will go here*/
    }
}

/**
 * @brief Quest Algorithrm code
 * @param pvParameter Task parameters.
 */
static void kalmanFilter(void * pvParameter) {
    while (1) {
        /*Main code will go here*/
    }
}

/**
 * @brief Runs Detumbling Control Law Algorithrm
 * @param pvParameter Task parameters.
 */
static void detumblingControl(void * pvParameter) {
    while (1) {
        /*Suspends itself when the satellite is not detumbling*/
        if (!isDetumbling) {
            vTaskSuspend(NULL);
        }
        
        /*Main code will go here*/
    }
}

/**
 * @brief Runs Reaction Wheel Control Law Algorithrm
 * @param pvParameter Task parameters.
 */
static void reactionWheelControl(void * pvParameter) {
    while (1) {
        /*Suspends itself when the satellite is detumbling or doesn't have an altitude error*/
        if (isDetumbling || !hasAttitudeError) {
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
    }
}

/**
 * @brief Runs Altitude Tracking Algorithrm (Might need to include the Solar Panel and/or Ground Target Tracking Code here or in seperate function(s))
 * @param pvParameter Task parameters.
 */
static void attitudeTracking(void * pvParameter) {
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

/**
 * @brief Runs Orbital Determination Algorithrm 
 * @param pvParameter Task parameters.
 */
static void orbitalDetermination(void * pvParameter) {
    while (1) {
        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling) {
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
    }
}

/**
 * @brief Runs Momuntum Dumping Algorithrm
 * @param pvParameter Task parameters.
 */
static void momentumDumping(void * pvParameter) {
    while (1) {
        /*Suspends itself when the satellite is detumbling*/
        if (isDetumbling) {
            vTaskSuspend(NULL);
        }

        /*Main code will go here*/
    }
}

static int initSupervisorTask(void) {
    /* Initialize the functions*/
    /*xTaskCreateStatic(func, name, size, parameters, priority, handle, buffer)*/
    // Always be running
    (void) xTaskCreateStatic(detumblingMonitor, "Detumbling Monitor", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);
    (void) xTaskCreateStatic(kalmanFilter, "Quest Algorithm", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);

    // Can be suspended
    detumblingHandle = xTaskCreateStatic(detumblingControl, "Detumbling Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);
    reactionWheelHandle = xTaskCreateStatic(reactionWheelControl, "Reaction Wheel Control", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);
    attitudeTrackingHandle = xTaskCreateStatic(attitudeTracking, "Attitude Tracking", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);
    orbitalDeterminationHandle = xTaskCreateStatic(orbitalDetermination, "Orbital Determination", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);
    momentumDumpingHandle = xTaskCreateStatic(momentumDumping, "Momentum Dumping", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);
    lowPowerHandle = xTaskCreateStatic(lowPower, "Low Power", DEFAULT_STACK_SIZE, NULL, DEFAULT_PRIORITY, NULL, NULL);

    /*Start scheduler*/
    vTaskStartScheduler();
    
    while (1) {};
    return 0;
}

#ifdef ADCS_MANAGER_TESTING
int main(void) {
    initADCSManager();   
}
#endif