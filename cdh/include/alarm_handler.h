#ifndef CDH_INCLUDE_RTC_ALARM_HANDLER_H_
#define CDH_INCLUDE_RTC_ALARM_HANDLER_H_

#include <stdint.h>

#include "obc_errors.h"
#include "command_callbacks.h"
#include "obc_time.h"

typedef enum {
    ALARM_HANDLER_NEW_ALARM,
    ALARM_HANDLER_ALARM_TRIGGERED,
} alarm_handler_event_id_t;

typedef obc_error_code_t (*alarm_handler_callback_def_t)(void);

typedef struct {
    uint32_t unixTime;
    alarm_handler_callback_def_t callbackDef;
} alarm_handler_alarm_info_t;

typedef struct {
    alarm_handler_event_id_t id;
    union {
        alarm_handler_alarm_info_t alarmInfo;
    };
} alarm_handler_event_t;

void initAlarmHandler(void);

obc_error_code_t sendToAlarmHandlerQueue(alarm_handler_event_t *event);

void alarmInterruptCallback(void);

#endif /* CDH_INCLUDE_RTC_ALARM_HANDLER_H_ */
