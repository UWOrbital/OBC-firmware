#ifndef CDH_INCLUDE_TELEMETRY_PACK_H_
#define CDH_INCLUDE_TELEMETRY_PACK_H_

#include "obc_errors.h"
#include "telemetry_manager.h"
#include "encode_telemetry.h"

#include <stdint.h>
#include <stddef.h>

typedef size_t (*telemetry_pack_func_t)(telemetry_data_t *, uint8_t *);

/**
 * @brief Packs/serializes a telemetry struct into a buffer
 * 
 * @param data - Pointer to the telemetry struct to pack
 * @param buffer - Pointer to the buffer to pack the telemetry struct into
 * @param buffLen - Length of the buffer
 * @param numBytesPacked - Pointer to the number of bytes packed into the buffer at the end of the function
 * @return obc_error_code_t - OBC_ERR_CODE_SUCCESS if all bytes were packed successfully
 */
obc_error_code_t packTelemetry(telemetry_data_t *data, uint8_t *buffer, size_t buffLen, size_t *numBytesPacked);

/* Declare all pack functions for telemetry data */
size_t packCC1120Temp(telemetry_data_t *data, uint8_t *buffer);
size_t packCommsCustomTransceiverTemp(telemetry_data_t *data, uint8_t *buffer);
size_t packObcTemp(telemetry_data_t *data, uint8_t *buffer);
size_t packAdcsMagBoardTemp(telemetry_data_t *data, uint8_t *buffer);
size_t packAdcsSensorBoardTemp(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsBoardTemp(telemetry_data_t *data, uint8_t *buffer);
size_t packSolarPanel1Temp(telemetry_data_t *data, uint8_t *buffer);
size_t packSolarPanel2Temp(telemetry_data_t *data, uint8_t *buffer);
size_t packSolarPanel3Temp(telemetry_data_t *data, uint8_t *buffer);
size_t packSolarPanel4Temp(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsComms5vCurrent(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsComms3v3Current(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsMagnetorquer8vCurrent(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsAdcs5vCurrent(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsAdcs3v3Current(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsObc3v3Current(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsComms5vVoltage(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsComms3v3Voltage(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsMagnetorquer8vVoltage(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsAdcs5vVoltage(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsAdcs3v3Voltage(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsObc3v3Voltage(telemetry_data_t *data, uint8_t *buffer);
size_t packObcState(telemetry_data_t *data, uint8_t *buffer);
size_t packEpsState(telemetry_data_t *data, uint8_t *buffer);
size_t packNumCspPacketsRcvd(telemetry_data_t *data, uint8_t *buffer);

#endif /* CDH_INCLUDE_TELEMETRY_PACK_H_ */
