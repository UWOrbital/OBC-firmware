#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <stdlib.h>
#include "ds3232_mz.h"
#include "timekeeper_sg.h"
#include "time.h"
#include "obc_task_config.h"
#include "timekeeper_sg_util.h"

static TaskHandle_t timekeeprSgTaskHandle = NULL;
static StaticTask_t timekeeprSgTaskBuffer;
static StackType_t timekeeprSgTaskStack[TIMEKEEPER_SG_STACK_SIZE];

static QueueHandle_t timekeeperSgQueueHandle = NULL;
static StaticQueue_t timekeeperSgQueue;
static uint8_t timekeeperSgQueueStack[TIMEKEEPER_SG_QUEUE_LENGTH * sizeof(rtc_alarm_time_t)];

/**
 * @brief	Timekeeper_sg task.
 * @param	pvParameters	Task parameters.
 */
static void vTimekeeperSgTask(void * pvParameters);

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

static void vTimekeeperSgTask(void * pvParameters) {
    ASSERT(timekeeperSgQueueHandle != NULL);

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