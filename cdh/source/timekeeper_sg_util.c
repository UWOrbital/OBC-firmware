#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <stdlib.h>
#include "ds3232_mz.h"
#include "timekeeper_sg_util.h"

int8_t front = -1, rear = -1;
int8_t numOfActiveAlarms = 0;
timekeeper_sg_rtc_alarm alarmQueue[ALARM_QUEUE_SIZE];
timekeeper_sg_rtc_alarm dummyAlarm = {.mode.alarm1Mode = RTC_ALARM1_ONCE_PER_SECOND,
                                      .alarmVal = {1, {1, 1, 1}},
                                    };

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

void addAlarm(timekeeper_sg_rtc_alarm alarm) {
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

uint8_t isFull() {
    if((rear == front - 1) || (front == 0 && rear == ALARM_QUEUE_SIZE - 1))
        return 1;
    return 0;
}

uint8_t isEmpty() {
    if(front == -1)
        return 1;
    return 0;
}

obc_error_code_t enQueue(timekeeper_sg_rtc_alarm alarm) {
    if(isFull())
        return OBC_ERR_CODE_UNKNOWN;
    else
    {
        if(front == -1)
            front = 0;
        rear = (rear + 1) % ALARM_QUEUE_SIZE;
        alarmQueue[rear] = alarm;
        numOfActiveAlarms++;
    }
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t deQueue() {
    if(isEmpty())
        return OBC_ERR_CODE_UNKNOWN;
    else {
        if(front == rear) {
            alarmQueue[front] = dummyAlarm;
            front = -1;
            rear = -1;
            numOfActiveAlarms--;
        }
        else {
            alarmQueue[front] = dummyAlarm;
            front = (front + 1) % ALARM_QUEUE_SIZE;
            numOfActiveAlarms--;
        }
    }
    return OBC_ERR_CODE_SUCCESS;
}

void swap(rtc_alarm_time_t *a1, rtc_alarm_time_t *a2) {
    rtc_alarm_time_t temp = *a1;
    *a1 = *a2;
    *a2 = temp;
}

//https://www.geeksforgeeks.org/sort-m-elements-of-given-circular-array-starting-from-index-k/
void bubbleSort() {
    int8_t n = numOfActiveAlarms;
    for(int8_t i = 0; i < n; i++) {
        for(int8_t j = front; j < front + numOfActiveAlarms - 1; j++) {
            // waiting on daniel's unix time PR to get merged to fix expression must have arithmetic pointer error below
            /*if(alarmQueue[j % ALARM_QUEUE_SIZE] > alarmQueue[(j + 1) % ALARM_QUEUE_SIZE]) {
                swap(&alarmQueue[j % ALARM_QUEUE_SIZE], &alarmQueue[(j + 1) % ALARM_QUEUE_SIZE]);
            }*/
        }
    }
}