#ifndef OBC_QUEUE_H_
#define OBC_QUEUE_H_

#include <stdint.h>
#include <gio.h>
#include "ds3232_mz.h"
#include "rtc_alarm_handler_util.h"

#define ALARM_QUEUE_SIZE 10U

/**
 * @brief	check if alarm queue is full
 * @return  approproate boolean value
 */
uint8_t isFull();

/**
 * @brief	check if alarm queue is empty
 * @return  approproate boolean value
 */
uint8_t isEmpty();

/**
 * @brief	add alarm to a queue
 * @param	alarm   alarm to be added
 * @return  the error code
 */
obc_error_code_t enQueue(rtc_alarm_handler_rtc_alarm alarm);

/**
 * @brief	remove closest alarm in the queue
 * @return  the error code
 */
obc_error_code_t deQueue();

/**
 * @brief	swaps 2 alarm time values by reference
 * @param	a1   reference of first alarm value to be swapped
 * @param	a2   reference of second alarm value to be swapped
 */
void swap(rtc_alarm_time_t *a1, rtc_alarm_time_t *a2);

/**
 * @brief	standard bubble sort algo customized a bit for circular queues
 */
void bubbleSort();

#endif /*OBC_QUEUE_H_*/