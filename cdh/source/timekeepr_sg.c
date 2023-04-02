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
#include<stdlib.h>
#include "ds3232_mz.h"
#include "timekeeper_sg.h"
#include "time.h"

int8_t front = -1, rear = -1;
int8_t numOfActiveAlarms = 0;

obc_error_code_t setAlarm1(rtc_alarm_time_t alarmTime, rtc_alarm1_mode_t alarmMode) {
    return setAlarm1RTC(alarmMode, alarmTime);
}

obc_error_code_t setAlarm2(rtc_alarm_time_t alarmTime, rtc_alarm2_mode_t alarmMode) {
    return setAlarm2RTC(alarmMode, alarmTime);
}

obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime) {
    return setCurrentDateTimeRTC(&currentTime);
}

//can't be of the type obc_error_code_t else to get time we will have to do pass by pointer to actually get the time, BUT the function getCurrenTimeRTC() also has pass by pointer which is requiring me to pass
// a double pointer that leads to incompatible types, hence here error code can't be returned. Can't think of any other method
rtc_time_t getCurrentTime(rtc_time_t getTime) {
    getCurrentTimeRTC(&getTime);
    return getTime;
}

void addAlarm(rtc_alarm_time_t alarmTime) {
    /*
        Following Figure 5.6: Adding new alarms 
        https://ntnuopen.ntnu.no/ntnu-xmlui/bitstream/handle/11250/2413318/14533_FULLTEXT.pdf?sequence=1#page=54&zoom=100,94,101
    */
   enQueue(alarmTime);
   bubbleSort();
}

void exectureAlarm() {
    deQueue();
}

uint8_t isFull() {
    if((rear == front - 1) || (front == 0 && rear == taskQueueSize - 1))
        return 1;
    return 0;
}

uint8_t isEmpty() {
    if(front == -1)
        return 1;
    return 0;
}

obc_error_code_t enQueue(rtc_alarm_time_t alarmTime) {
    if(isFull())
        return OBC_ERR_CODE_UNKNOWN;
    else
    {
        if(front == -1)
            front = 0;
        rear = (rear + 1) % taskQueueSize;
        taskQueue[rear] = alarmTime;
        numOfActiveAlarms++;
    }
}

obc_error_code_t deQueue() {
    if(isEmpty())
        return OBC_ERR_CODE_UNKNOWN;
    else {
        if(front == rear) {
            taskQueue[front] = {0};
            front = -1;
            rear = -1;
            numOfActiveAlarms--;
        }
        else {
            taskQueue[front] = {0};
            front = (front + 1) % taskQueueSize;
            numOfActiveAlarms--;
        }
    }
}

void swap(rtc_alarm_time_t *a1, rtc_alarm_time_t *a2) {
    rtc_alarm_time_t temp = *a1;
    *a1 = *a2;
    *a2 = temp;
}
// look into difftime() maybe which makes use of time_t
// int compare(rtc_alarm_time_t a, rtc_alarm_time_t b) {
//     if(a.date > b.date)
//         return 1;
//     else if(a.date == b.date) {
//         if(a.time.hours > b.time.hours)
//             return 1;
//         else if(a.time.hours == b.time.hours) {
//             if(a.time.minutes > b.time.minutes)
//                 return 1;
//             else if(a.time.minutes == b.time.minutes) {
//                 if(a.time.seconds > b.time.seconds)
//                     return 1;
//                 else 
//                     return -1;
//             }
//             else    
//                 return -1;
//         }
//         else
//             return -1;
//     }
//     else
//         return -1;
// }

//https://www.geeksforgeeks.org/sort-m-elements-of-given-circular-array-starting-from-index-k/

void bubbleSort() {
    int8_t n = numOfActiveAlarms;
    int8_t numOfOuterIterations = 0;
    int8_t numOfInnerIterations = 0;
    for(int8_t i = 0; i < n; i++) {
        for(int8_t j = front; j < front + numOfActiveAlarms - 1; j++) {
            if(taskQueue[j % taskQueueSize] > taskQueue[(j + 1) % taskQueueSize]) {
                swap(&taskQueue[j % taskQueueSize], &taskQueue[(j + 1) % taskQueueSize]);
            }
        }
    }
}

