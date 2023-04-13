#ifndef CDH_INCLUDE_TIMEKEEPER_SG_UTIL_H_
#define CDH_INCLUDE_TIMEKEEPER_SG_UTIL_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"

/**
 * @union	timekeeper_sg_alarm_mode_t
 * @brief	union of alarm modes for alarm 1 and 2 on RTC.
*/
typedef union {
    rtc_alarm1_mode_t alarm1Mode;
    rtc_alarm2_mode_t alarm2Mode;
} timekeeper_sg_alarm_mode_t;

/**
 * @struct	timekeeper_sg_rtc_alarm
 * @brief	atruct for RTC alarm containing both mode and alarm value
 */
typedef struct {
    timekeeper_sg_alarm_mode_t mode;
    rtc_alarm_time_t alarmVal;
} timekeeper_sg_rtc_alarm;

// timekeeper_sg_rtc_alarm dummyAlarm;

#define ALARM_QUEUE_SIZE 10U
// timekeeper_sg_rtc_alarm alarmQueue[];

//test
obc_error_code_t setAlarm1(rtc_alarm_time_t alarmTime, rtc_alarm1_mode_t alarmMode);

obc_error_code_t setAlarm2(rtc_alarm_time_t alarmTime, rtc_alarm2_mode_t alarmMode);

obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime);

rtc_time_t getCurrentTime(rtc_time_t getTime);

void addAlarm(timekeeper_sg_rtc_alarm alarm);

void exectureAlarm();

uint8_t isFull();

uint8_t isEmpty();

obc_error_code_t enQueue(timekeeper_sg_rtc_alarm alarm);

obc_error_code_t deQueue();

void swap(rtc_alarm_time_t *a1, rtc_alarm_time_t *a2);

void bubbleSort();

#endif /*CDH_INCLUDE_TIMEKEEPER_SG_UTIL_H_*/