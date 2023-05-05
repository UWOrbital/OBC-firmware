#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <stdlib.h>
#include "ds3232_mz.h"
#include "rtc_alarm_handler.h"
#include "time.h"
#include "obc_task_config.h"
#include "rtc_alarm_handler_util.h"

static TaskHandle_t rtcAlarmHandlerTaskHandle = NULL;
static StaticTask_t rtcAlarmHandlerTaskBuffer;
static StackType_t rtcAlarmHandlerTaskStack[RTC_ALARM_HANDLER_STACK_SIZE];

static QueueHandle_t rtcAlarmHandlerQueueHandle = NULL;
static StaticQueue_t rtcAlarmHandlerQueue;
static uint8_t rtcAlarmHandlerQueueStack[RTC_ALARM_HANDLER_QUEUE_LENGTH * sizeof(rtc_alarm_time_t)];

/**
 * @brief	rtc_alarm_handler task.
 * @param	pvParameters	Task parameters.
 */
static void vrtcAlarmHandlerTask(void * pvParameters);

void initrtcAlarmHandler(void) {
    ASSERT((rtcAlarmHandlerTaskStack != NULL) && (rtcAlarmHandlerTaskBuffer != NULL));
    if(rtcAlarmHandlerTaskHandle == NULL) {
        rtcAlarmHandlerTaskHandle = xTaskCreateStatic(vrtcAlarmHandlerTask, RTC_ALARM_HANDLER_NAME, RTC_ALARM_HANDLER_STACK_SIZE, NULL, RTC_ALARM_HANDLER_PRIORITY, rtcAlarmHandlerTaskStack, &rtcAlarmHandlerTaskBuffer);
    }

    ASSERT( (rtcAlarmHandlerQueueStack != NULL) && (&rtcAlarmHandlerQueue != NULL) );
    if (rtcAlarmHandlerQueueHandle == NULL) {
        rtcAlarmHandlerQueueHandle = xQueueCreateStatic(RTC_ALARM_HANDLER_QUEUE_LENGTH, sizeof(rtc_alarm_time_t), rtcAlarmHandlerQueueStack, &rtcAlarmHandlerQueue);
    }
}

obc_error_code_t sendToRtcAlarmHandlerQueue(rtc_alarm_handler_event_t *event) {
    ASSERT(rtcAlarmHandlerQueueHandle != NULL);

    if(event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if(xQueueSend(rtcAlarmHandlerQueueHandle, (void *) event, RTC_ALARM_HANDLER_QUEUE_TX_WAIT_PERIOD) == pdPASS)
        return OBC_ERR_CODE_SUCCESS;
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static void vrtcAlarmHandlerTask(void * pvParameters) {
    ASSERT(rtcAlarmHandlerQueueHandle != NULL);

    while(1) {
        rtc_alarm_handler_event_t inMsg;

        if(xQueueReceive(rtcAlarmHandlerQueueHandle, &inMsg, RTC_ALARM_HANDLER_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            inMsg.eventID = RTC_ALARM_HANDLER_NULL_EVENT_ID;
        
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