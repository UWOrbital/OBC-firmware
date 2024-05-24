#include "obc_time.h"
#include "obc_time_utils.h"
#include "obc_errors.h"
#include "logger.h"
#include "obc_assert.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_atomic.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Global Unix time
static volatile uint32_t currTime;

obc_error_code_t initTime(void) {
  obc_error_code_t errCode;

  memset((void *)&currTime, 0, sizeof(currTime));

  // Initialize the RTC

  // TODO: Replace hardcoded datetime with fetch from FRAM
  // or synch with ground station
  rtc_date_time_t rtcDateTime = {0};
  rtcDateTime.date.year = 23;  // 2023
  rtcDateTime.date.month = 1;
  rtcDateTime.date.date = 1;
  rtcDateTime.time.hours = 0;
  rtcDateTime.time.minutes = 0;
  rtcDateTime.time.seconds = 0;

  // Set current date and time
  RETURN_IF_ERROR_CODE(rtcInit(&rtcDateTime));

  // Enable alarm 1 interrupt
  rtc_control_t rtcControl = {0};
  RETURN_IF_ERROR_CODE(getControlRTC(&rtcControl));
  rtcControl.A1IE = 1;
  rtcControl.INTCN = 1;
  RETURN_IF_ERROR_CODE(setControlRTC(&rtcControl));

  // Synch the local Unix time with the RTC
  RETURN_IF_ERROR_CODE(syncUnixTime());

  return OBC_ERR_CODE_SUCCESS;
}

uint32_t getCurrentUnixTime(void) {
  uint32_t time;

  vPortEnterCritical();
  time = currTime;
  vPortExitCritical();

  return time;
}

void setCurrentUnixTime(uint32_t unixTime) {
  vPortEnterCritical();
  currTime = unixTime;
  vPortExitCritical();
}

void incrementCurrentUnixTime(void) { Atomic_Increment_u32(&currTime); }

obc_error_code_t syncUnixTime(void) {
  obc_error_code_t errCode;

  rtc_date_time_t datetime = {0};

  RETURN_IF_ERROR_CODE(getCurrentDateTimeRTC(&datetime));

  uint32_t unixTime;
  RETURN_IF_ERROR_CODE(datetimeToUnix(&datetime, &unixTime));

  setCurrentUnixTime(unixTime);

  return OBC_ERR_CODE_SUCCESS;
}
