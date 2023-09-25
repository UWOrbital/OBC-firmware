#pragma once

#include <FreeRTOS.h>
#include <os_task.h>

typedef enum {
  OBC_SCHEDULER_TASK_ID_STATE_MGR,
  OBC_SCHEDULER_TASK_ID_TELEMETRY_MGR,
  OBC_SCHEDULER_TASK_ID_COMMAND_MGR,
  OBC_SCHEDULER_TASK_ID_COMMS_MGR,
  OBC_SCHEDULER_TASK_ID_COMMS_DOWNLINK_ENCODER,
  OBC_SCHEDULER_TASK_ID_COMMS_UPLINK_DECODER,
  OBC_SCHEDULER_TASK_ID_EPS_MGR,
  OBC_SCHEDULER_TASK_ID_PAYLOAD_MGR,
  OBC_SCHEDULER_TASK_ID_TIMEKEEPER,
  OBC_SCHEDULER_TASK_ID_SW_WATCHDOG,
  OBC_SCHEDULER_TASK_ID_ALARM_HANDLER,
  OBC_SCHEDULER_TASK_ID_HEALTH_COLLECTOR,
  OBC_SCHEDULER_TASK_ID_TASK_STATS_COLLECTOR,

  OBC_SCHEDULER_TASK_COUNT
} obc_scheduler_task_id_t;

typedef struct {
  TaskHandle_t *taskHandle;
  StaticTask_t *taskBuffer;
  StackType_t *taskStack;
  uint32_t stackSize;
  uint32_t priority;
  const char *taskName;
  void (*taskFunc)(void *);
} obc_scheduler_config_t;

obc_scheduler_config_t *obcSchedulerGetConfig(obc_scheduler_task_id_t taskID);
void obcSchedulerCreateTask(obc_scheduler_task_id_t taskID);
