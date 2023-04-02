#include "obc_time.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_rw_lock.h"
#include "obc_assert.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

static unix_time_t currUnixTime;
static obc_rw_lock_t currUnixTimeLock;

STATIC_ASSERT(sizeof(unix_time_t) == sizeof(uint32_t), "sizeof(unix_time_t) != sizeof(uint32_t)");

void initTime(void) {
    memset(&currUnixTimeLock, 0, sizeof(currUnixTimeLock));
    memset(&currUnixTime, 0, sizeof(currUnixTime));

    rwLockInit(&currUnixTimeLock, 0);
}

unix_time_t getCurrentUnixTime(void) {
    return rwLockRead(&currUnixTimeLock);
}

void setCurrentUnixTime(unix_time_t unixTime) {
    rwLockWrite(&currUnixTimeLock, unixTime);
}

void incrementCurrentUnixTime(void) {
    // Since there's only 1 writer, there's no Read-Modify-Write problem
    unix_time_t unixTime = getCurrentUnixTime();
    unixTime++;
    setCurrentUnixTime(unixTime);
}

obc_error_code_t syncUnixTime(void) {
    obc_error_code_t errCode;

    rtc_date_time_t datetime;
    RETURN_IF_ERROR_CODE(getCurrentDateTimeRTC(&datetime));

    unix_time_t unixTime;
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

    uint32_t tmYday = datetime->date.date;
    uint32_t tmYear = datetime->date.year - 1900;

    // See https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html
    *unixTime = tmSec + tmMin*60 + tmHour*3600 + tmYday*86400 + 
        (tmYear-70)*31536000 + ((tmYear-69)/4)*86400 - 
        ((tmYear-1)/100)*86400 + ((tmYear+299)/400)*86400;

    return OBC_ERR_CODE_SUCCESS;
}
