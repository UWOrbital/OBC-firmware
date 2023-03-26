#include "obc_time.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <stdint.h>
#include <stddef.h>

static unix_time_t currUnixTime;

obc_error_code_t syncUnixTime(void) {
    obc_error_code_t errCode;

    unix_time_t unixTime;
    RETURN_IF_ERROR_CODE(fetchCurrentTimeUnix(&unixTime));

    setCurrentUnixTime(unixTime);
    return OBC_ERR_CODE_SUCCESS;
}

unix_time_t getCurrentUnixTime(void) {
    // TODO: Implement sequence lock
    return currUnixTime;
}

// All writes to the global time should be done from the timekeeper task
// to prevent conflicts due to concurrent writes.
void setCurrentUnixTime(unix_time_t unixTime) {
    // TODO: Implement sequence lock
    currUnixTime = unixTime;
}

void incrementCurrentUnixTime(void) {
    // TODO: Implement sequence lock
    currUnixTime++;
}

obc_error_code_t fetchCurrentTimeUnix(unix_time_t *unixTime) {
    obc_error_code_t errCode;

    if (unixTime == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    rtc_date_time_t datetime;
    RETURN_IF_ERROR_CODE(getCurrentDateTimeRTC(&datetime));

    RETURN_IF_ERROR_CODE(datetimeToUnix(&datetime, unixTime));
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

    *unixTime = tmSec + tmMin*60 + tmHour*3600 + tmYday*86400 + 
        (tmYear-70)*31536000 + ((tmYear-69)/4)*86400 - 
        ((tmYear-1)/100)*86400 + ((tmYear+299)/400)*86400;

    return OBC_ERR_CODE_SUCCESS;
}
