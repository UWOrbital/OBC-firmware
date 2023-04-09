#ifndef CDH_INCLUDE_TIMEKEEPER_SG_H_
#define CDH_INCLUDE_TIMEKEEPER_SG_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"

/**
 * @enum	timekeeper_sg_event_id_t
 * @brief	Timekeeper_sg event ID enum.
 *
 * Enum containing all possible event IDs passed to the supervisor event queue.
*/
typedef enum {
    TIMEKEEPER_SG_NULL_EVENT_ID,
    ADD_ALARM_EVENT_ID,
    SET_ALARM1_EVENT_ID,
    SET_ALARM2_EVENT_ID,
    EXECUTE_ALARM_EVENT_ID,
} timekeeper_sg_event_id_t;

/**
 * @union	timekeeper_sg_alarm_mode_t
 * @brief	union of alarm modes for alarm 1 and 2 on RTC.
*/
typedef union {
    rtc_alarm1_mode_t alarm1Mode;
    rtc_alarm2_mode_t alarm2Mode;
} timekeeper_sg_alarm_mode_t;

/**
 * @struct	timekeeper_sg_rtc_alarm
 * @brief	atruct for RTC alarm containing both mode and alarm value
 */
typedef struct {
    timekeeper_sg_alarm_mode_t mode;
    rtc_alarm_time_t alarmVal;
} timekeeper_sg_rtc_alarm;

timekeeper_sg_rtc_alarm dummyAlarm = {.mode.alarm1Mode = RTC_ALARM1_ONCE_PER_SECOND,
                                      .alarmVal = {1, {1, 1, 1}},
                                    };

/**
 * @union	timekeeper_sg_event_data_t
 * @brief	Timekeeper_sg event data union
*/
//making it a union for now since there might be other data types needed
typedef union {
    timekeeper_sg_rtc_alarm alarm;
} timekeeper_sg_event_data_t;

/**
 * @struct	timekeeper_sg_event_t
 * @brief	Timekeeper_sg event struct
 *
 * Holds the message data for each event sent/received by the timekeeper_sg manager queue.
*/
typedef struct {
    timekeeper_sg_event_id_t eventID;
    timekeeper_sg_event_data_t data;
} timekeeper_sg_event_t;

#define ALARM_QUEUE_SIZE 10U
timekeeper_sg_rtc_alarm alarmQueue[ALARM_QUEUE_SIZE];   // dummy size for now, get better clarity on queuings

/* Timekeeper_sg queue config */
#define TIMEKEEPER_SG_QUEUE_LENGTH 10U
#define TIMEKEEPER_SG_QUEUE_ITEM_SIZE sizeof(timekeeper_sg_event_t)
#define TIMEKEEPER_SG_QUEUE_RX_WAIT_PERIOD pdMS_TO_TICKS(10)
#define TIMEKEEPER_SG_QUEUE_TX_WAIT_PERIOD pdMS_TO_TICKS(10)

/**
 * @brief	Initialize the timekeeper_sg task and associated FreeRTOS constructs (queues, timers, etc.)
 */
void initSupervisor(void);

/**
 * @brief	Send an event to the timekeeper_sg queue.
 * @param	event	Event to send.
 * @return The error code
 */
obc_error_code_t sendToTimekeeperSgQueue(timekeeper_sg_event_t *event);

obc_error_code_t setCurrentDateTime(rtc_date_time_t currentTime);

// should this be event based as well?
rtc_time_t getCurrentTime(rtc_time_t getTime);

// TBD what to do with this, local interrupt or use RTC alarm, for now just dequeing from alarm queue
//  void executeAlarm();

#endif /*CDH_INCLUDE_TIMEKEEPER_SG_H_*/