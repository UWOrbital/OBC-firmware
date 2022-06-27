/*
 * telemetry.h
 *
 *  Created on: Jun. 22, 2022
 *      Author: kiran
 */

#ifndef CDH_INCLUDE_TELEMETRY_H_
#define CDH_INCLUDE_TELEMETRY_H_


#define TELEMETRY_STACK_SIZE   200
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY     1
#define TELEMETRY_DELAY_TICKS  1000/portTICK_PERIOD_MS

typedef enum {
    TELEMETRY_NULL_EVENT_ID,
    TURN_ON_LED_EVENT_ID,
} telemetry_event_id_t;

typedef union {
    int delayPeriod;
} telemetry_event_data_t;

typedef struct {
    telemetry_event_id_t eventID;
    telemetry_event_data_t data;
} telemetry_event_t;

#define TELEMETRY_QUEUE_LENGTH 10
#define TELEMETRY_QUEUE_ITEM_SIZE sizeof(telemetry_event_id_t)
#define TELEMETRY_QUEUE_WAIT_PERIOD 10/portTICK_PERIOD_MS

void vTelemetryTask(void * pvParameters);


#endif /* CDH_INCLUDE_TELEMETRY_H_ */
