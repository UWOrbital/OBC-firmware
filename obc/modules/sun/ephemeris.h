#pragma once

#include "obc_errors.h"
#include "obc_logging.h"
#include "sun_utils.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the sun position module. This should only be called once
 */
obc_error_code_t initSunPosition(void);

/**
 * @brief Gets the sun position at the given julian date
 * @attention jd must be greater than 0
 * @attention buffer must be a valid pointer
 * @param jd The julian date (or close to it) to get the sun position at
 * @param buffer The buffer to store the sun position in
 *
 * @details May modify the file used for storing the data points
 */
obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer);

#ifdef __cplusplus
}
#endif
