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

void obcTaskFunctionThermalMgr(void* pvParameters) {
  obc_error_code_t errCode;
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();

  while (1) {
    LOG_IF_ERROR_CODE(collectObcLm75bdTemp());
    LOG_IF_ERROR_CODE(collectObcCC1120Temp());
    LOG_IF_ERROR_CODE(collectObcDs3232Temp());
    digitalWatchdogTaskCheckIn(OBC_SCHEDULER_CONFIG_ID_THERMAL_MGR);

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

static obc_error_code_t setObcCC1120TempReading(void) {
  obc_error_code_t errCode;

  // To read the temperature sensor on the CC1120, first set the GPIO1 pin to analog output via setting 0x80 on IOCFG1
  // (It has no digital reading for temperature)
  uint8_t data = 0x80;
  RETURN_IF_ERROR_CODE(cc1120WriteSpi(CC1120_REGS_IOCFG1, &data, sizeof(data)));

  // Conifiguration of registers to read output: GBIAS1 to 0x07, ATEST_MODE to 0x0C, ATEST to 0x2A
  data = 0x07;
  RETURN_IF_ERROR_CODE(cc1120WriteExtAddrSpi(CC1120_REGS_EXT_GBIAS1, &data, sizeof(data)));
  data = 0x0C;
  RETURN_IF_ERROR_CODE(cc1120WriteExtAddrSpi(CC1120_REGS_EXT_ATEST_MODE, &data, sizeof(data)));
  data = 0x2A;
  RETURN_IF_ERROR_CODE(cc1120WriteExtAddrSpi(CC1120_REGS_EXT_ATEST, &data, sizeof(data)));
}

static obc_error_code_t resetObcCC120TempConfig(void) {
  obc_error_code_t errCode;
  // reset registers

  uint8_t data = 0;
  data = 0x30;
  RETURN_IF_ERROR_CODE(cc1120WriteSpi(CC1120_REGS_IOCFG1, &data, sizeof(data)));

  data = CC1120_EXT_DEFAULTS_GBIAS1;
  RETURN_IF_ERROR_CODE(cc1120WriteExtAddrSpi(CC1120_REGS_EXT_GBIAS1, &data, sizeof(data)));
  data = CC1120_EXT_DEFAULTS_ATEST_MODE;
  RETURN_IF_ERROR_CODE(cc1120WriteExtAddrSpi(CC1120_REGS_EXT_ATEST_MODE, &data, sizeof(data)));
  data = CC1120_EXT_DEFAULTS_ATEST;
  RETURN_IF_ERROR_CODE(cc1120WriteExtAddrSpi(CC1120_REGS_EXT_ATEST, &data, sizeof(data)));
}

static obc_error_code_t collectObcCC1120Temp(void) {
  obc_error_code_t errCode;

  float temp = 0.0f;
  RETURN_IF_ERROR_CODE(setObcCC1120TempReading());

  uint16_t tempData = 0;
  // read output voltage on GPIO1
  RETURN_IF_ERROR_CODE(
      cc1120ReadSpi(CC1120_REGS_IOCFG1, &tempData,
                    sizeof(tempData)));  // this voltage represents the PTAT voltage but has an error of +/- 10C

  // Use Single Point Calibration to get a more accurate temperature reading (within +/- 1C for limited temperature
  // range or +/- 2C across -40C to 85C) Can also use Two Point Calibtration for better accuracy
  //  See sections 1.4 and 1.5 of https://www.ti.com/lit/an/swra415d/swra415d.pdf?ts=1730233031162 (CC112x datasheet)
  //  for more information

  telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_OBC_TEMP, .timestamp = getCurrentUnixTime()};

  RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  RETURN_IF_ERROR_CODE(resetObcCC120TempConfig());

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t collectObcDs3232Temp(void) {
  obc_error_code_t errCode;

  float temp = 0.0f;
  RETURN_IF_ERROR_CODE(getTemperatureRTC(&temp));

  telemetry_data_t obcTempVal = {.obcTemp = temp, .id = TELEM_OBC_TEMP, .timestamp = getCurrentUnixTime()};

  RETURN_IF_ERROR_CODE(addTelemetryData(&obcTempVal));

  return OBC_ERR_CODE_SUCCESS;
}
