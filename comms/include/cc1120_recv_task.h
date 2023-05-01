#ifndef COMMS_INCLUDE_CC1120_RECV_TASK_H
#define COMMS_INCLUDE_CC1120_RECV_TASK_H

#include "obc_logging.h"
#include "comms_manager.h"

#include <os_portmacro.h>

#include <stdbool.h>

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
obc_error_code_t startUplink(void);

#endif /* COMMS_INCLUDE_CC1120_RECV_TASK_H */