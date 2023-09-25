#include "obc_scheduler_config.h"

#include "obc_errors.h"
#include "obc_assert.h"

#include <FreeRTOSConfig.h>
#include <sys_common.h>

#include <stdint.h>

/* DEFINES */
#define TASK_STATE_MGR_STACK_SIZE 1024U
#define TASK_STATE_MGR_NAME "state_mgr"

#define TASK_TELEMETRY_MGR_STACK_SIZE 1024U
#define TASK_TELEMETRY_MGR_NAME "telemetry_mgr"

#define TASK_COMMAND_MGR_STACK_SIZE 1024U
#define TASK_COMMAND_MGR_NAME "command_mgr"

#define TASK_COMMS_MGR_STACK_SIZE 1024U
#define TASK_COMMS_MGR_NAME "comms_mgr"

#define TASK_COMMS_DOWNLINK_ENCODER_STACK_SIZE 512U
#define TASK_COMMS_DOWNLINK_ENCODER_NAME "comms_encoder"

#define TASK_COMMS_UPLINK_DECODER_STACK_SIZE 1024U
#define TASK_COMMS_UPLINK_DECODER_NAME "comms_decoder"

#define TASK_EPS_MGR_STACK_SIZE 1024U
#define TASK_EPS_MGR_NAME "eps_mgr"

#define TASK_PAYLOAD_MGR_STACK_SIZE 1024U
#define TASK_PAYLOAD_MGR_NAME "payload_mgr"

#define TASK_TIMEKEEPER_STACK_SIZE 1024U
#define TASK_TIMEKEEPER_NAME "timekeeper"

#define TASK_SW_WATCHDOG_STACK_SIZE 128U
#define TASK_SW_WATCHDOG_NAME "sw_watchdog"

#define TASK_ALARM_MGR_STACK_SIZE 512U
#define TASK_ALARM_MGR_NAME "alarm_handler"

#define TASK_HEALTH_COLLECTOR_STACK_SIZE 256U
#define TASK_HEALTH_COLLECTOR_NAME "health_collector"

#define TASK_STATS_COLLECTOR_STACK_SIZE 1024U
#define TASK_STATS_COLLECTOR_NAME "stats_collector"

#define OBC_SCHEDULER_MAX_PRIORITY configMAX_PRIORITIES - 1U

/* TYPEDEFS */
typedef enum {
  OBC_TASK_PRIORITY_IDLE = 0U,

  OBC_TASK_PRIORITY_COMMAND_MGR = 1U,
  OBC_TASK_PRIORITY_TELEMETRY_MGR = 1U,
  OBC_TASK_PRIORITY_HEALTH_COLLECTOR = 1U,
  OBC_TASK_PRIORITY_PAYLOAD_MGR = 1U,
  OBC_TASK_PRIORITY_COMMS = 2U,
  OBC_TASK_PRIORITY_COMMS_MGR = OBC_TASK_PRIORITY_COMMS,
  OBC_TASK_PRIORITY_COMMS_UPLINK_DECODE = OBC_TASK_PRIORITY_COMMS,
  OBC_TASK_PRIORITY_COMMS_DOWNLINK_ENCODE = OBC_TASK_PRIORITY_COMMS,
  OBC_TASK_PRIORITY_EPS_MGR = 3U,
  OBC_TASK_PRIORITY_STATS_COLLECTOR = 4U,
  OBC_TASK_PRIORITY_ALARM_MGR = 4U,
  OBC_TASK_PRIORITY_STATE_MGR = 5U,
  OBC_TASK_PRIORITY_TIMEKEEPER = 6U,
  OBC_TASK_PRIORITY_SW_WATCHDOG = OBC_SCHEDULER_MAX_PRIORITY,

  OBC_TASK_PRIORITY_MAX = OBC_SCHEDULER_MAX_PRIORITY,
} obc_scheduler_task_priority_t;

/* TASK FUNCTION PROTOTYPES */
extern void obcTaskFunctionStateMgr(void *params);
extern void obcTaskFunctionTelemetryMgr(void *params);
extern void obcTaskFunctionCommandMgr(void *params);
extern void obcTaskFunctionCommsMgr(void *params);
extern void obcTaskFunctionCommsDownlinkEncoder(void *params);
extern void obcTaskFunctionCommsUplinkDecoder(void *params);
extern void obcTaskFunctionEpsMgr(void *params);
extern void obcTaskFunctionPayloadMgr(void *params);
extern void obcTaskFunctionTimekeeper(void *params);
extern void obcTaskFunctionSwWatchdog(void *params);
extern void obcTaskFunctionAlarmMgr(void *params);
extern void obcTaskFunctionHealthCollector(void *params);
extern void obcTaskFunctionStatsCollector(void *params);

