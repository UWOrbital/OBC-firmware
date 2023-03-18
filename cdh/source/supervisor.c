#include "supervisor.h"
#include "telemetry_manager.h"
#include "adcs_manager.h"
#include "comms_manager.h"
#include "eps_manager.h"
#include "payload_manager.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_states.h"
#include "obc_task_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <redposix.h>

/* Supervisor queue config */
#define SUPERVISOR_QUEUE_LENGTH 10U
#define SUPERVISOR_QUEUE_ITEM_SIZE sizeof(supervisor_event_t)
#define SUPERVISOR_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define SUPERVISOR_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

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
    /* TODO: Add startup messages to other tasks */
}

static void vSupervisorTask(void * pvParameters) {
    ASSERT(supervisorQueueHandle != NULL);

    int32_t ret;

    ret = red_init();
    if (ret == 0) {
        LOG_DEBUG("microSD initialization succeeded");
    } else {
        LOG_DEBUG("red_init failed with error: %d", red_errno);
    }

    ret = red_format("");
    if (ret == 0) {
        LOG_DEBUG("microSD formatted successfully");
    } else {
        LOG_DEBUG("red_format failed with error: %d", red_errno);
    }

    ret = red_mount("");
    if (ret == 0) {
        LOG_DEBUG("FS volume mounted successfully");
    } else {
        LOG_DEBUG("red_mount failed with error: %d", red_errno);
    }

    /* Initialize other tasks */
    initTelemetry();
    initADCSManager();
    initCommsManager();
    initEPSManager();
    initPayloadManager();

    changeStateOBC(OBC_STATE_INITIALIZING);

    /* Send initial messages to system queues */
    sendStartupMessages();    

    // TODO: Only enter normal state after initial checks are complete
    changeStateOBC(OBC_STATE_NORMAL);
    
    while(1) {
        supervisor_event_t inMsg;
        
        if (xQueueReceive(supervisorQueueHandle, &inMsg, SUPERVISOR_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            continue;
        }

        switch (inMsg.eventID) {
            default:
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_EVENT);
        }

        gioToggleBit(gioPORTB, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
