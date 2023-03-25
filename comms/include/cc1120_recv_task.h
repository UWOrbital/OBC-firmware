#ifndef COMMS_INCLUDE_CC1120_RECV_TASK_H
#define COMMS_INCLUDE_CC1120_RECV_TASK_H

#include "obc_logging.h"
#include <os_portmacro.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <sys_common.h>
#include <stdbool.h>

#define RECV_DATA_QUEUE_LENGTH 255U // fill in kater
#define RECV_DATA_QUEUE_ITEM_SIZE 278

#define RECV_TASK_NAME "cc1120_receive_task"
#define RECV_STACK_SIZE 278
#define RECV_PRIORITY 1

void initRecvTask(void);

#endif /* COMMS_INCLUDE_CC1120_RECV_TASK_H */