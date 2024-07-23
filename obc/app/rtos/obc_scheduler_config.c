#include "obc_scheduler_config.h"

#include "obc_errors.h"
#include "obc_assert.h"

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <os_task.h>
#include <sys_common.h>

#include <stdint.h>

/* DEFINES */
// Task names
#define TASK_STATE_MGR_NAME "state_mgr"
#define TASK_TELEMETRY_MGR_NAME "telemetry_mgr"
#define TASK_COMMAND_MGR_NAME "command_mgr"
#define TASK_COMMS_MGR_NAME "comms_mgr"
#define TASK_COMMS_DOWNLINK_ENCODER_NAME "comms_encoder"
#define TASK_COMMS_UPLINK_DECODER_NAME "comms_decoder"
#define TASK_EPS_MGR_NAME "eps_mgr"
#define TASK_PAYLOAD_MGR_NAME "payload_mgr"
#define TASK_TIMEKEEPER_NAME "timekeeper"
#define TASK_DIGITAL_WATCHDOG_MGR_NAME "digital_wdg_mgr"
#define TASK_ALARM_MGR_NAME "alarm_handler"
#define TASK_HEALTH_COLLECTOR_NAME "health_collector"
#define TASK_STATS_COLLECTOR_NAME "stats_collector"
#define TASK_LOGGER_NAME "logger"
#define TASK_GNC_MGR_NAME "gnc_mgr"

// Task stack sizes in words
#define TASK_STATE_MGR_STACK_SIZE 1024U
#define TASK_TELEMETRY_MGR_STACK_SIZE 1024U
#define TASK_COMMAND_MGR_STACK_SIZE 1024U
#define TASK_COMMS_MGR_STACK_SIZE 1024U
#define TASK_COMMS_DOWNLINK_ENCODER_STACK_SIZE 512U
#define TASK_COMMS_UPLINK_DECODER_STACK_SIZE 2048U
#define TASK_EPS_MGR_STACK_SIZE 1024U
#define TASK_PAYLOAD_MGR_STACK_SIZE 1024U
#define TASK_TIMEKEEPER_STACK_SIZE 1024U
#define TASK_DIGITAL_WATCHDOG_MGR_STACK_SIZE 128U
#define TASK_ALARM_MGR_STACK_SIZE 512U
#define TASK_HEALTH_COLLECTOR_STACK_SIZE 256U
#define TASK_STATS_COLLECTOR_STACK_SIZE 1024U
#define TASK_LOGGER_STACK_SIZE 512U
#define TASK_GNC_MGR_STACK_SIZE 512U

// All task priorities must be in [0, OBC_SCHEDULER_MAX_PRIORITY]
#define OBC_SCHEDULER_MAX_PRIORITY configMAX_PRIORITIES - 1U
#define TASK_IDLE_PRIORITY 0U
#define TASK_COMMAND_MGR_PRIORITY 1U
#define TASK_TELEMETRY_MGR_PRIORITY 1U
#define TASK_HEALTH_COLLECTOR_PRIORITY 1U
#define TASK_PAYLOAD_MGR_PRIORITY 1U
#define TASK_COMMS_PRIORITY 2U  // Comms tasks must have the same priority
#define TASK_COMMS_MGR_PRIORITY TASK_COMMS_PRIORITY
#define TASK_COMMS_UPLINK_DECODE_PRIORITY TASK_COMMS_PRIORITY
#define TASK_COMMS_DOWNLINK_ENCODE_PRIORITY TASK_COMMS_PRIORITY
#define TASK_EPS_MGR_PRIORITY 3U
#define TASK_GNC_MGR_PRIORITY 3U
#define TASK_STATS_COLLECTOR_PRIORITY 4U
#define TASK_ALARM_MGR_PRIORITY 4U
#define TASK_STATE_MGR_PRIORITY 5U
#define TASK_LOGGER_PRIORITY 5U
#define TASK_TIMEKEEPER_PRIORITY 6U
#define TASK_DIGITAL_WATCHDOG_MGR_PRIORITY OBC_SCHEDULER_MAX_PRIORITY

