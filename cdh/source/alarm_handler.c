#include "alarm_handler.h"
#include "ds3232_mz.h"
#include "obc_task_config.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_time.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_queue.h>
#include <sys_common.h>

#define ALARM_HANDLER_QUEUE_LENGTH 64U
#define ALARM_HANDLER_QUEUE_ITEM_SIZE sizeof(alarm_handler_event_t)
#define ALARM_HANDLER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define ALARM_HANDLER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

static TaskHandle_t alarmHandlerTaskHandle;
static StaticTask_t alarmHandlerTaskBuffer;
static StackType_t alarmHandlerTaskStack[ALARM_HANDLER_STACK_SIZE];

static QueueHandle_t alarmHandlerQueueHandle;
static StaticQueue_t alarmHandlerQueue;
static uint8_t alarmHandlerQueueStack[ALARM_HANDLER_QUEUE_LENGTH * ALARM_HANDLER_QUEUE_ITEM_SIZE];

#define ALARM_QUEUE_SIZE 64U
static alarm_handler_alarm_info_t alarmQueue[ALARM_QUEUE_SIZE];
static size_t numActiveAlarms = 0;

static void alarmHandler(void * pvParameters);

static obc_error_code_t enqueueAlarm(alarm_handler_alarm_info_t alarm, size_t *insertedAlarmIndex);

static obc_error_code_t dequeueAlarm(alarm_handler_alarm_info_t *alarm);

static obc_error_code_t peekEarliestAlarm(alarm_handler_alarm_info_t *alarm);

static void datetimeToAlarmTime(rtc_date_time_t *datetime, rtc_alarm_time_t *alarmTime);

void initAlarmHandler(void) {
    ASSERT( (alarmHandlerTaskStack != NULL) && (&alarmHandlerTaskBuffer != NULL) );
    alarmHandlerTaskHandle = xTaskCreateStatic(alarmHandler,
        ALARM_HANDLER_NAME,
        ALARM_HANDLER_STACK_SIZE,
        NULL,
        ALARM_HANDLER_PRIORITY,
        alarmHandlerTaskStack,
        &alarmHandlerTaskBuffer
    );

    ASSERT( (alarmHandlerQueueStack != NULL) && (&alarmHandlerQueue != NULL) );
    alarmHandlerQueueHandle = xQueueCreateStatic(
        ALARM_HANDLER_QUEUE_LENGTH,
        ALARM_HANDLER_QUEUE_ITEM_SIZE,
        alarmHandlerQueueStack,
        &alarmHandlerQueue
    );
}

static void alarmHandler(void * pvParameters) {
    obc_error_code_t errCode;

    // TODO: Move this to a separate init function. Probably to where rtcInit() is called.
    rtc_control_t rtcControl = {0};
    LOG_IF_ERROR_CODE(getControlRTC(&rtcControl));
    rtcControl.A1IE = 1;
    rtcControl.INTCN = 1;
    LOG_IF_ERROR_CODE(setControlRTC(&rtcControl));

    while(1) {
        alarm_handler_event_t event;

        if (xQueueReceive(alarmHandlerQueueHandle, &event, ALARM_HANDLER_QUEUE_RX_WAIT_PERIOD) != pdPASS) {
            continue;
        }

        switch (event.id) {
            case ALARM_HANDLER_NEW_ALARM: {
                size_t insertedAlarmIndex;
                LOG_IF_ERROR_CODE(enqueueAlarm(event.alarmInfo, &insertedAlarmIndex));
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    break;
                }

                if (insertedAlarmIndex != 0) {
                    break;
                }

                // If the new alarm is the earliest alarm, set the RTC alarm to it
                rtc_date_time_t alarmDateTime;
                LOG_IF_ERROR_CODE(unixToDatetime(event.alarmInfo.unixTime, &alarmDateTime));
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    break;
                }

                rtc_alarm_time_t alarmTime;
                datetimeToAlarmTime(&alarmDateTime, &alarmTime);
                LOG_IF_ERROR_CODE(setAlarm1RTC(RTC_ALARM1_MATCH_DATE_HOURS_MINUTES_SECONDS, alarmTime));

                break;
            }

            case ALARM_HANDLER_ALARM_TRIGGERED: {
                // Reset alarm flag
                LOG_IF_ERROR_CODE(clearAlarm1RTC());
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    break;
                }

                alarm_handler_alarm_info_t alarm;
                LOG_IF_ERROR_CODE(peekEarliestAlarm(&alarm));
                if (errCode != OBC_ERR_CODE_SUCCESS) {
                    break;
                }

                // TODO: Maybe fetch current time from the RTC instead of the local time
                if (getCurrentUnixTime() < alarm.unixTime) {
                    // The alarm interrupt fired before the alarm time for some reason
                    // Reset the alarm to the correct time
                    rtc_date_time_t alarmDateTime;
                    LOG_IF_ERROR_CODE(unixToDatetime(alarm.unixTime, &alarmDateTime));
                    if (errCode != OBC_ERR_CODE_SUCCESS) {
                        break;
                    }

                    rtc_alarm_time_t alarmTime;
                    datetimeToAlarmTime(&alarmDateTime, &alarmTime);
                    LOG_IF_ERROR_CODE(setAlarm1RTC(RTC_ALARM1_MATCH_DATE_HOURS_MINUTES_SECONDS, alarmTime));
                    break;
                }

                uint32_t timestampThresh = alarm.unixTime;

                // Execute callbacks for all alarms that have triggered
                for (size_t i = 0; i < numActiveAlarms; i++) {
                    LOG_IF_ERROR_CODE(peekEarliestAlarm(&alarm));
                    if (errCode != OBC_ERR_CODE_SUCCESS) {
                        break;
                    }

                    if (alarm.unixTime > timestampThresh) {
                        break;
                    }

                    LOG_IF_ERROR_CODE(dequeueAlarm(&alarm));
                    if (errCode != OBC_ERR_CODE_SUCCESS) {
                        break;
                    }

                    // TODO: Select callback based on alarm type
                    LOG_IF_ERROR_CODE(alarm.callbackDef.defaultCallback());
                }

                break;
            }

            default: {
                LOG_ERROR_CODE(OBC_ERR_CODE_UNSUPPORTED_EVENT);
                continue;
            }
        }
    }
}

