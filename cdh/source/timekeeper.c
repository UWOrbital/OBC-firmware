#include "timekeeper.h"
#include "obc_time.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_task_config.h"
#include "ds3232_mz.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_timer.h>
#include <sys_common.h>

#define LOCAL_TIME_INC_PERIOD_TICKS     pdMS_TO_TICKS(1000)
#define LOCAL_TIME_UPDATE_TIMER_NAME    "local_time_update_timer"
#define LOCAL_TIME_SYNC_PERIOD_S        60UL

STATIC_ASSERT(LOCAL_TIME_INC_PERIOD_TICKS == pdMS_TO_TICKS(1000), 
    "Global time update period must be 1 second. Otherwise, update the time sync period.");

/**
 * @brief Timekeeper task.
 */
static void timekeeperTask(void *pvParameters);

/**
 * @brief Callback for the global time update timer.
 */
static void globalTimeUpdateCallback(TimerHandle_t xTimer);

static TaskHandle_t timekeeperTaskHandle;
static StaticTask_t timekeeperTaskBuffer;
static StackType_t timekeeperTaskStack[TIMEKEEPER_STACK_SIZE];

static TimerHandle_t globalTimeUpdateHandle;
static StaticTimer_t globalTimeUpdateBuffer;

void initTimekeeper(void) {
    memset(&timekeeperTaskBuffer, 0, sizeof(timekeeperTaskBuffer));
    memset(&timekeeperTaskStack, 0, sizeof(timekeeperTaskStack));

    ASSERT(timekeeperTaskStack != NULL && &timekeeperTaskBuffer != NULL);
    timekeeperTaskHandle = xTaskCreateStatic(
        timekeeperTask,
        TIMEKEEPER_NAME,
        TIMEKEEPER_STACK_SIZE,
        NULL,
        TIMEKEEPER_PRIORITY,
        timekeeperTaskStack,
        &timekeeperTaskBuffer
    );

    ASSERT(&globalTimeUpdateBuffer != NULL);
    globalTimeUpdateHandle = xTimerCreateStatic(
        LOCAL_TIME_UPDATE_TIMER_NAME,
        LOCAL_TIME_INC_PERIOD_TICKS,
        pdTRUE,
        (void *) 0,
        globalTimeUpdateCallback,
        &globalTimeUpdateBuffer
    );
}

static void timekeeperTask(void *pvParameters) {
    obc_error_code_t errCode;

    // TODO: Deal with error code
    LOG_IF_ERROR_CODE(syncUnixTime());

    xTimerStart(globalTimeUpdateHandle, LOCAL_TIME_INC_PERIOD_TICKS);

    while (1);
}

static void globalTimeUpdateCallback(TimerHandle_t xTimer) {
    obc_error_code_t errCode;

    uint32_t callCount = (uint32_t) pvTimerGetTimerID(xTimer);

    // Sync the local time with the RTC every LOCAL_TIME_SYNC_PERIOD_S seconds.
    if (callCount == 0) {
        LOG_IF_ERROR_CODE(syncUnixTime());

        // TODO: If we failed to sync, increment local time and the 
        // number of times we've failed to sync.
    } else {
        incrementCurrentUnixTime();
    }

    callCount = (callCount + 1) % LOCAL_TIME_SYNC_PERIOD_S;

    vTimerSetTimerID(xTimer, (void *) callCount);
}