/* TYPEDEFS */
typedef struct {
  TaskHandle_t *taskHandle;
  StaticTask_t *taskBuffer;
  StackType_t *taskStack;
  uint32_t stackSize;
  uint32_t priority;
  const char *taskName;
  void (*taskFunc)(void *);
  void (*taskInit)(void);  // Optional
} obc_scheduler_config_t;

/* TASK INIT PROTOTYPES - These run before their respective tasks are created */
extern void obcTaskInitStateMgr(void);
extern void obcTaskInitTelemetryMgr(void);
extern void obcTaskInitCommandMgr(void);
extern void obcTaskInitCommsMgr(void);
extern void obcTaskInitCommsDownlinkEncoder(void);
extern void obcTaskInitCommsUplinkDecoder(void);
extern void obcTaskInitEpsMgr(void);
extern void obcTaskInitPayloadMgr(void);
extern void obcTaskInitTimekeeper(void);
extern void obcTaskInitAlarmMgr(void);
extern void obcTaskInitHealthCollector(void);
extern void obcTaskInitStatsCollector(void);
extern void obcTaskInitLogger(void);
extern void obcTaskInitGncMgr(void);

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
extern void obcTaskFunctionLogger(void *params);
extern void obcTaskFunctionGncMgr(void *params);

/* PRIVATE FUNCTION PROTOTYPES */
static obc_scheduler_config_t *obcSchedulerGetConfig(obc_scheduler_config_id_t taskID);

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

