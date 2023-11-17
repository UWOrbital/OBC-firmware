#include "test_rtc.h"
#include "obc_print.h"
#include "ds3232_mz.h"
#include "gio.h"

#define TEMP_LOWER_BOUND 0.0f
#define TEMP_UPPER_BOUND 40.0f

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
  if (rtcInit(&rtcDateTime) != OBC_ERR_CODE_SUCCESS) {
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
  } else if (temperature < TEMP_LOWER_BOUND || TEMP_UPPER_BOUND < temperature) {
    sciPrintf("RTC Temperature Test Failed: %f°C\r\n\tExpected: [%f°C, %f°C]\r\n", temperature, TEMP_LOWER_BOUND,
              TEMP_UPPER_BOUND);

  } else {
    sciPrintf("RTC Temperature Test Passed: %f\r\n", temperature);
  }

  // Control register configuration read
  if (getControlRTC(&control) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to get RTC control\r\n");
  } else if (control.EOSC == 0 && control.BBSQW == 0 && control.CONV == 0 && control.A1IE == 0 && control.A2IE == 0 &&
             control.INTCN == 1) {
    sciPrintf("Initial RTC Control Test Passed: EOSC-%u BBSQW-%u CONV-%u A1IE-%u A2IE-%u INTCN-%u\r\n", control.EOSC,
              control.BBSQW, control.CONV, control.A1IE, control.A2IE, control.INTCN);
  } else {
    sciPrintf(
        "Initial RTC Control Test Failed: EOSC-%u BBSQW-%u CONV-%u A1IE-%u A2IE-%u INTCN-%u\r\n\tExpected: EOSC-%u "
        "BBSQW-%u CONV-%u A1IE-%u A2IE-%u INTCN-%u",
        control.EOSC, control.BBSQW, control.CONV, control.A1IE, control.A2IE, control.INTCN, 0, 0, 0, 0, 0, 1);
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
               // Two second tolerance to account for delay between writing and reading
               0 <= updatedDateTime.time.seconds - currentDateTime.time.seconds &&
               updatedDateTime.time.seconds - currentDateTime.time.seconds <= 2) {
      sciPrintf("Successfully set RTC DateTime\r\n");
    } else {
      sciPrintf("RTC DateTime does not match expected\r\n");
    }
  }

  /*---------ALARM TESTS---------*/

  // Alarm & interrupt register sets
  control.A1IE = 1;
  control.INTCN = 1;
  if (setControlRTC(&control) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to set RTC control\r\n");
  }

  rtc_alarm_time_t alarm = {0};

  // Alarm1 set to trigger whenever seconds == 30
  rtc_alarm1_mode_t alarm1Mode = RTC_ALARM1_MATCH_SECONDS;
  alarm.time.seconds = 30;
  alarm.time.minutes = 0;
  alarm.time.hours = 0;
  alarm.date = 1;
  if (setAlarm1RTC(alarm1Mode, alarm) != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to set RTC alarm 1\r\n");
  } else {
    sciPrintf("Successfully set RTC alarm 1\r\nWaiting for alarm interrupt...\r\n");
  }

  // Poll for alarm interrupt which should set GIOA_0 HIGH
  uint32_t elapsed = 0;
  bool failed = false;
  while (gioGetBit(gioPORTA, 0) == 0) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    elapsed++;

    // Since seconds will match 30 once a minute, let this poll for max 1 minute before assuming failure
    if (elapsed > 60) {
      failed = true;
      sciPrintf("Alarm Interrupt Failed: Alarm was not triggered within a minute");
      break;
    }
  }

  if (!failed) {
    // Verify the alarm flag was set at the right time (matching 30 seconds, with some error)
    if (getCurrentTimeRTC(&currentDateTime.time) != OBC_ERR_CODE_SUCCESS) {
      if (0 <= (currentDateTime.time.seconds - 30) && (currentDateTime.time.seconds - 30) <= 2) {
        sciPrintf("Alarm 1 interrupt triggered succesfully\r\n");
      } else {
        sciPrintf("Alarm 1 flag interrupt triggered at wrong time: %d seconds\r\n\tExpected: 30 seconds",
                  currentDateTime.time.seconds);
      }
    } else {
      sciPrintf("Failed to get RTC DateTime\r\n");
    }
  }

  if (clearAlarm1RTC() != OBC_ERR_CODE_SUCCESS) {
    sciPrintf("Failed to clear RTC alarm 1 flag\r\n");
  } else {
    sciPrintf("RTC alarm 1 flag cleared succesfully\r\n");
  }
}
