#ifndef COMMS_INCLUDE_CC1120_RECV_TASK_H
#define COMMS_INCLUDE_CC1120_RECV_TASK_H

#include "obc_logging.h"
#include "comms_manager.h"

#include <os_portmacro.h>

#include <stdbool.h>

#define RECV_DATA_QUEUE_LENGTH 10U 
#define RECV_DATA_QUEUE_ITEM_SIZE sizeof(comms_event_t)

#define RECV_TASK_NAME "cc1120_receive_task"
#define RECV_STACK_SIZE 1024U
#define RECV_PRIORITY tskIDLE_PRIORITY + 1U

#define RECV_DATA_QUEUE_RX_WAIT_PERIOD portMAX_DELAY
#define RECV_DATA_QUEUE_TX_WAIT_PERIOD portMAX_DELAY

/**
 * @brief initializes the receive task for uplink
 * 
 * @return void
*/
void initRecvTask(void);

/**
 * @brief send an event to the Recv queue to start uplink
 * 
 * @param event the type of comms event
 * 
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
*/
obc_error_code_t sendToRecvDataQueue(comms_event_t *event);

#endif /* COMMS_INCLUDE_CC1120_RECV_TASK_H */