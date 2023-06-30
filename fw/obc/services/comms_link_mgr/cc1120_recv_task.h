#pragma once

#include "obc_logging.h"
#include "comms_manager.h"

#include <os_portmacro.h>

#include <stdbool.h>

/**
 * @brief initializes the receive task for uplink
 * @param pvParameters parameters to be passed into the created task
 *
 * @return void
 */
void initRecvTask(void *pvParameters);

/**
 * @brief send an event to the Recv queue to start uplink
 *
 * @param event the type of comms event
 *
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
 */
obc_error_code_t startUplink(void);
