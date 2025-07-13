#include "thermal_mgr.h"
#include "lm75bd.h"
#include "obc_time.h"
#include "telemetry_manager.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_scheduler_config.h"
#include "digital_watchdog_mgr.h"
#include "obc_print.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <sys_common.h>
#include <stdio.h>

static obc_error_code_t collectObcLm75bdTemp(void);
static obc_error_code_t collectCc1120Temp(void);

// least significant bit is whether temperature has been read
// everything else are the data bits
extern uint32_t cc1120TemperatureData;
typedef enum {
  OBC_TEMP = 0,
  CC1120_TEMP,
} thermal_mgr_temperature_id_t;

typedef obc_error_code_t (*thermal_mgr_telemetry_func_t)(void);
static const thermal_mgr_telemetry_func_t thermalMgrTelemetryFns[] = {
    [OBC_TEMP] = collectObcLm75bdTemp, [CC1120_TEMP] = collectCc1120Temp};

void obcTaskInitThermalMgr(void) {}

void obcTaskFunctionThermalMgr(void* pvParameters) {
  obc_error_code_t errCode;

  while (1) {
    digitalWatchdogTaskCheckIn(OBC_SCHEDULER_CONFIG_ID_THERMAL_MGR);
    // add telemetry data
    for (uint8 i = 0; i < sizeof(thermalMgrTelemetryFns) / sizeof(thermal_mgr_telemetry_func_t); i++) {
      if (thermalMgrTelemetryFns[i] != NULL) {
        sciPrintf("Running thermal manager telemetry function %d\r\n", i);
        LOG_IF_ERROR_CODE(thermalMgrTelemetryFns[i]());
      }
    }
    vTaskDelay(pdMS_TO_TICKS(THERMAL_MGR_PERIOD_MS));
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

#define COLLECT_TEMP_DATA(tempData, tempId, tempName)                                               \
  do {                                                                                              \
    obc_error_code_t errCode;                                                                       \
    bool isValid;                                                                                   \
    RETURN_IF_ERROR_CODE(isTemperatureValid(tempData, &isValid));                                   \
    if (!isValid) {                                                                                 \
      return OBC_ERR_CODE_INVALID_STATE;                                                            \
    }                                                                                               \
    uint32_t temp = 0;                                                                              \
    RETURN_IF_ERROR_CODE(getTemperatureData(tempData, &temp));                                      \
    sciPrintf("Thermal manager: %s temperature is %d\r\n", #tempName, temp);                        \
    telemetry_data_t tempVal = {.tempName = temp, .id = tempId, .timestamp = getCurrentUnixTime()}; \
    RETURN_IF_ERROR_CODE(addTelemetryData(&tempVal));                                               \
    RETURN_IF_ERROR_CODE(setTemperatureData(&tempData, temp, false));                               \
  } while (0)
static obc_error_code_t collectCc1120Temp(void) {
  COLLECT_TEMP_DATA(cc1120TemperatureData, TELEM_CC1120_TEMP, cc1120Temp);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setTemperatureData(uint32_t* temperatureData, uint32_t temperature, bool isValid) {
  if (temperatureData == NULL || (temperature & 0x80000000) != 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  *temperatureData = (temperature << 1) | (isValid ? 0b1 : 0b0);

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getTemperatureData(uint32_t temperatureData, uint32_t* temperature) {
  if (temperature == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;

  // Extract the temperature data from the temperatureData variable
  *temperature = temperatureData >> 1;

  // Check if the temperature is valid
  bool isValid;
  RETURN_IF_ERROR_CODE(isTemperatureValid(temperatureData, &isValid));
  if (isValid) {
    return OBC_ERR_CODE_SUCCESS;
  } else {
    return OBC_ERR_CODE_INVALID_STATE;
  }
}

obc_error_code_t isTemperatureValid(uint32_t temperatureData, bool* isValid) {
  if (isValid == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if ((temperatureData & 0b1) == 1) {
    *isValid = true;
  } else {
    *isValid = false;
  }

  return OBC_ERR_CODE_SUCCESS;
}
