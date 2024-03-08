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
#include "obc_reset.h"
#include "obc_persistent.h"

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

static comms_state_t commsManagerState = COMMS_STATE_DISCONNECTED;

uint8_t resetReason = RESET_REASON_UNKNOWN;

/**
 * @brief Send all startup messages from the stateMgr task to other tasks.
 */
static void sendStartupMessages(void);

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

void obcTaskFunctionStateMgr(void *pvParameters) {
  obc_error_code_t errCode;

  ASSERT(stateMgrQueueHandle != NULL);

  obcSchedulerInitTask(OBC_SCHEDULER_CONFIG_ID_LOGGER);
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_LOGGER);

  /* Initialize critical peripherals */
  // LOG_IF_ERROR_CODE(setupFileSystem());  // microSD card
  LOG_IF_ERROR_CODE(initTime());  // RTC

  initFRAM();  // FRAM storage (OBC)

  /* if reset was done in software then read fram for reason */
  if (resetReason == RESET_REASON_SW_RESET || resetReason == RESET_REASON_CPU_RESET ||
      resetReason == RESET_REASON_DIG_WATCHDOG_RESET) {
    obc_reset_reason_t storedReason = RESET_REASON_UNKNOWN;
    LOG_IF_ERROR_CODE(
        getPersistentData(OBC_PERSIST_SECTION_ID_RESET_REASON, &storedReason, sizeof(obc_reset_reason_t)));
    if (storedReason != RESET_REASON_UNKNOWN) {
      resetReason = storedReason;
    }
  }

  LOG_ERROR_CODE(resetReason + RESET_REASON_ERROR_CODE_OFFSET);

  resetReason = RESET_REASON_UNKNOWN;
  LOG_IF_ERROR_CODE(setPersistentData(OBC_PERSIST_SECTION_ID_RESET_REASON, &resetReason, sizeof(obc_reset_reason_t)));

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
#if ENABLE_TASK_STATS_COLLECTOR == 1
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_STATS_COLLECTOR);
#endif
  obcSchedulerCreateTask(OBC_SCHEDULER_CONFIG_ID_DIGITAL_WATCHDOG_MGR);
  taskEXIT_CRITICAL();

  // TODO: Deal with errors
  LOG_IF_ERROR_CODE(changeStateOBC(OBC_STATE_INITIALIZING));

  /* Send initial messages to system queues */
  sendStartupMessages();

  // TODO: Deal with errors
  LOG_IF_ERROR_CODE(changeStateOBC(OBC_STATE_NORMAL));

  while (1) {
    state_mgr_event_t inMsg;

    if (xQueueReceive(stateMgrQueueHandle, &inMsg, STATE_MGR_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
#if defined(DEBUG) && !defined(OBC_REVISION_2)
      vTaskDelay(pdMS_TO_TICKS(1000));
      gioToggleBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT);
#endif
      continue;
    }

    switch (inMsg.eventID) {
      default:
        LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_EVENT);
    }
  }
}
