#include "supervisor.h"
#include "timekeeper.h"
#include "telemetry_manager.h"
#include "adcs_manager.h"
#include "command_manager.h"
#include "comms_manager.h"
#include "eps_manager.h"
#include "payload_manager.h"
#include "obc_sw_watchdog.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_states.h"
#include "obc_task_config.h"
#include "obc_reset.h"
#include "obc_board_config.h"

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

/**
 * @brief Setup the file system.
 * 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code.
 */
static obc_error_code_t setupFileSystem(void);

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
    /* TODO: Add startup messages to other tasks */
}

static void vSupervisorTask(void * pvParameters) {
    obc_error_code_t errCode;

    ASSERT(supervisorQueueHandle != NULL);

    // TODO: Deal with errors
    LOG_IF_ERROR_CODE(setupFileSystem());

    /* Initialize other tasks */
    initSwWatchdog();
    initTimekeeper();
    initTelemetry();
    initCommandManager();
    initADCSManager();
    initCommsManager();
    initEPSManager();
    initPayloadManager();

    // TODO: Deal with errors
    LOG_IF_ERROR_CODE(changeStateOBC(OBC_STATE_INITIALIZING));

    /* Send initial messages to system queues */
    sendStartupMessages();    

    // TODO: Only enter normal state after initial checks are complete
    // TODO: Deal with errors
    LOG_IF_ERROR_CODE(changeStateOBC(OBC_STATE_NORMAL));
    
    while(1) {
        supervisor_event_t inMsg;
        
        if (xQueueReceive(supervisorQueueHandle, &inMsg, SUPERVISOR_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            #if defined(DEBUG) && !defined(OBC_REVISION_2)
            vTaskDelay(pdMS_TO_TICKS(1000));
            gioToggleBit(SUPERVISOR_DEBUG_LED_GIO_PORT, SUPERVISOR_DEBUG_LED_GIO_BIT);
            #endif
            continue;
        }

        switch (inMsg.eventID) {
            default:
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_EVENT);
        }
    }
}

static obc_error_code_t setupFileSystem(void) {
    int32_t ret;

    ret = red_init();
    if (ret != 0) {
        LOG_DEBUG("red_init failed with error: %d", red_errno);
        return OBC_ERR_CODE_FS_INIT_FAILED;
    }

    // TODO: FS formatting doesn't need to be done every time
    ret = red_format("");
    if (ret != 0) {
        LOG_DEBUG("red_format failed with error: %d", red_errno);
        return OBC_ERR_CODE_FS_FORMAT_FAILED;
    }

    ret = red_mount("");
    if (ret != 0) {
        LOG_DEBUG("red_mount failed with error: %d", red_errno);
        return OBC_ERR_CODE_FS_MOUNT_FAILED;
    }

    return OBC_ERR_CODE_SUCCESS;
}