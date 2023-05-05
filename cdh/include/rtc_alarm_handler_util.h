#ifndef CDH_INCLUDE_TIMEKEEPER_SG_UTIL_H_
#define CDH_INCLUDE_TIMEKEEPER_SG_UTIL_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"

/**
 * @union	rtc_alarm_handler_alarm_mode_t
 * @brief	union of alarm modes for alarm 1 and 2 on RTC.
*/
typedef union {
    rtc_alarm1_mode_t alarm1Mode;
    rtc_alarm2_mode_t alarm2Mode;
} rtc_alarm_handler_alarm_mode_t;

/**
 * @struct	rtc_alarm_handler_rtc_alarm
 * @brief	atruct for RTC alarm containing both mode and alarm value
 */
typedef struct {
    rtc_alarm_handler_alarm_mode_t mode;
    rtc_alarm_time_t alarmVal;
} rtc_alarm_handler_rtc_alarm;


/**
 * @brief	Set alarm 1 on RTC
 * @param	alarmTime	Alarm time to be set
 * @param   alarmMode   Alarm mode options for alarm 1
 * @return  The error code
 */
obc_error_code_t setAlarm1(rtc_alarm_time_t alarmTime, rtc_alarm1_mode_t alarmMode);

/**
 * @brief	Set alarm 2 on RTC
 * @param	alarmTime	Alarm time to be set
 * @param   alarmMode   Alarm mode options for alarm 2
 * @return  The error code
 */
obc_error_code_t setAlarm2(rtc_alarm_time_t alarmTime, rtc_alarm2_mode_t alarmMode);

/**
 * @brief	Set date and time on RTC
 * @param	currentTime	  Time to be set
 * @return  The error code
 */
obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime);

/**
 * @brief	Get current time on RTC
 * @param	getTime   stores current time on RTC
 * @return  the time
 */
rtc_time_t getCurrentTime(rtc_time_t getTime);

/**
 * @brief	add alarm to queue of alarms and then re-sort the queue
 * @param	alarm   alarm to be added
 */
void addAlarm(rtc_alarm_handler_rtc_alarm alarm);

/**
 * @brief	executes closest alarm in the queue
 */
void exectureAlarm();

#endif /*CDH_INCLUDE_TIMEKEEPER_SG_UTIL_H_*/