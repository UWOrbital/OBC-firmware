#pragma once

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <os_task.h>

#include "obc_assert.h"

/*  NOTES ON TASK PRIORITIES
    - Idle task priority defined as 0
    - Timer task priority defined in FreeRTOSConfig.h
    - All task priorities must be in [0, OBC_TASK_MAX_PRIORITY]
*/

#define OBC_TASK_MAX_PRIORITY configMAX_PRIORITIES - 1U

/* Supervisor task config */
#define SUPERVISOR_STACK_SIZE 1024U
#define SUPERVISOR_NAME "supervisor"
#define SUPERVISOR_PRIORITY tskIDLE_PRIORITY + 4U

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE 1024U
#define TELEMETRY_NAME "telemetry_mgr"
#define TELEMETRY_PRIORITY tskIDLE_PRIORITY + 1U

/* Command Manager task config */
#define CMD_MANAGER_STACK_SIZE 1024U
#define CMD_MANAGER_NAME "cmd_mgr"
#define CMD_MANAGER_PRIORITY tskIDLE_PRIORITY + 1U

/* Comms Manager task config */
#define COMMS_MANAGER_STACK_SIZE 1024U
#define COMMS_MANAGER_NAME "comms_mgr"
#define COMMS_MANAGER_PRIORITY tskIDLE_PRIORITY + 2U

/* Comms telemetry encode task config */
#define COMMS_DOWNLINK_ENCODE_STACK_SIZE 512U
#define COMMS_DOWNLINK_ENCODE_NAME "comms_encoder"
#define COMMS_DOWNLINK_ENCODE_PRIORITY tskIDLE_PRIORITY + 2U

/* Comms Decode task config */
#define COMMS_UPLINK_DECODE_NAME "comms_decoder"
#define COMMS_UPLINK_DECODE_STACK_SIZE 1024U
#define COMMS_UPLINK_DECODE_PRIORITY tskIDLE_PRIORITY + 2U

// All 3 comms tasks should have the same priority
STATIC_ASSERT_EQ(COMMS_UPLINK_DECODE_PRIORITY, COMMS_DOWNLINK_ENCODE_PRIORITY);
STATIC_ASSERT_EQ(COMMS_MANAGER_PRIORITY, COMMS_DOWNLINK_ENCODE_PRIORITY);

/* EPS Manager task config */
#define EPS_MANAGER_STACK_SIZE 1024U
#define EPS_MANAGER_NAME "eps_mgr"
#define EPS_MANAGER_PRIORITY tskIDLE_PRIORITY + 3U

/* Payload Manager task config */
#define PAYLOAD_MANAGER_STACK_SIZE 1024U
#define PAYLOAD_MANAGER_NAME "payload_mgr"
#define PAYLOAD_MANAGER_PRIORITY tskIDLE_PRIORITY + 1U

/* Timekeeper task config */
#define TIMEKEEPER_STACK_SIZE 1024U
#define TIMEKEEPER_NAME "timekeeper"
#define TIMEKEEPER_PRIORITY tskIDLE_PRIORITY + 5U

/* OBC Sw Watchdog task config */
#define SW_WATCHDOG_STACK_SIZE 128U
#define SW_WATCHDOG_NAME "sw_watchdog"
#define SW_WATCHDOG_PRIORITY OBC_TASK_MAX_PRIORITY

/* rtc_alarm_handler task config */
#define ALARM_HANDLER_STACK_SIZE 512U
#define ALARM_HANDLER_NAME "alarm_handler"
#define ALARM_HANDLER_PRIORITY tskIDLE_PRIORITY + 4U

/* Health Collector task config */
#define HEALTH_COLLECTOR_STACK_SIZE 256U
#define HEALTH_COLLECTOR_NAME "health_collector"
#define HEALTH_COLLECTOR_PRIORITY tskIDLE_PRIORITY + 1U

/* Task debug stats collector task config */
#if (DEBUG == 1)
#define TASK_STATS_COLLECTOR_STACK_SIZE 1024U
#define TASK_STATS_COLLECTOR_NAME "task_stats_collector"
#define TASK_STATS_COLLECTOR_PRIORITY tskIDLE_PRIORITY + 4U
#endif
