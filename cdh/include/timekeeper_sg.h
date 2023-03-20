#ifndef CDH_INCLUDE_TIMEKEEPER_SG_H_
#define CDH_INCLUDE_TIMEKEEPER_SG_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"

rtc_alarm_time_t queue[10];   // dummy size for now, get better clarity on queuing

obc_error_code_t setAlarm1(rtc_alarm_time_t alarmTime, rtc_alarm1_mode_t alarmMode);

obc_error_code_t setAlarm2(rtc_alarm_time_t alarmTime, rtc_alarm2_mode_t alarmMode);

/*An idea is to also have a generic alarm function which can take any mode from alarm 1 or alarm 2. A seperate enum will be made for that in this file which combines
both the alarm enums from the rtc driver, based on the mode the function will choose which alarm to set and if a mode is available in both alarms maybe choose based on
which alarm is available? If we go with this, then setalar1 and setAlarm2 functions should not be declared in the header file as it is not part of the interface we
want the other seubteams to see*/

obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime);

rtc_time_t getCurrentTime(rtc_time_t getTime);

void alarmQueue(rtc_alarm_time_t alarmTime);

int compare(rtc_alarm_time_t a, rtc_alarm_time_t b);

#endif /*CDH_INCLUDE_TIMEKEEPER_SG_H_*/