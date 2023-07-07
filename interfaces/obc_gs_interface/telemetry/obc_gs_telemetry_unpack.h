#pragma once

#include "obc_gs_errors.h"
#include "obc_gs_telemetry_data.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Unpack telemetry data from a buffer of bytes
 *
 * @param buffer The buffer to unpack from
 * @param offset The offset into the buffer to unpack from
 * @param data The telemetry data to unpack into
 */
obc_gs_error_code_t unpackTelemetry(const uint8_t *buffer, uint32_t *offset, telemetry_data_t *data);

#ifdef __cplusplus
}
#endif
