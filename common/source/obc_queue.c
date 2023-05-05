#include <stdlib.h>
#include "ds3232_mz.h"
#include "obc_queue.h"
#include "rtc_alarm_handler_util.h"

int8_t front = -1, rear = -1;
int8_t numOfActiveAlarms = 0;
rtc_alarm_handler_rtc_alarm alarmQueue[ALARM_QUEUE_SIZE];
rtc_alarm_handler_rtc_alarm dummyAlarm = {.mode.alarm1Mode = RTC_ALARM1_ONCE_PER_SECOND,
                                      .alarmVal = {1, {1, 1, 1}},
                                    };

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

obc_error_code_t enQueue(rtc_alarm_handler_rtc_alarm alarm) {
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