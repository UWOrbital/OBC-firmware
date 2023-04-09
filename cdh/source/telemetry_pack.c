#include "telemetry_pack.h"
#include "telemetry_manager.h"
#include "obc_pack_utils.h"
#include "obc_logging.h"
#include "encode_telemetry.h"

#include <stddef.h>
#include <stdint.h>

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

static obc_error_code_t packTelemetryId(telemetry_data_t *data, uint8_t *buffer, size_t *numBytesPacked);
static obc_error_code_t packTelemetryTimestamp(telemetry_data_t *data, uint8_t *buffer, size_t *numBytesPacked);
static obc_error_code_t packTelemetryParameters(telemetry_data_t *data, uint8_t *buffer, size_t *numBytesPacked);

/**
 * @brief Packs/serializes a telemetry struct into a buffer
 * 
 * @param data - Pointer to the telemetry struct to pack
 * @param buffer - Pointer to the buffer to pack the telemetry struct into
 * @param buffLen - Length of the buffer
 * @param numBytesPacked - Pointer to the number of bytes packed into the buffer at the end of the function
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if all bytes were packed successfully
 */
obc_error_code_t packTelemetry(telemetry_data_t *data, uint8_t *buffer, size_t buffLen, size_t *numBytesPacked) {
    obc_error_code_t errCode;
    
    if (data == NULL || buffer == NULL || numBytesPacked == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (buffLen < MAX_TELEMETRY_DATA_SIZE) {
        // Enforce that the buffer is large enough to hold the maximum amount of telemetry data
        // This is to prevent the possibility of a buffer overflow
        return OBC_ERR_CODE_BUFF_TOO_SMALL;
    }

    size_t bytesWritten = 0;

    RETURN_IF_ERROR_CODE(packTelemetryId(data, buffer, &bytesWritten));
    (*numBytesPacked) += bytesWritten;
    RETURN_IF_ERROR_CODE(packTelemetryTimestamp(data, buffer + bytesWritten, &bytesWritten));
    (*numBytesPacked) += bytesWritten;
    RETURN_IF_ERROR_CODE(packTelemetryParameters(data, buffer + bytesWritten, &bytesWritten));
    (*numBytesPacked) += bytesWritten;
    
    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t packTelemetryId(telemetry_data_t *data, uint8_t *buffer, size_t *numBytesPacked) {
    if (data == NULL || buffer == NULL || numBytesPacked == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    size_t offset = 0;
    packUint8(data->id, buffer, &offset);
    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t packTelemetryTimestamp(telemetry_data_t *data, uint8_t *buffer, size_t *numBytesPacked) {
    if (data == NULL || buffer == NULL || numBytesPacked == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    size_t offset = 0;
    packUint32(data->timestamp, buffer, &offset);
    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t packTelemetryParameters(telemetry_data_t *data, uint8_t *buffer, size_t *numBytesPacked) {
    if (data == NULL || buffer == NULL || numBytesPacked == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (telemPackFns[data->id] == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *numBytesPacked = telemPackFns[data->id](data, buffer);
    return OBC_ERR_CODE_SUCCESS;
}


size_t packCC1120Temp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->cc1120Temp, buffer, &offset);
    return offset;
}

size_t packCommsCustomTransceiverTemp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->commsCustomTransceiverTemp, buffer, &offset);
    return offset;
}

size_t packObcTemp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->obcTemp, buffer, &offset);
    return offset;
}

size_t packAdcsMagBoardTemp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->adcsMagBoardTemp, buffer, &offset);
    return offset;
}

size_t packAdcsSensorBoardTemp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->adcsSensorBoardTemp, buffer, &offset);
    return offset;
}

size_t packEpsBoardTemp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsBoardTemp, buffer, &offset);
    return offset;
}

size_t packSolarPanel1Temp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->solarPanel1Temp, buffer, &offset);
    return offset;
}

size_t packSolarPanel2Temp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->solarPanel2Temp, buffer, &offset);
    return offset;
}

size_t packSolarPanel3Temp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->solarPanel3Temp, buffer, &offset);
    return offset;
}

size_t packSolarPanel4Temp(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->solarPanel4Temp, buffer, &offset);
    return offset;
}

size_t packEpsComms5vCurrent(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsComms5vCurrent, buffer, &offset);
    return offset;
}

size_t packEpsComms3v3Current(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsComms3v3Current, buffer, &offset);
    return offset;
}

size_t packEpsMagnetorquer8vCurrent(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsMagnetorquer8vCurrent, buffer, &offset);
    return offset;
}

size_t packEpsAdcs5vCurrent(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsAdcs5vCurrent, buffer, &offset);
    return offset;
}

size_t packEpsAdcs3v3Current(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsAdcs3v3Current, buffer, &offset);
    return offset;
}

size_t packEpsObc3v3Current(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsObc3v3Current, buffer, &offset);
    return offset;
}

size_t packEpsComms5vVoltage(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsComms5vVoltage, buffer, &offset);
    return offset;
}

size_t packEpsComms3v3Voltage(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsComms3v3Voltage, buffer, &offset);
    return offset;
}

size_t packEpsMagnetorquer8vVoltage(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsMagnetorquer8vVoltage, buffer, &offset);
    return offset;
}

size_t packEpsAdcs5vVoltage(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsAdcs5vVoltage, buffer, &offset);
    return offset;
}

size_t packEpsAdcs3v3Voltage(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsAdcs3v3Voltage, buffer, &offset);
    return offset;
}

size_t packEpsObc3v3Voltage(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packFloat(data->epsObc3v3Voltage, buffer, &offset);
    return offset;
}

size_t packObcState(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packUint8(data->obcState, buffer, &offset);
    return offset;
}

size_t packEpsState(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packUint8(data->epsState, buffer, &offset);
    return offset;
}

size_t packNumCspPacketsRcvd(telemetry_data_t *data, uint8_t *buffer) {
    size_t offset = 0;
    packUint32(data->numCspPacketsRcvd, buffer, &offset);
    return offset;
}