/* PRIVATE DATA */
static StackType_t obcTaskStackStateMgr[TASK_STATE_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferStateMgr;

static StackType_t obcTaskStackTelemetryMgr[TASK_TELEMETRY_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferTelemetryMgr;

static StackType_t obcTaskStackCommandMgr[TASK_COMMAND_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferCommandMgr;

static StackType_t obcTaskStackCommsMgr[TASK_COMMS_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferCommsMgr;

static StackType_t obcTaskStackCommsDownlinkEncoder[TASK_COMMS_DOWNLINK_ENCODER_STACK_SIZE];
static StaticTask_t obcTaskBufferCommsDownlinkEncoder;

static StackType_t obcTaskStackCommsUplinkDecoder[TASK_COMMS_UPLINK_DECODER_STACK_SIZE];
static StaticTask_t obcTaskBufferCommsUplinkDecoder;

static StackType_t obcTaskStackEpsMgr[TASK_EPS_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferEpsMgr;

static StackType_t obcTaskStackPayloadMgr[TASK_PAYLOAD_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferPayloadMgr;

static StackType_t obcTaskStackTimekeeper[TASK_TIMEKEEPER_STACK_SIZE];
static StaticTask_t obcTaskBufferTimekeeper;

static StackType_t obcTaskStackSwWatchdog[TASK_SW_WATCHDOG_STACK_SIZE];
static StaticTask_t obcTaskBufferSwWatchdog;

static StackType_t obcTaskStackAlarmMgr[TASK_ALARM_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferAlarmMgr;

static StackType_t obcTaskStackHealthCollector[TASK_HEALTH_COLLECTOR_STACK_SIZE];
static StaticTask_t obcTaskBufferHealthCollector;

#if ENABLE_TASK_STATS_COLLECTOR == 1
static StackType_t obcTaskStackTaskStatsCollector[TASK_STATS_COLLECTOR_STACK_SIZE];
static StaticTask_t obcTaskBufferTaskStatsCollector;
#endif

static obc_scheduler_config_t obcSchedulerConfig[] = {
    [OBC_SCHEDULER_TASK_ID_STATE_MGR] =
        {
            .taskName = TASK_STATE_MGR_NAME,
            .taskStack = obcTaskStackStateMgr,
            .taskBuffer = &obcTaskBufferStateMgr,
            .stackSize = TASK_STATE_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_STATE_MGR,
            .taskFunc = obcTaskFunctionStateMgr,
        },
    [OBC_SCHEDULER_TASK_ID_TELEMETRY_MGR] =
        {
            .taskName = TASK_TELEMETRY_MGR_NAME,
            .taskStack = obcTaskStackTelemetryMgr,
            .taskBuffer = &obcTaskBufferTelemetryMgr,
            .stackSize = TASK_TELEMETRY_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_TELEMETRY_MGR,
            .taskFunc = obcTaskFunctionTelemetryMgr,
        },
    [OBC_SCHEDULER_TASK_ID_COMMAND_MGR] =
        {
            .taskName = TASK_COMMAND_MGR_NAME,
            .taskStack = obcTaskStackCommandMgr,
            .taskBuffer = &obcTaskBufferCommandMgr,
            .stackSize = TASK_COMMAND_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_COMMAND_MGR,
            .taskFunc = obcTaskFunctionCommandMgr,
        },
    [OBC_SCHEDULER_TASK_ID_COMMS_MGR] =
        {
            .taskName = TASK_COMMS_MGR_NAME,
            .taskStack = obcTaskStackCommsMgr,
            .taskBuffer = &obcTaskBufferCommsMgr,
            .stackSize = TASK_COMMS_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_COMMS_MGR,
            .taskFunc = obcTaskFunctionCommsMgr,
        },
    [OBC_SCHEDULER_TASK_ID_COMMS_DOWNLINK_ENCODER] =
        {
            .taskName = TASK_COMMS_DOWNLINK_ENCODER_NAME,
            .taskStack = obcTaskStackCommsDownlinkEncoder,
            .taskBuffer = &obcTaskBufferCommsDownlinkEncoder,
            .stackSize = TASK_COMMS_DOWNLINK_ENCODER_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_COMMS_DOWNLINK_ENCODE,
            .taskFunc = obcTaskFunctionCommsDownlinkEncoder,
        },
    [OBC_SCHEDULER_TASK_ID_COMMS_UPLINK_DECODER] =
        {
            .taskName = TASK_COMMS_UPLINK_DECODER_NAME,
            .taskStack = obcTaskStackCommsUplinkDecoder,
            .taskBuffer = &obcTaskBufferCommsUplinkDecoder,
            .stackSize = TASK_COMMS_UPLINK_DECODER_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_COMMS_UPLINK_DECODE,
            .taskFunc = obcTaskFunctionCommsUplinkDecoder,
        },
    [OBC_SCHEDULER_TASK_ID_EPS_MGR] =
        {
            .taskName = TASK_EPS_MGR_NAME,
            .taskStack = obcTaskStackEpsMgr,
            .taskBuffer = &obcTaskBufferEpsMgr,
            .stackSize = TASK_EPS_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_EPS_MGR,
            .taskFunc = obcTaskFunctionEpsMgr,
        },
    [OBC_SCHEDULER_TASK_ID_PAYLOAD_MGR] =
        {
            .taskName = TASK_PAYLOAD_MGR_NAME,
            .taskStack = obcTaskStackPayloadMgr,
            .taskBuffer = &obcTaskBufferPayloadMgr,
            .stackSize = TASK_PAYLOAD_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_PAYLOAD_MGR,
            .taskFunc = obcTaskFunctionPayloadMgr,
        },
    [OBC_SCHEDULER_TASK_ID_TIMEKEEPER] =
        {
            .taskName = TASK_TIMEKEEPER_NAME,
            .taskStack = obcTaskStackTimekeeper,
            .taskBuffer = &obcTaskBufferTimekeeper,
            .stackSize = TASK_TIMEKEEPER_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_TIMEKEEPER,
            .taskFunc = obcTaskFunctionTimekeeper,
        },
    [OBC_SCHEDULER_TASK_ID_SW_WATCHDOG] =
        {
            .taskName = TASK_SW_WATCHDOG_NAME,
            .taskStack = obcTaskStackSwWatchdog,
            .taskBuffer = &obcTaskBufferSwWatchdog,
            .stackSize = TASK_SW_WATCHDOG_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_SW_WATCHDOG,
            .taskFunc = obcTaskFunctionSwWatchdog,
        },
    [OBC_SCHEDULER_TASK_ID_ALARM_HANDLER] =
        {
            .taskName = TASK_ALARM_MGR_NAME,
            .taskStack = obcTaskStackAlarmMgr,
            .taskBuffer = &obcTaskBufferAlarmMgr,
            .stackSize = TASK_ALARM_MGR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_ALARM_MGR,
            .taskFunc = obcTaskFunctionAlarmMgr,
        },
    [OBC_SCHEDULER_TASK_ID_HEALTH_COLLECTOR] =
        {
            .taskName = TASK_HEALTH_COLLECTOR_NAME,
            .taskStack = obcTaskStackHealthCollector,
            .taskBuffer = &obcTaskBufferHealthCollector,
            .stackSize = TASK_HEALTH_COLLECTOR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_HEALTH_COLLECTOR,
            .taskFunc = obcTaskFunctionHealthCollector,
        },

#if ENABLE_TASK_STATS_COLLECTOR == 1
    [OBC_SCHEDULER_TASK_ID_TASK_STATS_COLLECTOR] =
        {
            .taskName = TASK_STATS_COLLECTOR_NAME,
            .taskStack = obcTaskStackTaskStatsCollector,
            .taskBuffer = &obcTaskBufferTaskStatsCollector,
            .stackSize = TASK_STATS_COLLECTOR_STACK_SIZE,
            .priority = OBC_TASK_PRIORITY_STATS_COLLECTOR,
            .taskFunc = obcTaskFunctionStatsCollector,
        },
#endif

};

/* PUBLIC FUNCTION DEFINITIONS */
obc_scheduler_config_t *obcSchedulerGetConfig(obc_scheduler_task_id_t taskID) {
  ASSERT(taskID > 0 && taskID < OBC_SCHEDULER_TASK_COUNT);
  return &obcSchedulerConfig[taskID];
}

void obcSchedulerCreateTask(obc_scheduler_task_id_t taskID) {
  ASSERT(taskID > 0 && taskID < OBC_SCHEDULER_TASK_COUNT);
  obc_scheduler_config_t *taskConfig = obcSchedulerGetConfig(taskID);

  ASSERT(taskConfig->taskStack != NULL);
  ASSERT(taskConfig->taskBuffer != NULL);

  taskConfig->taskHandle = xTaskCreateStatic(taskConfig->taskFunc, taskConfig->taskName, taskConfig->stackSize, NULL,
                                             taskConfig->priority, taskConfig->taskStack, taskConfig->taskBuffer);
}
