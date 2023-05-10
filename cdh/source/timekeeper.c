#include "timekeeper.h"
#include "obc_time.h"
#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_task_config.h"
#include "obc_persist.h"
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
    obc_error_code_t errCode;

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

    fram_data_timekeeper_t persist;
    LOG_IF_ERROR_CODE(getPersistTimekeeper(&persist));
    
    rtc_date_time_t datetime;
    if (errCode == OBC_ERR_CODE_SUCCESS) {
        LOG_IF_ERROR_CODE(unixToDatetime(persist.unixTime, &datetime));
        LOG_IF_ERROR_CODE(setCurrentDateTimeRTC(&datetime));
    } else {
        // TODO: If time in FRAM invalid, figure out what to do
        LOG_IF_ERROR_CODE(setCurrentDateTimeRTC(&(rtc_date_time_t) {
            .date.year = 23,
            .date.month = 4,
            .date.date = 1,
            .time.hours = 12,
            .time.minutes = 30,
            .time.seconds = 30
        }));
    }

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
        
        uint32_t currTime = getCurrentUnixTime();
        LOG_DEBUG("Current time: %lu", currTime);

        LOG_IF_ERROR_CODE(setPersistTimekeeper(&(fram_data_timekeeper_t) {
            .unixTime = currTime
        }));

        fram_data_timekeeper_t persist;
        LOG_IF_ERROR_CODE(getPersistTimekeeper(&persist));
        LOG_DEBUG("Persisted time: %lu", persist.unixTime);

        syncPeriodCounter = (syncPeriodCounter + 1) % LOCAL_TIME_SYNC_PERIOD_S;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
