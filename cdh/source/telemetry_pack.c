#include "telemetry_pack.h"
#include "telemetry_manager.h"
#include "obc_pack_utils.h"

#include <stddef.h>
#include <stdint.h>

typedef void (*telemetry_pack_func_t)(telemetry_data_t *, uint8_t *, size_t *);

static const telemetry_pack_func_t telemPackFns[] = {
    [TELEM_CC1120_TEMP] = packCC1120Temp,
    [TELEM_COMMS_CUSTOM_TRANSCEIVER_TEMP] = packCommsCustomTransceiverTemp,
    [TELEM_OBC_TEMP] = packObcTemp,
    [TELEM_ADCS_MAG_BOARD_TEMP] = packAdcsMagBoardTemp,
    [TELEM_ADCS_SENSOR_BOARD_TEMP] = packAdcsSensorBoardTemp,
    [TELEM_EPS_BOARD_TEMP] = packEpsBoardTemp,
    [TELEM_SOLAR_PANEL_1_TEMP] = packSolarPanel1Temp,
    [TELEM_SOLAR_PANEL_2_TEMP] = packSolarPanel2Temp,
    [TELEM_SOLAR_PANEL_3_TEMP] = packSolarPanel3Temp,
    [TELEM_SOLAR_PANEL_4_TEMP] = packSolarPanel4Temp,
    [TELEM_EPS_COMMS_5V_CURRENT] = packEpsComms5vCurrent,
    [TELEM_EPS_COMMS_3V3_CURRENT] = packEpsComms3v3Current,
    [TELEM_EPS_MAGNETORQUER_8V_CURRENT] = packEpsMagnetorquer8vCurrent,
    [TELEM_EPS_ADCS_5V_CURRENT] = packEpsAdcs5vCurrent,
    [TELEM_EPS_ADCS_3V3_CURRENT] = packEpsAdcs3v3Current,
    [TELEM_EPS_OBC_3V3_CURRENT] = packEpsObc3v3Current,
    [TELEM_EPS_COMMS_5V_VOLTAGE] = packEpsComms5vVoltage,
    [TELEM_EPS_COMMS_3V3_VOLTAGE] = packEpsComms3v3Voltage,
    [TELEM_EPS_MAGNETORQUER_8V_VOLTAGE] = packEpsMagnetorquer8vVoltage,
    [TELEM_EPS_ADCS_5V_VOLTAGE] = packEpsAdcs5vVoltage,
    [TELEM_EPS_ADCS_3V3_VOLTAGE] = packEpsAdcs3v3Voltage,
    [TELEM_EPS_OBC_3V3_VOLTAGE] = packEpsObc3v3Voltage,
    [TELEM_OBC_STATE] = packObcState,
    [TELEM_EPS_STATE] = packEpsState,
    [TELEM_NUM_CSP_PACKETS_RCVD] = packNumCspPacketsRcvd,
};

static void packTelemetryId(telemetry_data_id_t id, uint8_t *buffer, size_t *offset);
static void packTelemetryTimestamp(uint32_t timestamp, uint8_t *buffer, size_t *offset);

obc_error_code_t packTelemetry(telemetry_data_t *data, uint8_t *buffer, size_t len, size_t *numPacked) {
    if (data == NULL || buffer == NULL || numPacked == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (len < MAX_TELEMETRY_DATA_SIZE) {
        return OBC_ERR_CODE_BUFF_TOO_SMALL;
    }
    
    if (telemPackFns[data->id] == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    size_t offset = 0;

    // Pack the telemetry ID
    packTelemetryId(data->id, buffer, &offset);

    // Pack the timestamp
    packTelemetryTimestamp(data->timestamp, buffer, &offset);

    // Pack the telemetry parameters
    telemPackFns[data->id](data, buffer, &offset);

    *numPacked = offset;

    return OBC_ERR_CODE_SUCCESS;
}

static void packTelemetryId(telemetry_data_id_t id, uint8_t *buffer, size_t *offset) {
    packUint8(id, buffer, offset);
}

static void packTelemetryTimestamp(uint32_t timestamp, uint8_t *buffer, size_t *offset) {
    packUint32(timestamp, buffer, offset);
}

void packCC1120Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->cc1120Temp, buffer, offset);
}

void packCommsCustomTransceiverTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->commsCustomTransceiverTemp, buffer, offset);
}

void packObcTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->obcTemp, buffer, offset);
}

void packAdcsMagBoardTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->adcsMagBoardTemp, buffer, offset);
}

void packAdcsSensorBoardTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->adcsSensorBoardTemp, buffer, offset);
}

void packEpsBoardTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsBoardTemp, buffer, offset);
}

void packSolarPanel1Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->solarPanel1Temp, buffer, offset);
}

void packSolarPanel2Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->solarPanel2Temp, buffer, offset);
}

void packSolarPanel3Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->solarPanel3Temp, buffer, offset);
}

void packSolarPanel4Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->solarPanel4Temp, buffer, offset);
}

void packEpsComms5vCurrent(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsComms5vCurrent, buffer, offset);
}

void packEpsComms3v3Current(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsComms3v3Current, buffer, offset);
}

void packEpsMagnetorquer8vCurrent(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsMagnetorquer8vCurrent, buffer, offset);
}

void packEpsAdcs5vCurrent(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsAdcs5vCurrent, buffer, offset);
}

void packEpsAdcs3v3Current(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsAdcs3v3Current, buffer, offset);
}

void packEpsObc3v3Current(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsObc3v3Current, buffer, offset);
}

void packEpsComms5vVoltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsComms5vVoltage, buffer, offset);
}

void packEpsComms3v3Voltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsComms3v3Voltage, buffer, offset);
}

void packEpsMagnetorquer8vVoltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsMagnetorquer8vVoltage, buffer, offset);
}

void packEpsAdcs5vVoltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsAdcs5vVoltage, buffer, offset);
}

void packEpsAdcs3v3Voltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsAdcs3v3Voltage, buffer, offset);
}

void packEpsObc3v3Voltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packFloat(data->epsObc3v3Voltage, buffer, offset);
}

void packObcState(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packUint8(data->obcState, buffer, offset);
}

void packEpsState(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packUint8(data->epsState, buffer, offset);
}

void packNumCspPacketsRcvd(telemetry_data_t *data, uint8_t *buffer, size_t *offset) {
    packUint32(data->numCspPacketsRcvd, buffer, offset);
}
