#ifndef COMMON_INCLUDE_OBC_TASK_CONFIG_H_
#define COMMON_INCLUDE_OBC_TASK_CONFIG_H_

#include <FreeRTOS.h>
#include <os_task.h>

/* Supervisor task config */
#define SUPERVISOR_STACK_SIZE   1024U
#define SUPERVISOR_NAME         "supervisor"
#define SUPERVISOR_PRIORITY     tskIDLE_PRIORITY + 1U

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE   1024U
#define TELEMETRY_NAME         "telemetry"
#define TELEMETRY_PRIORITY     tskIDLE_PRIORITY + 1U

/* Command Manager task config */
#define CMD_MANAGER_STACK_SIZE 1024U
#define CMD_MANAGER_NAME       "cmd_manager"
#define CMD_MANAGER_PRIORITY   tskIDLE_PRIORITY + 1U

/* ADCS Manager task config */
#define ADCS_MANAGER_STACK_SIZE   1024U
#define ADCS_MANAGER_NAME         "adcs_manager"
#define ADCS_MANAGER_PRIORITY     tskIDLE_PRIORITY + 1U

/* Comms Manager task config */
#define COMMS_MANAGER_STACK_SIZE   1024U
#define COMMS_MANAGER_NAME         "comms_manager"
#define COMMS_MANAGER_PRIORITY     tskIDLE_PRIORITY + 1U

/* EPS Manager task config */
#define EPS_MANAGER_STACK_SIZE   1024U
#define EPS_MANAGER_NAME         "eps_manager"
#define EPS_MANAGER_PRIORITY     tskIDLE_PRIORITY + 1U

/* Payload Manager task config */
#define PAYLOAD_MANAGER_STACK_SIZE   1024U
#define PAYLOAD_MANAGER_NAME         "payload_manager"
#define PAYLOAD_MANAGER_PRIORITY     tskIDLE_PRIORITY + 1U

/* Timekeeper task config */
#define TIMEKEEPER_STACK_SIZE   1024U
#define TIMEKEEPER_NAME         "timekeeper"
#define TIMEKEEPER_PRIORITY     tskIDLE_PRIORITY + 100U

#endif /* COMMON_INCLUDE_OBC_TASK_CONFIG_H_ */
