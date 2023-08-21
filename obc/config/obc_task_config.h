#pragma once

#include <FreeRTOS.h>
#include <os_task.h>

/* Supervisor task config */
#define SUPERVISOR_STACK_SIZE 1024U
#define SUPERVISOR_NAME "supervisor"
#define SUPERVISOR_PRIORITY tskIDLE_PRIORITY + 1U

/* Telemetry task config */
#define TELEMETRY_STACK_SIZE 1024U
#define TELEMETRY_NAME "telemetry"
#define TELEMETRY_PRIORITY tskIDLE_PRIORITY + 1U

/* Command Manager task config */
#define CMD_MANAGER_STACK_SIZE 1024U
#define CMD_MANAGER_NAME "cmd_manager"
#define CMD_MANAGER_PRIORITY tskIDLE_PRIORITY + 1U

/* ADCS Manager task config */
#define ADCS_MANAGER_STACK_SIZE 1024U
#define ADCS_MANAGER_NAME "adcs_manager"
#define ADCS_MANAGER_PRIORITY tskIDLE_PRIORITY + 1U

/* Comms Manager task config */
#define COMMS_MANAGER_STACK_SIZE 1024U
#define COMMS_MANAGER_NAME "comms_manager"
#define COMMS_MANAGER_PRIORITY tskIDLE_PRIORITY + 5U  // Should be same priority as the other comms_link_mgr tasks

/* Comms telemetry encode task config */
#define COMMS_DOWNLINK_ENCODE_STACK_SIZE 512U
#define COMMS_DOWNLINK_ENCODE_NAME "comms_telemetry_encode"
#define COMMS_DOWNLINK_ENCODE_PRIORITY \
  tskIDLE_PRIORITY + 5U  // Should be same priority as the other comms_link_mgr tasks

/* Comms Decode task config */
#define COMMS_UPLINK_DECODE_NAME "decode_task"
#define COMMS_UPLINK_DECODE_STACK_SIZE 1024U
#define COMMS_UPLINK_DECODE_PRIORITY tskIDLE_PRIORITY + 5U  // Should be same priority as the other comms_link_mgr tasks

/* EPS Manager task config */
#define EPS_MANAGER_STACK_SIZE 1024U
#define EPS_MANAGER_NAME "eps_manager"
#define EPS_MANAGER_PRIORITY tskIDLE_PRIORITY + 1U

/* Payload Manager task config */
#define PAYLOAD_MANAGER_STACK_SIZE 1024U
#define PAYLOAD_MANAGER_NAME "payload_manager"
#define PAYLOAD_MANAGER_PRIORITY tskIDLE_PRIORITY + 1U

/* Timekeeper task config */
#define TIMEKEEPER_STACK_SIZE 1024U
#define TIMEKEEPER_NAME "timekeeper"
#define TIMEKEEPER_PRIORITY tskIDLE_PRIORITY + 100U

/* OBC Sw Watchdog task config */
#define SW_WATCHDOG_STACK_SIZE 128U
#define SW_WATCHDOG_NAME "sw_watchdog"
#define SW_WATCHDOG_PRIORITY tskIDLE_PRIORITY + 200U

/* rtc_alarm_handler task config */
#define ALARM_HANDLER_STACK_SIZE 512U
#define ALARM_HANDLER_NAME "alarm_handler"
#define ALARM_HANDLER_PRIORITY tskIDLE_PRIORITY + 1U

/* Health Collector task config */
#define HEALTH_COLLECTOR_STACK_SIZE 256U
#define HEALTH_COLLECTOR_NAME "health_collector"
#define HEALTH_COLLECTOR_PRIORITY tskIDLE_PRIORITY + 1U

/* Task debug stats collector task config */
#if (DEBUG == 1)
#define TASK_STATS_COLLECTOR_STACK_SIZE 1024U
#define TASK_STATS_COLLECTOR_NAME "task_stats_collector"
#define TASK_STATS_COLLECTOR_PRIORITY tskIDLE_PRIORITY + 50U
#endif
