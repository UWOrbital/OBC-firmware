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

#define LOCAL_TIME_SYNC_PERIOD_S        60UL

/**
 * @brief Timekeeper task.
 */
static void timekeeperTask(void *pvParameters);

static TaskHandle_t timekeeperTaskHandle;
static StaticTask_t timekeeperTaskBuffer;
static StackType_t timekeeperTaskStack[TIMEKEEPER_STACK_SIZE];

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

    initTime();
}

static void timekeeperTask(void *pvParameters) {
    /* 
     * The timekeeper is a task instead of a FreeRTOS timer because we have more control
     * over its priority relative to other tasks (inc. the time service daemon). The sync period is
     * often enough (and the MCU clock speed is high enough) that we don't need to 
     * worry about significant drift.
     * 
     * The I2C clock speed is high enough that, if another task is completing an I2C transaction
     * when the timekeeper task is performing a local RTC sync, the timekeeper task should not have to 
     * wait a significant amount of time.
     */

    obc_error_code_t errCode;

    uint8_t syncPeriodCounter = 0;  // Sync whenever this counter is 0

    while (1) {
        if (syncPeriodCounter == 0) {
            // Sync the local time with the RTC every LOCAL_TIME_SYNC_PERIOD_S seconds.
            // TODO: Deal with errors
            LOG_IF_ERROR_CODE(syncUnixTime());
        } else {
            incrementCurrentUnixTime();
        }
        
        LOG_DEBUG("Current time: %lu", getCurrentUnixTime());

        syncPeriodCounter = (syncPeriodCounter + 1) % LOCAL_TIME_SYNC_PERIOD_S;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
