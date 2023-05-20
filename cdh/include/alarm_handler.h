#ifndef CDH_INCLUDE_RTC_ALARM_HANDLER_H_
#define CDH_INCLUDE_RTC_ALARM_HANDLER_H_

#include <stdint.h>

#include "obc_errors.h"
#include "command_callbacks.h"
#include "obc_time.h"

// Alarm handler event IDs
typedef enum {
    ALARM_HANDLER_NEW_ALARM, // New alarm to be added to the queue
    ALARM_HANDLER_ALARM_TRIGGERED, // RTC alarm triggered
} alarm_handler_event_id_t;

// Alarm handler callback definition
typedef union {
    obc_error_code_t (*defaultCallback)(void);
} alarm_handler_callback_def_t;

// All required information for an alarm
typedef struct {
    uint32_t unixTime;
    alarm_handler_callback_def_t callbackDef;
} alarm_handler_alarm_info_t;

// Alarm handler event struct
typedef struct {
    alarm_handler_event_id_t id;
    union {
        alarm_handler_alarm_info_t alarmInfo;
    };
} alarm_handler_event_t;

/**
 * @brief Initialize the alarm handler task and associated FreeRTOS constructs (queues, timers, etc.)
 * 
 */
void initAlarmHandler(void);

/**
 * @brief Send an event to the alarm handler queue.
 * 
 * @param event Event to send.
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the event was sent successfully, error code otherwise
 */
obc_error_code_t sendToAlarmHandlerQueue(alarm_handler_event_t *event);

/**
 * @brief Handle the RTC alarm interrupt.
 * 
 */
void alarmInterruptCallback(void);

#endif /* CDH_INCLUDE_RTC_ALARM_HANDLER_H_ */
