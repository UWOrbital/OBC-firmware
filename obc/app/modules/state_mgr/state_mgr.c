#include "state_mgr.h"
#include "comms_manager.h"  // for comms_state_t

#include "obc_board_config.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_reliance_fs.h"
#include "obc_scheduler_config.h"
#include "obc_state_handle.h"
#include "obc_state_defs.h"
#include "obc_time.h"

#include "fm25v20a.h"
#include "lm75bd.h"  // TODO: Handle within thermal manager

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <sys_common.h>

#if defined(DEBUG) && !defined(OBC_REVISION_2)
#include <gio.h>
#endif

/* Supervisor queue config */
#define STATE_MGR_QUEUE_LENGTH 10U
#define STATE_MGR_QUEUE_ITEM_SIZE sizeof(state_mgr_event_t)
#define STATE_MGR_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define STATE_MGR_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

static QueueHandle_t stateMgrQueueHandle = NULL;
static StaticQueue_t stateMgrQueue;
static uint8_t stateMgrQueueStack[STATE_MGR_QUEUE_LENGTH * STATE_MGR_QUEUE_ITEM_SIZE];
static uint8_t resetCounter = 0;

static comms_state_t commsManagerState = COMMS_STATE_DISCONNECTED;
static state_mgr_state_t cubeSatState = CUBESAT_STATE_INITIALIZATION;
/**
 * @brief Send all startup messages from the stateMgr task to other tasks.
 */
static void sendStartupMessages(void);

static void resetStateCounter(void);

/**
 * @brief Change the current state of the OBC
 *
 * @param newState The new state of the OBC
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 * @warning This function is not thread safe. It should only be called from the supervisor task.
 */
static obc_error_code_t changeStateOBC(uint8_t newState);

/**
 * @brief determines what the next Cubesat state should be and sets it to that state
 *
 * @param event the comms manager event triggering a state transition
 * @param state pointer to comms state variable
 *
 * @return obc_error_code_t - whether or not the state transition was successful
 */
static obc_error_code_t getNextCubeSatState(state_mgr_event_id_t event, state_mgr_state_t *state);

void obcTaskInitStateMgr(void) {
  ASSERT((stateMgrQueueStack != NULL) && (&stateMgrQueue != NULL));
  if (stateMgrQueueHandle == NULL) {
    stateMgrQueueHandle =
        xQueueCreateStatic(STATE_MGR_QUEUE_LENGTH, STATE_MGR_QUEUE_ITEM_SIZE, stateMgrQueueStack, &stateMgrQueue);
  }
}

obc_error_code_t sendToStateMgrEventQueue(state_mgr_event_t *event) {
  ASSERT(stateMgrQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(stateMgrQueueHandle, (void *)event, STATE_MGR_QUEUE_TX_WAIT_PERIOD) == pdPASS)
    return OBC_ERR_CODE_SUCCESS;

  return OBC_ERR_CODE_QUEUE_FULL;
}

static void sendStartupMessages(void) {}

static void resetStateCounter(void) {
  resetCounter++;
  telemetry_data_t resetTelem = {
      .id = TELEM_OBC_STATE, .timestamp = getCurrentUnixTime(), .resetStateCounter = resetCounter};
  RETURN_IF_ERROR_CODE(addTelemetryData(&resetTelem));
}

