#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <stdlib.h>
#include "ds3232_mz.h"
#include "rtc_alarm_handler_util.h"
#include "obc_queue.h"

obc_error_code_t setAlarm1(rtc_alarm_time_t alarmTime, rtc_alarm1_mode_t alarmMode) {
    return setAlarm1RTC(alarmMode, alarmTime);
}

obc_error_code_t setAlarm2(rtc_alarm_time_t alarmTime, rtc_alarm2_mode_t alarmMode) {
    return setAlarm2RTC(alarmMode, alarmTime);
}

obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime) {
    return setCurrentDateTimeRTC(&currentTime);
}

rtc_time_t getCurrentTime(rtc_time_t getTime) {
    getCurrentTimeRTC(&getTime);
    return getTime;
}

void addAlarm(rtc_alarm_handler_rtc_alarm alarm) {
    /*
        Following Figure 5.6: Adding new alarms 
        https://ntnuopen.ntnu.no/ntnu-xmlui/bitstream/handle/11250/2413318/14533_FULLTEXT.pdf?sequence=1#page=54&zoom=100,94,101
    */
   enQueue(alarm);
   bubbleSort();
}

void exectureAlarm() {

    // TBD what to do with this, local interrupt or use RTC alarm, for now just dequeing from alarm queue
    deQueue();
}