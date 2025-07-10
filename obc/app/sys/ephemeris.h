#pragma once

#include "obc_errors.h"
#include "obc_logging.h"
#include "sun_utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the sun position module. It is safe to call this function
 * multiple times.
 */
obc_error_code_t sunPositionInit(void);

/**
 * @brief Gets the sun position at the given julian date
 * @attention jd must be greater than 0
 * @attention buffer must be a valid pointer
 * @param jd The julian date (or close to it) to get the sun position at
 * @param buffer The buffer to store the sun position in
 */
obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer);

/**
 * @brief Returns the name of the file used for the sun position data
 */
const char *sunPositionGetFileName(void);

#ifdef __cplusplus
}
#endif