obc_error_code_t sendToAlarmHandlerQueue(alarm_handler_event_t *event) {
    if (alarmHandlerQueueHandle == NULL) {
        return OBC_ERR_CODE_INVALID_STATE;
    }

    if (event == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (xQueueSend(alarmHandlerQueueHandle, (void *)event, ALARM_HANDLER_QUEUE_TX_WAIT_PERIOD) == pdPASS) {
        return OBC_ERR_CODE_SUCCESS;
    }
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static obc_error_code_t enqueueAlarm(alarm_handler_alarm_info_t alarm, size_t *insertedAlarmIndex) {
    if (numActiveAlarms >= ALARM_QUEUE_SIZE) {
        return OBC_ERR_CODE_QUEUE_FULL;
    }

    // Insert the alarm into the queue in order of increasing time
    size_t i = 0;
    while (i < numActiveAlarms && alarmQueue[i].unixTime < alarm.unixTime) {
        i++;
    }

    // Shift all alarms after the new alarm back by one
    for (size_t j = numActiveAlarms; j > i; j--) {
        alarmQueue[j] = alarmQueue[j - 1];
    }

    // Insert the new alarm
    alarmQueue[i] = alarm;
    numActiveAlarms++;

    *insertedAlarmIndex = i;

    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t dequeueAlarm(alarm_handler_alarm_info_t *alarm) {
    if (numActiveAlarms == 0) {
        return OBC_ERR_CODE_QUEUE_EMPTY;
    }

    *alarm = alarmQueue[0];

    // Shift all alarms after the new alarm back by one
    for (size_t j = 0; j < numActiveAlarms - 1; j++) {
        alarmQueue[j] = alarmQueue[j + 1];
    }

    numActiveAlarms--;

    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t peekEarliestAlarm(alarm_handler_alarm_info_t *alarm) {
    if (numActiveAlarms == 0) {
        return OBC_ERR_CODE_QUEUE_EMPTY;
    }

    *alarm = alarmQueue[0];

    return OBC_ERR_CODE_SUCCESS;
}

void alarmInterruptCallback(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Currently, it's fine if the alarm handler task received other events 
    // after we send the alarm triggered event.

    alarm_handler_event_t event = {.id = ALARM_HANDLER_ALARM_TRIGGERED};
    xQueueSendToFrontFromISR(alarmHandlerQueueHandle, (void *)&event, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void datetimeToAlarmTime(rtc_date_time_t *datetime, rtc_alarm_time_t *alarmTime) {
    alarmTime->time.seconds = datetime->time.seconds;
    alarmTime->time.minutes = datetime->time.minutes;
    alarmTime->time.hours = datetime->time.hours;
    alarmTime->date = datetime->date.date;
}
