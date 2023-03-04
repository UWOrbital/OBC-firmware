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

obc_error_code_t setCurrentTime(rtc_time_t currentTime) {
    return setCurrentTimeRTC(currentTime);
}

obc_error_code_t getCurrentTime(rtc_time_t *getTime) {
    return getCurrentTimeRTC(&getTime);
}


