/*
 * eventManager.c
 *
 * May 31, 2022
 * kiransuren
 *
 */

#include "eventManager.h"

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_portmacro.h"
#include "os_task.h"
#include "os_semphr.h"

#include "sys_common.h"
#include <stdint.h>

static QueueHandle_t masterEventQueueList[NUM_OF_QUEUES];           // list of FreeRTOS queues that represent each tasks' event queue
static SemaphoreHandle_t masterEventQueueListMutex;

/*
 * initializeEventQueues
 *
 * initializes the masterQueueList by creating queue handles for every task
 */
void initializeEventQueues(){
    for(int i=0; i<NUM_OF_QUEUES; i++){
        masterEventQueueList[i] = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    }
    masterEventQueueListMutex = xSemaphoreCreateMutex();
}

/*
 * addEventQueue
 * - adds a event to the specified event queue
 *
 * @param eventQueueID - the queue to be sent to
 * @param eventID - the event enum to send to the queue
 */
void addEventQueue(event_queue_id eventQueueID, event_id eventID){
    if(xSemaphoreTake(masterEventQueueListMutex, MASTER_EVENT_MUTEX_WAIT)){     //TODO: This mau not be an ideal piece of code, talk to Kiran about more details
        QueueHandle_t queueHandle = masterEventQueueList[eventQueueID];
        xQueueSend(queueHandle, (void *) &eventID, QUEUE_WAIT_PERIOD);
        xSemaphoreGive(masterEventQueueListMutex);
    }
}

/*
 * receiveEventQueue
 * - receives (pops) event from queue
 *
 * @param eventQueueID - the queue to receive from
 * @return the event enum popped from the queue
 */
event_id receiveEventQueue(event_queue_id eventQueueID){
    QueueHandle_t queueHandle = masterEventQueueList[eventQueueID];
    event_id eventID;
    if(xQueueReceive(queueHandle, (void *) &eventID, QUEUE_WAIT_PERIOD)){   // TODO: Do we need a mutex for reading?
        return eventID;
    }
    return NULL_EVENT_ID;
}




