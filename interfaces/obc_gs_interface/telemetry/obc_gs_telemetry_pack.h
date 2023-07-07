#pragma once

#include "obc_gs_errors.h"
#include "obc_gs_telemetry_data.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pack telemetry data into a buffer of bytes
 *
 * @param data The telemetry data to pack
 * @param buffer The buffer to pack the data into
 * @param buffLen The length of the buffer
 * @param numPacked The number of bytes packed into the buffer
 */
obc_gs_error_code_t packTelemetry(const telemetry_data_t *data, uint8_t *buffer, size_t buffLen, uint32_t *numPacked);

#ifdef __cplusplus
}
#endif
