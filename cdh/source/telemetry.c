#include "telemetry.h"
#include "supervisor.h"
#include "comms_manager.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

#include <stdio.h>
#include <stdbool.h>

static TaskHandle_t telemetryTaskHandle = NULL;
static StaticTask_t telemetryTaskBuffer;
static StackType_t telemetryTaskStack[TELEMETRY_STACK_SIZE];

static QueueHandle_t telemetryQueueHandle = NULL;
static StaticQueue_t telemetryQueue;
static uint8_t telemetryQueueStack[TELEMETRY_QUEUE_LENGTH*TELEMETRY_QUEUE_ITEM_SIZE];

static TimerHandle_t ledTimerHandle = NULL;
static StaticTimer_t ledTimerBuffer;

/**
 * @brief	Telemetry task.
 * @param	pvParameters	Task parameters.
 */
static void vTelemetryTask(void * pvParameters);

/**
 * @brief Example timer callback function for "Turn on LED" event
 */
static void timerCallback(TimerHandle_t xTimer);

void initTelemetry(void) {
    ASSERT( (telemetryTaskStack != NULL) && (&telemetryTaskBuffer != NULL) );
    if (telemetryTaskHandle == NULL) {
        telemetryTaskHandle = xTaskCreateStatic(vTelemetryTask, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, telemetryTaskStack, &telemetryTaskBuffer);
    }

    ASSERT( (telemetryQueueStack != NULL) && (&telemetryQueue != NULL) );
    if (telemetryQueueHandle == NULL) {
        telemetryQueueHandle = xQueueCreateStatic(TELEMETRY_QUEUE_LENGTH, TELEMETRY_QUEUE_ITEM_SIZE, telemetryQueueStack, &telemetryQueue);
    }

    ASSERT(&ledTimerBuffer != NULL);
    if (ledTimerHandle == NULL) {
        ledTimerHandle = xTimerCreateStatic("ledTimer", pdMS_TO_TICKS(1000), false, NULL, timerCallback, &ledTimerBuffer);
    }
}

obc_error_code_t sendToTelemetryQueue(telemetry_event_t *event) {
    ASSERT(telemetryQueueHandle != NULL);

    if (event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (xQueueSend(telemetryQueueHandle, (void *) event, TELEMETRY_QUEUE_TX_WAIT_PERIOD) == pdPASS)
        return OBC_ERR_CODE_SUCCESS;
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static uint8_t sendTelemetryToFile(FILE *telFile, telemetry_event_t queueMsg) {
    if(telFile == NULL) {
        return 0;
    }
    fwrite(&queueMsg, sizeof(telemetry_event_t), 1, telFile);

    return 1;
}

static void vTelemetryTask(void * pvParameters) {
    char fileName[14] = "telemetry"; // This will go into a particular directory on OBC sd card
    char fileNumber = '0'; // Will increment this everytime a new file needs to be created. 
    char fileType[4] = ".tlm"; // Will be a .dat file for now

    strcat(fileName, &fileNumber);
    strcat(fileName, fileType);

    char command[] = "";

    bool fileOpen = false;
    FILE *telFile; // not sure if a new file object needs to be created whenever a new file is requested. 
    telFile = fopen(fileName, "w"); 

    while(1){
        if(strcmp(command, "new file") == 1) { // if a new file is requested
            strcpy(fileName, "telemetry"); // reset file name before apending file number and type

            if(fileOpen) {
                fclose(telFile); // close the previously opened file
                fileOpen = false;
            }

            if(fileNumber == '9') { // if file number reaches 10 reset to 0
                fileNumber = '0';
            }
            else {
                fileNumber++; // increment file number by one
            }

            strcat(fileName, &fileNumber);
            strcat(fileName, fileType);

            if(!fileOpen) {
                telFile = fopen(fileName, "w"); 
                fileOpen = true;
            }
        }
 
        telemetry_event_t queueMsg;
        if(xQueueReceive(telemetryQueueHandle, &queueMsg, TELEMETRY_QUEUE_RX_WAIT_PERIOD) != pdPASS){
            switch (queueMsg.eventID)
            {
                /* If telemetry file name is requested by comms */
                case SEND_FILE_NUMBER_TO_COMMS_ID:
                    comms_event_t event;
                    event.eventID = TELEMETRY_FILE_NUMBER_ID;
                    int number;
                    sscanf(&fileNumber, "%d", &number);
                    event.data.i = number;
                    
                    sendToCommsQueue(&event);
                    break;
                /* Any other case will be telemetry to store in the file */
                default:
                    sendTelemetryToFile(telFile, queueMsg);
                    break;
                }
        }


    }
}

static void timerCallback(TimerHandle_t xTimer) {
    supervisor_event_t newMsg;
    newMsg.eventID = TURN_OFF_LED_EVENT_ID;
    sendToSupervisorQueue(&newMsg);
}
