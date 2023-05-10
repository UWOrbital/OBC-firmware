#ifndef CDH_INCLUDE_TELEMETRY_H_
#define CDH_INCLUDE_TELEMETRY_H_

#include "obc_errors.h"
#include "obc_states.h"

#include <stdint.h>
#include <stddef.h>

typedef enum {
    /* Used to indicate that the telemetry data is invalid.
       It should not have a pack function. */
    TELEM_NONE = 0,

    // Temperature values
    TELEM_CC1120_TEMP,
    TELEM_COMMS_CUSTOM_TRANSCEIVER_TEMP,
    TELEM_OBC_TEMP,
    TELEM_ADCS_MAG_BOARD_TEMP,
    TELEM_ADCS_SENSOR_BOARD_TEMP,
    TELEM_EPS_BOARD_TEMP,
    TELEM_SOLAR_PANEL_1_TEMP,
    TELEM_SOLAR_PANEL_2_TEMP,
    TELEM_SOLAR_PANEL_3_TEMP,
    TELEM_SOLAR_PANEL_4_TEMP,

    // Current values
    TELEM_EPS_COMMS_5V_CURRENT,
    TELEM_EPS_COMMS_3V3_CURRENT,
    TELEM_EPS_MAGNETORQUER_8V_CURRENT,
    TELEM_EPS_ADCS_5V_CURRENT,
    TELEM_EPS_ADCS_3V3_CURRENT,
    TELEM_EPS_OBC_3V3_CURRENT,

    // Voltage values
    TELEM_EPS_COMMS_5V_VOLTAGE,
    TELEM_EPS_COMMS_3V3_VOLTAGE,
    TELEM_EPS_MAGNETORQUER_8V_VOLTAGE,
    TELEM_EPS_ADCS_5V_VOLTAGE,
    TELEM_EPS_ADCS_3V3_VOLTAGE,
    TELEM_EPS_OBC_3V3_VOLTAGE,
    
    TELEM_OBC_STATE,
    TELEM_EPS_STATE,

    TELEM_NUM_CSP_PACKETS_RCVD,  
} telemetry_data_id_t;

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

        obc_state_t obcState;
        uint8_t epsState;

        uint32_t numCspPacketsRcvd;
    };

    telemetry_data_id_t id;
    uint32_t timestamp; // seconds since epoch
    
} telemetry_data_t;

#define MAX_TELEMETRY_DATA_SIZE sizeof(telemetry_data_t)

/**
 * @brief	Initialize the telemetry task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initTelemetry(void);

/**
 * @brief	Adds a telemetry data point to the telemetry queue
 * @param	data Pointer to the telemetry data point to add
 * @return  obc_error_code_t OBC_ERR_CODE_SUCCESS if the data was added to the queue, error code otherwise
 */
obc_error_code_t addTelemetryData(telemetry_data_t *data);

#endif /* CDH_INCLUDE_TELEMETRY_H_ */
