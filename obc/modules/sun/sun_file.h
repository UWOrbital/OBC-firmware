// This file is included by ephermeris.h and should NOT be included directly, unless for testing purposes
// All functions are NOT atomic. It is the responsibility of the caller to setup the appropriate locks
#pragma once

#include "sun_utils.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@breif Initializes the sun file module with the fileName, also serves as a reset if the file is modified
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileInit(const char *fileName);

/**
 * @brief Gets the min JD stored in the sun file
 * @attention Requires that jd is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileGetMinJD(julian_date_t *jd);

/**
 * @brief Gets the max JD stored in the sun file
 * @attention Requires that jd is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileGetMaxJD(julian_date_t *jd);

/**
 * @brief Checks if the JD is within range and stores it in the buffer if valdi
 * @attention Requires that buff is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileJDInRange(julian_date_t jd, bool *buff);

/**
 * @brief Reads the data point from the file at the index into the buff
 * @attention Requires that jd is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileReadDataPoint(uint32_t index, position_data_t *buff);

/**
 * @brief Gets the number of data points from the file
 * @attention Requires that number is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileGetNumDataPoints(uint32_t *number);

/**
 * @brief Gets the index of the jd in the file and stores it into the index buffer
 * @attention Requires that index is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileGetIndexOfJD(julian_date_t jd, uint32_t *index);

/**
 * @brief Gets the number of data points after the JD and stores it into number
 * @attention Requires that number is a valid pointer
 * @warning This operation is NOT atomic. It is the responsibility of the caller to setup the appropriate locks
 */
obc_error_code_t sunFileGetNumDataPointsAfterJD(julian_date_t jd, uint32_t *number);

#ifdef __cplusplus
}
#endif
