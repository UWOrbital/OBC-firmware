#ifndef COMMON_INCLUDE_OBC_TIME_H_
#define COMMON_INCLUDE_OBC_TIME_H_

#include "obc_errors.h"
#include "ds3232_mz.h"

#include <stdint.h>

/**
 * @brief Initialize the time module.
 */
void initTime(void);

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

/**
 * @brief Convert a datetime to unix time.
 * 
 * @param datetime The datetime to convert.
 * @param unixTime Buffer to store the unix time.
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code.
 */
obc_error_code_t datetimeToUnix(rtc_date_time_t *datetime, uint32_t *unixTime);

/**
 * @brief Convert unix time to a datetime.
 * 
 * @param unixTime The unix time to convert.
 * @param datetime Buffer to store the datetime.
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code.
 */
obc_error_code_t unixToDatetime(uint32_t unixTime, rtc_date_time_t *datetime);

#endif /* COMMON_INCLUDE_OBC_TIME_H_ */
