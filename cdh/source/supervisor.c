#include "supervisor.h"
#include "telemetry.h"
#include "adcs_manager.h"
#include "comms_manager.h"
#include "eps_manager.h"
#include "payload_manager.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t supervisorTaskHandle = NULL;
static StaticTask_t supervisorTaskBuffer;
static StackType_t supervisorTaskStack[SUPERVISOR_STACK_SIZE];

static QueueHandle_t supervisorQueueHandle = NULL;
static StaticQueue_t supervisorQueue;
static uint8_t supervisorQueueStack[SUPERVISOR_QUEUE_LENGTH*SUPERVISOR_QUEUE_ITEM_SIZE];

/**
 * @brief	Supervisor task.
 * @param	pvParameters	Task parameters.
 */
static void vSupervisorTask(void * pvParameters);

/**
 * @brief Send all startup messages from the supervisor task to other tasks.
 */
static void sendStartupMessages(void);

void initSupervisor(void) {
    ASSERT( (supervisorTaskStack != NULL) && (&supervisorTaskBuffer != NULL) );
    if (supervisorTaskHandle == NULL) {
        supervisorTaskHandle = xTaskCreateStatic(vSupervisorTask, SUPERVISOR_NAME, SUPERVISOR_STACK_SIZE, NULL, SUPERVISOR_PRIORITY, supervisorTaskStack, &supervisorTaskBuffer);
    }

    ASSERT( (supervisorQueueStack != NULL) && (&supervisorQueue != NULL) );
    if (supervisorQueueHandle == NULL) {
        supervisorQueueHandle = xQueueCreateStatic(SUPERVISOR_QUEUE_LENGTH, SUPERVISOR_QUEUE_ITEM_SIZE, supervisorQueueStack, &supervisorQueue);
    }
}

obc_error_code_t sendToSupervisorQueue(supervisor_event_t *event) {
    ASSERT(supervisorQueueHandle != NULL);

    if (event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (xQueueSend(supervisorQueueHandle, (void *) event, SUPERVISOR_QUEUE_TX_WAIT_PERIOD) == pdPASS)
        return OBC_ERR_CODE_SUCCESS;
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static void sendStartupMessages(void) {
    /* Send startup message to telemetry task as example */
    telemetry_event_t newMsg;
    newMsg.eventID = TURN_ON_LED_EVENT_ID;
    newMsg.data.i = TELEMETRY_DELAY_TICKS;
    sendToTelemetryQueue(&newMsg);

    /* TODO: Add startup messages to other tasks */
}

static void vSupervisorTask(void * pvParameters) {
    ASSERT(supervisorQueueHandle != NULL);

    /* Initialize other tasks */
    initTelemetry();
    initADCSManager();
    initCommsManager();
    initEPSManager();
    initPayloadManager();

    /* Send initial messages to system queues */
    sendStartupMessages();    
    
    while(1) {
        supervisor_event_t inMsg;
        telemetry_event_t outMsgTelemetry;

        if (xQueueReceive(supervisorQueueHandle, &inMsg, SUPERVISOR_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            inMsg.eventID = SUPERVISOR_NULL_EVENT_ID;

        switch (inMsg.eventID) {
            case TURN_OFF_LED_EVENT_ID:
                gioToggleBit(gioPORTB, 0);
                outMsgTelemetry.eventID = TURN_ON_LED_EVENT_ID;
                outMsgTelemetry.data.i = TELEMETRY_DELAY_TICKS;
                sendToTelemetryQueue(&outMsgTelemetry);
                break;
            case SUPERVISOR_NULL_EVENT_ID:
                break;
            default:
                ;
        }
    }
}
