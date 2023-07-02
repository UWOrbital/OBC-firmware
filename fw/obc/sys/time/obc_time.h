#pragma once

#include "obc_errors.h"
#include "ds3232_mz.h"

#include <stdint.h>

/**
 * @brief Initialize the time module.
 */
obc_error_code_t initTime(void);

/**
 * @brief Get the current unix time.
 *
 * @return uint32_t The current unix time.
 */
uint32_t getCurrentUnixTime(void);

/**
 * @brief Set the current unix time.
 *
 * @param unixTime The unix time to set.
 * @warning This function should not be called from an ISR.
 */
void setCurrentUnixTime(uint32_t unixTime);

/**
 * @brief Increment the current unix time by 1 second.
 * @warning This function should not be called from an ISR.
 */
void incrementCurrentUnixTime(void);

/**
 * @brief Sync the unix time with the RTC.
 *
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code.
 */
obc_error_code_t syncUnixTime(void);
