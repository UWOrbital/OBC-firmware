#include "thermal_mgr.h"

// sensor imports for telemetry
#include "lm75bd.h"
#include "cc1120.h"
#include "cc1120_defs.h"
#include "ds3232_mz.h"
// bd621x, fm25V025A, MAX5360, and RFFM6404 doesn't have a temperature sensors

#include "obc_time.h"
#include "telemetry_manager.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_scheduler_config.h"
#include <obc_digital_watchdog.h>
#include "digital_watchdog_mgr.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_task.h>
#include <os_queue.h>

#define THERMAL_MGR_PERIOD_MS 1000

void obcTaskInitThermalMgr() {}
static obc_error_code_t collectObcLm75bdTemp(void);
static obc_error_code_t collectObcDs3232Temp(void);
static obc_error_code_t collectObcCC1120Temp(void);
static obc_error_code_t collectBMSTemp(void);

void obcTaskFunctionThermalMgr(void* pvParameters) {
  obc_error_code_t errCode;
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    LOG_IF_ERROR_CODE(collectObcLm75bdTemp());
    LOG_IF_ERROR_CODE(collectObcCC1120Temp());
    LOG_IF_ERROR_CODE(collectObcDs3232Temp());  // RTC
    LOG_IF_ERROR_CODE(collectBMSTemp());

    digitalWatchdogTaskCheckIn(OBC_SCHEDULER_CONFIG_ID_THERMAL_MGR);

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(THERMAL_MGR_PERIOD_MS));
  }
}

static obc_error_code_t collectObcLm75bdTemp(void) {
  obc_error_code_t errCode;

  float temp = 0.0f;
  RETURN_IF_ERROR_CODE(readTempLM75BD(LM75BD_OBC_I2C_ADDR, &temp));

  telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_LM75BD_TEMP, .timestamp = getCurrentUnixTime()};

  RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t collectObcCC1120Temp(void) {
  // function commented out due to cc1120 driver function being implemented in a different PR
  //  obc_error_code_t errCode;

  // float temp = 0.0f;
  // RETURN_IF_ERROR_CODE(readTempCC1120(&temp));

  // telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_CC1120_TEMP, .timestamp = getCurrentUnixTime()};

  // RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t collectObcDs3232Temp(void) {
  obc_error_code_t errCode;

  float temp = 0.0f;
  RETURN_IF_ERROR_CODE(getTemperatureRTC(&temp));

  telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_DS3232_TEMP, .timestamp = getCurrentUnixTime()};

  RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t collectBMSTemp(void) {
  obc_error_code_t errCode;

  // BMS in pr https://github.com/UWOrbital/OBC-firmware/pull/187
  // float temp = 0.0f;
  // RETURN_IF_ERROR_CODE(getTempBMS(&temp)); //this function should get the temperature from the BMS/MAX17320

  // telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_BMS_TEMP, .timestamp = getCurrentUnixTime()};

  // RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  return OBC_ERR_CODE_SUCCESS;
}
