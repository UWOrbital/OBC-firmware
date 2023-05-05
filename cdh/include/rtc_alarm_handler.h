#ifndef CDH_INCLUDE_RTC_ALARM_HANDLER_H_
#define CDH_INCLUDE_RTC_ALARM_HANDLER_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"
#include "rtc_alarm_handler_util.h"

/**
 * @enum	rtc_alarm_handler_event_id_t
 * @brief	rtc_alarm_handler event ID enum.
 *
 * Enum containing all possible event IDs passed to the supervisor event queue.
*/
typedef enum {
    RTC_ALARM_HANDLER_NULL_EVENT_ID,
    ADD_ALARM_EVENT_ID,
    SET_ALARM1_EVENT_ID,
    SET_ALARM2_EVENT_ID,
    EXECUTE_ALARM_EVENT_ID,
} rtc_alarm_handler_event_id_t;

/**
 * @union	rtc_alarm_handler_event_data_t
 * @brief	Timekeeper_sg event data union
*/
//making it a union for now since there might be other data types needed
typedef union {
    rtc_alarm_handler_rtc_alarm alarm;
} rtc_alarm_handler_event_data_t;

/**
 * @struct	rtc_alarm_handler_event_t
 * @brief	Timekeeper_sg event struct
 *
 * Holds the message data for each event sent/received by the rtc_alarm_handler manager queue.
*/
typedef struct {
    rtc_alarm_handler_event_id_t eventID;
    rtc_alarm_handler_event_data_t data;
} rtc_alarm_handler_event_t;

/* rtc_alarm_handler queue config */
#define RTC_ALARM_HANDLER_QUEUE_LENGTH 10U
#define RTC_ALARM_HANDLER_QUEUE_ITEM_SIZE sizeof(rtc_alarm_handler_event_t)
#define RTC_ALARM_HANDLER_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define RTC_ALARM_HANDLER_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Initialize the rtc_alarm_handler task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initSupervisor(void);

/**
 * @brief	Send an event to the rtc_alarm_handler queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToRtcAlarmHandlerQueue(rtc_alarm_handler_event_t *event);

#endif /*CDH_INCLUDE_RTC_ALARM_HANDLER_H_*/