static StackType_t obcTaskStackSwWatchdog[TASK_DIGITAL_WATCHDOG_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferSwWatchdog;

static StackType_t obcTaskStackAlarmMgr[TASK_ALARM_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferAlarmMgr;

static StackType_t obcTaskStackHealthCollector[TASK_HEALTH_COLLECTOR_STACK_SIZE];
static StaticTask_t obcTaskBufferHealthCollector;

static StackType_t obcTaskStackLogger[TASK_LOGGER_STACK_SIZE];
static StaticTask_t obcTaskBufferLogger;

static StackType_t obcTaskStackGncMgr[TASK_GNC_MGR_STACK_SIZE];
static StaticTask_t obcTaskBufferGncMgr;

#if ENABLE_TASK_STATS_COLLECTOR == 1
static StackType_t obcTaskStackTaskStatsCollector[TASK_STATS_COLLECTOR_STACK_SIZE];
static StaticTask_t obcTaskBufferTaskStatsCollector;
#endif

static obc_scheduler_config_t obcSchedulerConfig[] = {
    [OBC_SCHEDULER_CONFIG_ID_STATE_MGR] =
        {
            .taskName = TASK_STATE_MGR_NAME,
            .taskStack = obcTaskStackStateMgr,
            .taskBuffer = &obcTaskBufferStateMgr,
            .stackSize = TASK_STATE_MGR_STACK_SIZE,
            .priority = TASK_STATE_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionStateMgr,
            .taskInit = obcTaskInitStateMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_TELEMETRY_MGR] =
        {
            .taskName = TASK_TELEMETRY_MGR_NAME,
            .taskStack = obcTaskStackTelemetryMgr,
            .taskBuffer = &obcTaskBufferTelemetryMgr,
            .stackSize = TASK_TELEMETRY_MGR_STACK_SIZE,
            .priority = TASK_TELEMETRY_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionTelemetryMgr,
            .taskInit = obcTaskInitTelemetryMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_COMMAND_MGR] =
        {
            .taskName = TASK_COMMAND_MGR_NAME,
            .taskStack = obcTaskStackCommandMgr,
            .taskBuffer = &obcTaskBufferCommandMgr,
            .stackSize = TASK_COMMAND_MGR_STACK_SIZE,
            .priority = TASK_COMMAND_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionCommandMgr,
            .taskInit = obcTaskInitCommandMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_COMMS_MGR] =
        {
            .taskName = TASK_COMMS_MGR_NAME,
            .taskStack = obcTaskStackCommsMgr,
            .taskBuffer = &obcTaskBufferCommsMgr,
            .stackSize = TASK_COMMS_MGR_STACK_SIZE,
            .priority = TASK_COMMS_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionCommsMgr,
            .taskInit = obcTaskInitCommsMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_COMMS_DOWNLINK_ENCODER] =
        {
            .taskName = TASK_COMMS_DOWNLINK_ENCODER_NAME,
            .taskStack = obcTaskStackCommsDownlinkEncoder,
            .taskBuffer = &obcTaskBufferCommsDownlinkEncoder,
            .stackSize = TASK_COMMS_DOWNLINK_ENCODER_STACK_SIZE,
            .priority = TASK_COMMS_DOWNLINK_ENCODE_PRIORITY,
            .taskFunc = obcTaskFunctionCommsDownlinkEncoder,
            .taskInit = obcTaskInitCommsDownlinkEncoder,
        },
    [OBC_SCHEDULER_CONFIG_ID_COMMS_UPLINK_DECODER] =
        {
            .taskName = TASK_COMMS_UPLINK_DECODER_NAME,
            .taskStack = obcTaskStackCommsUplinkDecoder,
            .taskBuffer = &obcTaskBufferCommsUplinkDecoder,
            .stackSize = TASK_COMMS_UPLINK_DECODER_STACK_SIZE,
            .priority = TASK_COMMS_UPLINK_DECODE_PRIORITY,
            .taskFunc = obcTaskFunctionCommsUplinkDecoder,
            .taskInit = obcTaskInitCommsUplinkDecoder,
        },
    [OBC_SCHEDULER_CONFIG_ID_EPS_MGR] =
        {
            .taskName = TASK_EPS_MGR_NAME,
            .taskStack = obcTaskStackEpsMgr,
            .taskBuffer = &obcTaskBufferEpsMgr,
            .stackSize = TASK_EPS_MGR_STACK_SIZE,
            .priority = TASK_EPS_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionEpsMgr,
            .taskInit = obcTaskInitEpsMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_PAYLOAD_MGR] =
        {
            .taskName = TASK_PAYLOAD_MGR_NAME,
            .taskStack = obcTaskStackPayloadMgr,
            .taskBuffer = &obcTaskBufferPayloadMgr,
            .stackSize = TASK_PAYLOAD_MGR_STACK_SIZE,
            .priority = TASK_PAYLOAD_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionPayloadMgr,
            .taskInit = obcTaskInitPayloadMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_TIMEKEEPER] =
        {
            .taskName = TASK_TIMEKEEPER_NAME,
            .taskStack = obcTaskStackTimekeeper,
            .taskBuffer = &obcTaskBufferTimekeeper,
            .stackSize = TASK_TIMEKEEPER_STACK_SIZE,
            .priority = TASK_TIMEKEEPER_PRIORITY,
            .taskFunc = obcTaskFunctionTimekeeper,
            .taskInit = obcTaskInitTimekeeper,
        },
    [OBC_SCHEDULER_CONFIG_ID_DIGITAL_WATCHDOG_MGR] =
        {
            .taskName = TASK_DIGITAL_WATCHDOG_MGR_NAME,
            .taskStack = obcTaskStackSwWatchdog,
            .taskBuffer = &obcTaskBufferSwWatchdog,
            .stackSize = TASK_DIGITAL_WATCHDOG_MGR_STACK_SIZE,
            .priority = TASK_DIGITAL_WATCHDOG_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionSwWatchdog,
            .taskInit = NULL,
        },
    [OBC_SCHEDULER_CONFIG_ID_ALARM_MGR] =
        {
            .taskName = TASK_ALARM_MGR_NAME,
            .taskStack = obcTaskStackAlarmMgr,
            .taskBuffer = &obcTaskBufferAlarmMgr,
            .stackSize = TASK_ALARM_MGR_STACK_SIZE,
            .priority = TASK_ALARM_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionAlarmMgr,
            .taskInit = obcTaskInitAlarmMgr,
        },
    [OBC_SCHEDULER_CONFIG_ID_HEALTH_COLLECTOR] =
        {
            .taskName = TASK_HEALTH_COLLECTOR_NAME,
            .taskStack = obcTaskStackHealthCollector,
            .taskBuffer = &obcTaskBufferHealthCollector,
            .stackSize = TASK_HEALTH_COLLECTOR_STACK_SIZE,
            .priority = TASK_HEALTH_COLLECTOR_PRIORITY,
            .taskFunc = obcTaskFunctionHealthCollector,
            .taskInit = obcTaskInitHealthCollector,
        },
    [OBC_SCHEDULER_CONFIG_ID_LOGGER] =
        {
            .taskName = TASK_LOGGER_NAME,
            .taskStack = obcTaskStackLogger,
            .taskBuffer = &obcTaskBufferLogger,
            .stackSize = TASK_LOGGER_STACK_SIZE,
            .priority = TASK_LOGGER_PRIORITY,
            .taskFunc = obcTaskFunctionLogger,
            .taskInit = obcTaskInitLogger,
        },
    [OBC_SCHEDULER_CONFIG_ID_GNC_MGR] =
        {
            .taskName = TASK_GNC_MGR_NAME,
            .taskStack = obcTaskStackGncMgr,
            .taskBuffer = &obcTaskBufferGncMgr,
            .stackSize = TASK_GNC_MGR_STACK_SIZE,
            .priority = TASK_GNC_MGR_PRIORITY,
            .taskFunc = obcTaskFunctionGncMgr,
            .taskInit = obcTaskInitGncMgr,
        },

#if ENABLE_TASK_STATS_COLLECTOR == 1
    [OBC_SCHEDULER_CONFIG_ID_STATS_COLLECTOR] =
        {
            .taskName = TASK_STATS_COLLECTOR_NAME,
            .taskStack = obcTaskStackTaskStatsCollector,
            .taskBuffer = &obcTaskBufferTaskStatsCollector,
            .stackSize = TASK_STATS_COLLECTOR_STACK_SIZE,
            .priority = TASK_STATS_COLLECTOR_PRIORITY,
            .taskFunc = obcTaskFunctionStatsCollector,
            .taskInit = obcTaskInitStatsCollector,
        },
#endif

};

STATIC_ASSERT_EQ(sizeof(obcSchedulerConfig) / sizeof(obc_scheduler_config_t), OBC_SCHEDULER_TASK_COUNT);

/* PUBLIC FUNCTION DEFINITIONS */
void obcSchedulerCreateTask(obc_scheduler_config_id_t taskID) { obcSchedulerCreateTaskWithArgs(taskID, NULL); }

void obcSchedulerCreateTaskWithArgs(obc_scheduler_config_id_t taskID, void *args) {
  obc_scheduler_config_t *taskConfig = obcSchedulerGetConfig(taskID);
  ASSERT(taskConfig != NULL);

  const bool taskConfigExists =
      (taskConfig->taskFunc != NULL) && (taskConfig->taskStack != NULL) && (taskConfig->taskBuffer != NULL);

  if (taskConfigExists) {
    taskConfig->taskHandle = xTaskCreateStatic(taskConfig->taskFunc, taskConfig->taskName, taskConfig->stackSize, args,
                                               taskConfig->priority, taskConfig->taskStack, taskConfig->taskBuffer);
  }
}

void obcSchedulerInitTask(obc_scheduler_config_id_t taskID) {
  obc_scheduler_config_t *taskConfig = obcSchedulerGetConfig(taskID);

  ASSERT(taskConfig != NULL);

  if (taskConfig->taskInit != NULL) {
    taskConfig->taskInit();
  }
}

/* PRIVATE FUNCTION DEFINITIONS */
static obc_scheduler_config_t *obcSchedulerGetConfig(obc_scheduler_config_id_t taskID) {
  if (taskID >= OBC_SCHEDULER_TASK_COUNT) return NULL;
  return &obcSchedulerConfig[taskID];
}