static obc_error_code_t changeStateOBC(uint8_t newState) {
  obc_error_code_t errCode;

  currStateOBC = newState;
  telemetry_data_t telemData = {.id = TELEM_OBC_STATE, .timestamp = getCurrentUnixTime(), .obcState = currStateOBC};
  RETURN_IF_ERROR_CODE(addTelemetryData(&telemData));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getNextCubeSatState(state_mgr_event_id_t event, state_mgr_state_t *state) {
  obc_error_code_t errCode;
  state_mgr_state_t tempStateVariable;
  if (state == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  switch (*state) {
    case CUBESAT_STATE_INITIALIZATION:
      handleInitializationState(event, &tempStateVariable);
      *state = tempStateVariable;

    case CUBESAT_STATE_NOMINAL:
      handleNormalState(event, &tempStateVariable);
      *state = tempStateVariable;

    case CUBESAT_STATE_ASSEMBLY:
      // TODO: probably remove this state, for now physically removing jumper on board and then doing power on reset
      // should bring to this stste not sure if there is a way to implement that in code
      break;

    case CUBESAT_STATE_LOW_PWR:
      handleLowPwrState(event, &tempStateVariable);
      *state = tempStateVariable;
    case CUBESAT_STATE_RESET:
      // TODO: According to the FSM chart, the OBC should just initialize the tasks again after this, so go back to
      // CUBESAT_STATE_INITIALIZATION state, there isn't really an event which would trigger a state change from the
      // CUBESAT_STATE_RESET state. Left blank for now as I am not sure what needs to be done here
      break;

    default:
      return OBC_ERR_CODE_INVALID_STATE;
  }
}

void obcTaskFunctionStateMgr(void *pvParameters) {
  obc_error_code_t errCode;

  ASSERT(stateMgrQueueHandle != NULL);

  if (cubeSatState == CUBESAT_STATE_INITIALIZATION) {
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_LOGGER);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_LOGGER);

    /* Initialize critical peripherals */
    // LOG_IF_ERROR_CODE(setupFileSystem());  // microSD card
    LOG_IF_ERROR_CODE(initTime());  // RTC

    lm75bd_config_t config = {
        .devAddr = LM75BD_OBC_I2C_ADDR,
        .devOperationMode = LM75BD_DEV_OP_MODE_NORMAL,
        .osFaultQueueSize = 2,
        .osPolarity = LM75BD_OS_POL_ACTIVE_LOW,
        .osOperationMode = LM75BD_OS_OP_MODE_COMP,
        .overTempThresholdCelsius = 125.0f,
        .hysteresisThresholdCelsius = 75.0f,
    };

    LOG_IF_ERROR_CODE(lm75bdInit(&config));  // LM75BD temperature sensor (OBC)

    initFRAM();  // FRAM storage (OBC)

    // Initialize the state of each module. This will not start any tasks.
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_TIMEKEEPER);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_ALARM_MGR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_TELEMETRY_MGR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMAND_MGR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_MGR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_UPLINK_DECODER);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_EPS_MGR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_PAYLOAD_MGR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_HEALTH_COLLECTOR);
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_GNC_MGR);
#if ENABLE_TASK_STATS_COLLECTOR == 1
    obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_STATS_COLLECTOR);
#endif

    /* Create all tasks*/
    taskENTER_CRITICAL();
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_TIMEKEEPER);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_ALARM_MGR);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_TELEMETRY_MGR);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_COMMAND_MGR);
    obcSchedulerCreateTaskWithArgs(OBC_SCHEDULER_CONFIG_ID_COMMS_MGR, &commsManagerState);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_COMMS_UPLINK_DECODER);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_EPS_MGR);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_PAYLOAD_MGR);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_HEALTH_COLLECTOR);
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_GNC_MGR);
#if ENABLE_TASK_STATS_COLLECTOR == 1
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATS_COLLECTOR);
#endif
    obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_DIGITAL_WATCHDOG_MGR);
    taskEXIT_CRITICAL();

    /* Send initial messages to system queues */
    sendStartupMessages();

    state_mgr_event_t tasksRunningEvent = {
        .eventID = STATE_MGR_TASKS_RUNNING_EVENT_ID,
        .data =
            {
                .i = 0,
            },

    };
    sendToStateMgrEventQueue(&tasksRunningEvent);
  }

  while (1) {
    state_mgr_event_t inMsg;

    if (xQueueReceive(stateMgrQueueHandle, &inMsg, STATE_MGR_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
#if defined(DEBUG) && !defined(OBC_REVISION_2)
      vTaskDelay(pdMS_TO_TICKS(1000));
      gioToggleBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT);
#endif
      continue;
    }

    LOG_IF_ERROR_CODE(getNextCubeSatState(inMsg.eventID, &cubeSatState));
  }
}

obc_error_code_t handleInitializationState(state_mgr_event_id_t event, state_mgr_state_t *state) {
  switch (event) {
    case STATE_MGR_TASKS_RUNNING_EVENT_ID:
      *state = CUBESAT_STATE_NOMINAL;
      return OBC_ERR_CODE_SUCCESS;

    default:
      return OBC_ERR_CODE_INVALID_STATE_TRANSITION;
  }
}

obc_error_code_t handleNormalState(state_mgr_event_id_t event, state_mgr_state_t *state) {
  switch (event) {
    case STATE_MGR_RESET_EVENT_ID:
      *state = CUBESAT_STATE_RESET;
      resetStateCounter();
      return OBC_ERR_CODE_SUCCESS;

    case STATE_MGR_LOWPWR_EVENT_ID:
      *state = CUBESAT_STATE_LOW_PWR;
      return OBC_ERR_CODE_SUCCESS;

    default:
      return OBC_ERR_CODE_INVALID_STATE_TRANSITION;
  }
}

obc_error_code_t handleAssemblyState() {}

obc_error_code_t handleLowPwrState(state_mgr_event_id_t event, state_mgr_state_t *state) {
  switch (event) {
    case STATE_MGR_RESET_EVENT_ID:
      *state = CUBESAT_STATE_RESET;
      resetStateCounter();
      return OBC_ERR_CODE_SUCCESS;

    default:
      return OBC_ERR_CODE_INVALID_STATE;
  }
}
