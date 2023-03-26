#ifndef COMMON_INCLUDE_OBC_TIME_H_
#define COMMON_INCLUDE_OBC_TIME_H_

#include "obc_errors.h"
#include "ds3232_mz.h"

#include <stdint.h>

typedef uint32_t unix_time_t;

/**
 * @brief Get the current unix time.
 * 
 * @return unix_time_t The current unix time.
 */
unix_time_t getCurrentUnixTime(void);

/**
 * @brief Set the current unix time.
 * 
 * @param unixTime The unix time to set.
 */
void setCurrentUnixTime(unix_time_t unixTime);

/**
 * @brief Increment the current unix time by 1 second.
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
obc_error_code_t datetimeToUnix(rtc_date_time_t *datetime, unix_time_t *unixTime);

/**
 * @brief Fetch the current time from the RTC and convert it to unix time.
 * 
 * @param unixTime Buffer to store the unix time.
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code.
 */
obc_error_code_t fetchCurrentTimeUnix(unix_time_t *unixTime);


#endif /* COMMON_INCLUDE_OBC_TIME_H_ */
