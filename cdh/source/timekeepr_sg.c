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
#include "obc_task_config.h"

int8_t front = -1, rear = -1;
int8_t numOfActiveAlarms = 0;

static TaskHandle_t timekeeprSgTaskHandle = NULL;
static StaticTask_t timekeeprSgTaskBuffer;
static StackType_t timekeeprSgTaskStack[TIMEKEEPER_SG_STACK_SIZE];

static QueueHandle_t timekeeperSgQueueHandle = NULL;
static StaticQueue_t timekeeperSgQueue;
static uint8_t timekeeperSgQueueStack[TIMEKEEPER_SG_QUEUE_LENGTH * sizeof(rtc_alarm_time_t)];

void initTimekeeperSg(void) {
    ASSERT((timekeeperSgTaskStack != NULL) && (timekeeprSgTaskBuffer != NULL));
    if(timekeeprSgTaskHandle == NULL) {
        timekeeprSgTaskHandle = xTaskCreateStatic(vTimekeeperSgTask, TIMEKEEPER_SG_NAME, TIMEKEEPER_SG_STACK_SIZE, NULL, TIMEKEEPER_SG_PRIORITY, timekeeprSgTaskStack, &timekeeprSgTaskBuffer);
    }

    ASSERT( (timekeeperSgQueueStack != NULL) && (&timekeeperSgQueue != NULL) );
    if (timekeeperSgQueueHandle == NULL) {
        timekeeperSgQueueHandle = xQueueCreateStatic(TIMEKEEPER_SG_QUEUE_LENGTH, sizeof(rtc_alarm_time_t), timekeeperSgQueueStack, &timekeeperSgQueue);
    }
}

obc_error_code_t sendToTimekeeperSgQueue(timekeeper_sg_event_t *event) {
    ASSERT(timekeeperSgQueueHandle != NULL);

    if(event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if(xQueueSend(timekeeperSgQueueHandle, (void *) event, TIMEKEEPER_SG_QUEUE_TX_WAIT_PERIOD) == pdPASS)
        return OBC_ERR_CODE_SUCCESS;
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

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

void addAlarm(timekeeper_sg_rtc_alarm alarm) {
    /*
        Following Figure 5.6: Adding new alarms 
        https://ntnuopen.ntnu.no/ntnu-xmlui/bitstream/handle/11250/2413318/14533_FULLTEXT.pdf?sequence=1#page=54&zoom=100,94,101
    */
   enQueue(alarm);
   bubbleSort();
}

void exectureAlarm() {
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
}

void swap(rtc_alarm_time_t *a1, rtc_alarm_time_t *a2) {
    rtc_alarm_time_t temp = *a1;
    *a1 = *a2;
    *a2 = temp;
}

//https://www.geeksforgeeks.org/sort-m-elements-of-given-circular-array-starting-from-index-k/
void bubbleSort() {
    int8_t n = numOfActiveAlarms;
    int8_t numOfOuterIterations = 0;
    int8_t numOfInnerIterations = 0;
    for(int8_t i = 0; i < n; i++) {
        for(int8_t j = front; j < front + numOfActiveAlarms - 1; j++) {
            // waiting on daniel's unix time PR to get merged to fix expression must have arithmetic pointer error below
            if(alarmQueue[j % ALARM_QUEUE_SIZE] > alarmQueue[(j + 1) % ALARM_QUEUE_SIZE]) {
                swap(&alarmQueue[j % ALARM_QUEUE_SIZE], &alarmQueue[(j + 1) % ALARM_QUEUE_SIZE]);
            }
        }
    }
}

static void vTimekeeperSgTask(void * pvParameters) {
    ASSERT(timekeeperSgQueueHandle != NULL);

    /* Send initial messages to system queues */
    sendStartupMessages();

    while(1) {
        timekeeper_sg_event_t inMsg;

        if(xQueueReceive(timekeeperSgQueueHandle, &inMsg, TIMEKEEPER_SG_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            inMsg.eventID = TIMEKEEPER_SG_NULL_EVENT_ID;
        
        switch (inMsg.eventID) {
            case ADD_ALARM_EVENT_ID:
                addAlarm(inMsg.data.alarm);
                break;
            case SET_ALARM1_EVENT_ID:
                setAlarm1(inMsg.data.alarm.alarmVal, inMsg.data.alarm.mode.alarm1Mode);
                break;
            case SET_ALARM2_EVENT_ID:
                setAlarm2(inMsg.data.alarm.alarmVal, inMsg.data.alarm.mode.alarm2Mode);
                break;
            case EXECUTE_ALARM_EVENT_ID:
                exectureAlarm();
                break;
            default:
                ;
        }
    }
}