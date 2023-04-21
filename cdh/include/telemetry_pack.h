#ifndef CDH_INCLUDE_TELEMETRY_PACK_H_
#define CDH_INCLUDE_TELEMETRY_PACK_H_

#include "obc_errors.h"
#include "telemetry_manager.h"

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Pack telemetry data into a buffer of bytes
 * 
 * @param data The telemetry data to pack
 * @param buffer The buffer to pack the data into
 * @param buffLen The length of the buffer
 * @param numPacked The number of bytes packed into the buffer
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the data was packed successfully, otherwise an error code
 */
obc_error_code_t packTelemetry(telemetry_data_t *data, uint8_t *buffer, size_t buffLen, size_t *numPacked);

/* Declare all pack functions for telemetry data */
void packCC1120Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packCommsCustomTransceiverTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packObcTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packAdcsMagBoardTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packAdcsSensorBoardTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsBoardTemp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packSolarPanel1Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packSolarPanel2Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packSolarPanel3Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packSolarPanel4Temp(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsComms5vCurrent(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsComms3v3Current(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsMagnetorquer8vCurrent(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsAdcs5vCurrent(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsAdcs3v3Current(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsObc3v3Current(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsComms5vVoltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsComms3v3Voltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsMagnetorquer8vVoltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsAdcs5vVoltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsAdcs3v3Voltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsObc3v3Voltage(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packObcState(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packEpsState(telemetry_data_t *data, uint8_t *buffer, size_t *offset);
void packNumCspPacketsRcvd(telemetry_data_t *data, uint8_t *buffer, size_t *offset);

#endif /* CDH_INCLUDE_TELEMETRY_PACK_H_ */
