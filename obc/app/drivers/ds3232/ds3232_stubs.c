#ifndef CONFIG_DS3232
#include "obc_errors.h"
#include "obc_general_util.h"
#include "ds3232_mz.h"

obc_error_code_t rtcInit(rtc_date_time_t* dateTime) {
  UNUSED(dateTime);
  return OBC_ERR_CODE_SUCCESS;
}

void turnOnRTC(void) {}

void resetRTC(void) {}

obc_error_code_t getSecondsRTC(uint8_t* seconds) {
  UNUSED(seconds);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getMinutesRTC(uint8_t* minutes) {
  UNUSED(minutes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getHoursRTC(uint8_t* hours) {
  UNUSED(hours);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getDayRTC(uint8_t* day) {
  UNUSED(day);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getDateRTC(uint8_t* date) {
  UNUSED(date);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getMonthRTC(uint8_t* month) {
  UNUSED(month);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getYearRTC(uint8_t* year) {
  UNUSED(year);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getCurrentDateTimeRTC(rtc_date_time_t* dateTime) {
  UNUSED(dateTime);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getCurrentTimeRTC(rtc_time_t* time) {
  UNUSED(time);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getAlarmTimeRTC(rtc_alarm_time_t* alarmTime) {
  UNUSED(alarmTime);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getControlRTC(rtc_control_t* control) {
  UNUSED(control);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getStatusRTC(rtc_status_t* status) {
  UNUSED(status);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getAgingOffsetRTC(int8_t* agingOffset) {
  UNUSED(agingOffset);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getTemperatureRTC(float* temperature) {
  UNUSED(temperature);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setSecondsRTC(uint8_t writeSeconds) {
  UNUSED(writeSeconds);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setMinutesRTC(uint8_t writeMinutes) {
  UNUSED(writeMinutes);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setHourRTC(uint8_t writeHour) {
  UNUSED(writeHour);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setDayRTC(uint8_t writeDays) {
  UNUSED(writeDays);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setDateRTC(uint8_t writeDates) {
  UNUSED(writeDates);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setMonthRTC(uint8_t writeMonths) {
  UNUSED(writeMonths);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setYearRTC(uint8_t writeYears) {
  UNUSED(writeYears);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setCurrentDateTimeRTC(rtc_date_time_t* writeDateTime) {
  UNUSED(writeDateTime);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setControlRTC(rtc_control_t* writeControl) {
  UNUSED(writeControl);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setStatusRTC(rtc_status_t* writeStatus) {
  UNUSED(writeStatus);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setAgingOffsetRTC(int8_t writeAgingOffset) {
  UNUSED(writeAgingOffset);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setAlarm1RTC(rtc_alarm1_mode_t mode, rtc_alarm_time_t dt) {
  UNUSED(mode);
  UNUSED(dt);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setAlarm2RTC(rtc_alarm2_mode_t mode, rtc_alarm_time_t dt) {
  UNUSED(mode);
  UNUSED(dt);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t clearAlarm1RTC(void) { return OBC_ERR_CODE_SUCCESS; }

#endif  // CONFIG_DS3232
