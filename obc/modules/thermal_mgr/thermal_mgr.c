#include "thermal_mgr.h"
#include "lm75bd.h"
#include "obc_time.h"
#include "telemetry_manager.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_scheduler_config.h"
#include <obc_digital_watchdog.h>

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_task.h>
#include <os_queue.h>

#include <gio.h>

#define THERMAL_MGR_PERIOD_MS 60

static QueueHandle_t thermalMgrQueueHandle = NULL;
static StaticQueue_t thermalMgrQueue;
static uint8_t thermalMgrQueueStack[THERMAL_MANAGER_QUEUE_LENGTH * THERMAL_MANAGER_QUEUE_ITEM_SIZE];

static obc_error_code_t collectObcLm75bdTemp(void);

void obcTaskInitThermalMgr(void) {
  ASSERT((thermalMgrQueueStack != NULL) && (&thermalMgrQueue != NULL));
  if (thermalMgrQueueHandle == NULL) {
    thermalMgrQueueHandle = xQueueCreateStatic(THERMAL_MANAGER_QUEUE_LENGTH, THERMAL_MANAGER_QUEUE_ITEM_SIZE,
                                               thermalMgrQueueStack, &thermalMgrQueue);
  }
}

obc_error_code_t sendToThermalMgrQueue(thermal_mgr_event_t* event) {
  ASSERT(thermalMgrQueueHandle != NULL);

  if (event == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (xQueueSend(thermalMgrQueueHandle, (void*)event, THERMAL_MANAGER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
    return OBC_ERR_CODE_SUCCESS;
  }

  return OBC_ERR_CODE_QUEUE_FULL;
}

void obcTaskFunctionThermalMgr(void* pvParameters) {
  obc_error_code_t errCode;
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    LOG_IF_ERROR_CODE(collectObcLm75bdTemp());
    feedDigitalWatchdog();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(THERMAL_MGR_PERIOD_MS));
  }
}

static obc_error_code_t collectObcLm75bdTemp(void) {
  obc_error_code_t errCode;

  float temp = 0.0f;
  RETURN_IF_ERROR_CODE(readTempLM75BD(LM75BD_OBC_I2C_ADDR, &temp));

  telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_OBC_TEMP, .timestamp = getCurrentUnixTime()};

  RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  return OBC_ERR_CODE_SUCCESS;
}
