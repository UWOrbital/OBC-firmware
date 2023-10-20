#include "test_rtc.h"
#include "obc_print.h"
#include "ds3232_mz.h"

void testRTC(void) {
  sciPrintf("Testing RTC...\r\n");

  // Hardcoded date and time for testing
  rtc_date_time_t rtcDateTime = {0};
  rtcDateTime.date.year = 23;
  rtcDateTime.date.month = 1;
  rtcDateTime.date.date = 1;
  rtcDateTime.time.hours = 0;
  rtcDateTime.time.minutes = 0;
  rtcDateTime.time.seconds = 0;

  rtc_date_time_t currentDateTime = {0};
  rtc_control_t control = {0};
  float temperature;

  // Initialize RTC
  if (rctInit(&rtcDateTime) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to initialize RTC\r\n");
    return;
  }

  /*---------INITIAL GET TESTS---------*/

  // DateTime read
  if (getCurrentDateTimeRTC(&currentDateTime) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to get RTC DateTime\r\n");
  }

  // Temperature read
  if (getTemperatureRTC(&temperature) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to get RTC temperature\r\n");
  } else {
    sciPrintf("RTC Temperature: %f\r\n", temperature);
  }

  // Control register configuration read
  if (getControlRTC(&control) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to get RTC control\r\n");
  } else {
    sciPrintf("Initial RTC Control: EOSC-%u BBSQW-%u CONV-%u A1IE-%u A2IE-%u INTCN-%u\r\n", control.EOSC, control.BBSQW,
              control.CONV, control.A1IE, control.A2IE, control.INTCN);
  }

  /*---------SET TESTS---------*/

  // DateTime set
  currentDateTime.date.year += 1;
  currentDateTime.date.month += 1;
  currentDateTime.date.date += 1;
  currentDateTime.time.hours += 1;
  currentDateTime.time.minutes += 30;
  currentDateTime.time.seconds += 30;
  if (setCurrentDateTimeRTC(&currentDateTime) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to set RTC DateTime\r\n");
  } else {
    // Verify that the values were updated
    rtc_date_time_t updatedDateTime = {0};
    if (getCurrentDateTimeRTC(&updatedDateTime) != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Failed to get updated RTC DateTime\r\n");
    } else if (updatedDateTime.date.year == currentDateTime.date.year &&
               updatedDateTime.date.month == currentDateTime.date.month &&
               updatedDateTime.date.date == currentDateTime.date.date &&
               updatedDateTime.time.hours == currentDateTime.time.hours &&
               updatedDateTime.time.minutes == currentDateTime.time.minutes &&
               updatedDateTime.time.seconds == currentDateTime.time.seconds) {
      sciPrintf("Successfully set RTC DateTime\r\n");
    } else {
      sciPrintf("RTC DateTime does not match expected\r\n");
    }
  }

  // Alarm & interrupt register sets
  control.A1IE = 1;
  control.A2IE = 1;
  control.INTCN = 1;
  if (setControlRTC(&control) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to set RTC control\r\n");
  }

  rtc_alarm_time_t alarm = {0};

  // Alarm1 set to pulse whenever seconds == 30
  rtc_alarm1_mode_t alarm1Mode = RTC_ALARM1_MATCH_SECONDS;
  alarm.time.seconds = 30;
  alarm.time.minutes = 0;
  alarm.time.hours = 0;
  alarm.date = 1;
  if (setAlarm1RTC(alarm1Mode, alarm) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to set RTC alarm 1\r\n");
  } else {
    sciPrintf("Successfully set RTC alarm 1\r\n");
  }

  // Alarm2 set to pulse whenever minutes == 15
  rtc_alarm2_mode_t alarm2Mode = RTC_ALARM2_MATCH_MINUTES;
  alarm.time.seconds = 0;
  alarm.time.minutes = 15;
  alarm.time.hours = 0;
  alarm.date = 1;
  if (setAlarm2RTC(alarm2Mode, alarm) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to set RTC alarm 2\r\n");
  } else {
    sciPrintf("Successfully set RTC alarm 2\r\n");
  }
}
