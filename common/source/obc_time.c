#include "obc_time.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_assert.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/**
 * @brief Checks if a given year is a leap year.
 * 
 * @param year Year to check
 * @return true if the year is a leap year, false otherwise
 */
static bool isLeapYear(uint16_t year);

/**
 * @brief Calculates the day of the year given a month, day, and year.
 * 
 * @param month The month (1-12)
 * @param day The day (1-31)
 * @param year The year
 * @return uint16_t 
 */
static uint16_t calcDayOfYear(uint8_t month, uint8_t day, uint16_t year);

// Global Unix time
static uint32_t currTime;

void initTime(void) {
    memset(&currTime, 0, sizeof(currTime));
    syncUnixTime();
}

uint32_t getCurrentUnixTime(void) {
    return currTime;
}

void setCurrentUnixTime(uint32_t unixTime) {
    taskENTER_CRITICAL();
    currTime = unixTime;
    taskEXIT_CRITICAL();
}

void incrementCurrentUnixTime(void) {
    taskENTER_CRITICAL();
    currTime++;
    taskEXIT_CRITICAL();
}

obc_error_code_t syncUnixTime(void) {
    obc_error_code_t errCode;

    rtc_date_time_t datetime = {
        .date = {
            .date = 1,
            .month = 4,
            .year = 23
        },
        .time = {
            .hours = 12,
            .minutes = 30,
            .seconds = 30
        }
    };

    // TODO: Uncomment this once the I2C infinite loop bug is fixed
    // For now, always sync to the same date/time
    // RETURN_IF_ERROR_CODE(getCurrentDateTimeRTC(&datetime));

    uint32_t unixTime;
    RETURN_IF_ERROR_CODE(datetimeToUnix(&datetime, &unixTime));

    setCurrentUnixTime(unixTime);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t datetimeToUnix(rtc_date_time_t *datetime, uint32_t *unixTime) {
    if (datetime == NULL || unixTime == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    uint32_t tmSec = datetime->time.seconds;
    uint32_t tmMin = datetime->time.minutes;
    uint32_t tmHour = datetime->time.hours;

    uint32_t tmYday = (uint32_t)calcDayOfYear(datetime->date.month, datetime->date.date, RTC_YEAR_OFFSET + datetime->date.year);
    
    // Get year since 1900
    uint32_t tmYear = (RTC_YEAR_OFFSET + datetime->date.year) - 1900;

    // See https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html
    *unixTime = tmSec + tmMin*60 + tmHour*3600 + tmYday*86400 + 
        (tmYear-70)*31536000 + ((tmYear-69)/4)*86400 - 
        ((tmYear-1)/100)*86400 + ((tmYear+299)/400)*86400;

    return OBC_ERR_CODE_SUCCESS;
}

/* 2000-03-01 (mod 400 year, immediately after Feb 29 */
#define LEAPOCH (946684800LL + 86400 * (31 + 29))

// To be a leap year, the year number must be divisible by four 
// except for end-of-century years, which must be divisible by 400.
#define DAYS_PER_400Y   (365 * 400 + 97)
#define DAYS_PER_100Y   (365 * 100 + 24)
#define DAYS_PER_4Y     (365 * 4   + 1)

#define SECS_PER_MIN    60
#define SECS_PER_HOUR   (SECS_PER_MIN * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)

obc_error_code_t unixToDatetime(uint32_t ts, rtc_date_time_t *dt) {
    /*
        This function is based on the implementation of __secs_to_tm in the
        musl C library. The original implementation can be found here:
        https://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c
    */

    // TODO: Try to clean this up a bit (I don't even fully understand it)

    if (dt == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Since LEAPOCH starts in March, the first month is March
    static const char days_in_month[] = {31,30,31,30,31,31,30,31,30,31,31,29};

    uint32_t years, months, days, secs;
    uint32_t remDays, remSecs, remYears;
    uint32_t qcCycles, cCycles, qCycles;

    if (ts < LEAPOCH) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    secs = ts - LEAPOCH;
    days = secs / SECS_PER_DAY;
    remSecs = secs % SECS_PER_DAY;

    qcCycles = days / DAYS_PER_400Y;
    remDays = days % DAYS_PER_400Y;

    cCycles = remDays / DAYS_PER_100Y;
    if (cCycles == 4)  {
        cCycles--;
    }
    remDays -= cCycles * DAYS_PER_100Y;

    qCycles = remDays / DAYS_PER_4Y;
    if (qCycles == 25) {
        qCycles--;
    }
    remDays -= qCycles * DAYS_PER_4Y;

    remYears = remDays / 365;
    if (remYears == 4) {
        remYears--;
    }
    remDays -= remYears * 365;

    years = remYears + 4*qCycles + 100*cCycles + 400*qcCycles;

    for (months=0; days_in_month[months] <= remDays; months++) {
        remDays -= days_in_month[months];
    }

    dt->date.year = years; // RTC expects 0-99 so we don't need to offset
    
    // Shift required since LEAPOCH starts in March
    // Convert to 1-indexed from 0-indexed
    dt->date.month = (months + 2) + 1;

    if (dt->date.month >= 12) {
        dt->date.month -=12;
        dt->date.year++;
    }
    dt->date.date = remDays + 1; // Day of month

    dt->time.hours = remSecs / 3600;
    dt->time.minutes = (remSecs / 60) % 60;
    dt->time.seconds = remSecs % 60;

    return OBC_ERR_CODE_SUCCESS;
}

static bool isLeapYear(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static uint16_t calcDayOfYear(uint8_t month, uint8_t day, uint16_t year) {
    // Cumulative days in a year up to the start of each month
    static const uint16_t days[2][12] = {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
    };

    // Row 0 is for non-leap years, row 1 is for leap years
    uint8_t leap = (uint8_t)isLeapYear(year);

    // Convert 1-indexed month to 0-indexed
    month--;
    day--;

    return days[leap][month] + day;
}
