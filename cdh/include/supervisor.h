/*
 * supervisor.h
 *
 * May 30, 2022
 * kiransuren
 *
 */


#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "hal_stdtypes.h"

#define SUPERVISOR_STACK_SIZE   200
#define SUPERVISOR_NAME         "supervisor"
#define SUPERVISOR_PRIORITY     1
#define SUPERVISOR_DELAY_TICKS  1000/portTICK_PERIOD_MS

typedef enum {
    SUPERVISOR_NULL_EVENT_ID,
    TURN_OFF_LED_EVENT_ID,
} supervisor_event_id_t;

typedef union {
    int i;
    float f;
} supervisor_event_data_t;

typedef struct {
    supervisor_event_id_t eventID;
    supervisor_event_data_t data;
} supervisor_event_t;

#define SUPERVISOR_QUEUE_LENGTH 10
#define SUPERVISOR_QUEUE_ITEM_SIZE sizeof(supervisor_event_id_t)
#define SUPERVISOR_QUEUE_WAIT_PERIOD 10/portTICK_PERIOD_MS

void vSupervisorTask(void * pvParameters);

#endif
