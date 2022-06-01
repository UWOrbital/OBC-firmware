/*
 * eventManager.h
 *
 * May 31, 2022
 * kiransuren
 *
 */


#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <stdint.h>
#include "os_portmacro.h"

#define NUM_OF_QUEUES 3
#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(uint16_t)
#define QUEUE_WAIT_PERIOD 10/portTICK_PERIOD_MS     //TODO: May need to change this wait period depending on testing
#define MASTER_EVENT_MUTEX_WAIT 10/portTICK_PERIOD_MS        //TODO: Not a good idea to indefinitely wait to push to a queue, figure out another solution

typedef enum{
    SUPERVISOR_QUEUE_ID,
    COMMS_QUEUE_ID,
    ADCS_QUEUE_ID
} event_queue_id;

// TODO: refactor location of event id enum, maybe new events header file?
typedef enum{
    NULL_EVENT_ID,
    TEST_EVENT1_ID,
    TEST_EVENT2_ID,
    TEST_EVENT3_ID,
    TEST_EVENT4_ID
} event_id;


void initializeEventQueues();
void addEventQueue(event_queue_id eventQueueID, event_id eventID);
event_id receiveEventQueue(event_queue_id eventQueueID);

#endif
