/// first create a Queue using freeRTOS provided functions xqueuecreate, sample length of 20
///  

/// 1. Set Alarm : Direct function call in the driver
/// 2. Handle different alarms:
/// 3 and 4 ask daniel
/// 5. Set current time: Direct function call in driver
/// 6. Get current time: Direct function call in driver
/// 7. Assign a mutex to each function
/// 8. Reset function in RTC driver can be used to reset all current times
/// 9. Feature not developed yet in the driver due to RTC limitations

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include "ds3232_mz.h"
#include "timekeeper_sg.h"

obc_error_code_t setAlarm1(rtc_alarm_time_t alarmTime, rtc_alarm1_mode_t alarmMode) {
    return setAlarm1RTC(alarmMode, alarmTime);
}

obc_error_code_t setAlarm2(rtc_alarm_time_t alarmTime, rtc_alarm2_mode_t alarmMode) {
    return setAlarm2RTC(alarmMode, alarmTime);
}

obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime) {
    return setCurrentDateTimeRTC(&currentTime);
}

//can't be of the type obc_Err else to get time we will have to do pass by pointer to actually get the time, BUT the function getCurrenTimeRTC() also has pass by pointer which is requiring me to pass
// a double pointer that leads to incompatible types, hence here error code can't be returned. Can't think of any other method
rtc_time_t getCurrentTime(rtc_time_t getTime) {
    getCurrentTimeRTC(&getTime);
    return getTime;
}


