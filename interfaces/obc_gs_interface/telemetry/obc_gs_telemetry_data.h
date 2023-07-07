#pragma once

#include "obc_gs_telemetry_id.h"

#include <stdint.h>
#include <stddef.h>

typedef struct {
  union {
    // Temperature values
    float cc1120Temp;
    float commsCustomTransceiverTemp;
    float obcTemp;
    float adcsMagBoardTemp;
    float adcsSensorBoardTemp;
    float epsBoardTemp;
    float solarPanel1Temp;
    float solarPanel2Temp;
    float solarPanel3Temp;
    float solarPanel4Temp;

    // Current values
    float epsComms5vCurrent;
    float epsComms3v3Current;
    float epsMagnetorquer8vCurrent;
    float epsAdcs5vCurrent;
    float epsAdcs3v3Current;
    float epsObc3v3Current;

    // Voltage values
    float epsComms5vVoltage;
    float epsComms3v3Voltage;
    float epsMagnetorquer8vVoltage;
    float epsAdcs5vVoltage;
    float epsAdcs3v3Voltage;
    float epsObc3v3Voltage;

    uint8_t obcState;
    uint8_t epsState;

    uint32_t numCspPacketsRcvd;
  };

  telemetry_data_id_t id;
  uint32_t timestamp;  // seconds since epoch

} telemetry_data_t;

#define MAX_TELEMETRY_DATA_SIZE sizeof(telemetry_data_t)
