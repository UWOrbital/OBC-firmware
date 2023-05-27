#include "lm75bd.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "telemetry_manager.h"
#include "obc_sci_io.h"

#include <os_projdefs.h>
#include <FreeRTOS.h>
#include <os_timer.h>
#include <sys_common.h>
#include <obc_time.h>

static TimerHandle_t tempReaderTimerHandler;
StaticTimer_t tempReaderTimerBuffer;

void vTimerCallback( TimerHandle_t xTimer ) {
    obc_error_code_t errCode;

    lm75bd_config_t config = {
        .devAddr = LM75BD_OBC_I2C_ADDR,
        .devOperationMode = LM75BD_DEV_OP_MODE_NORMAL,
        .osFaultQueueSize = 2,
        .osPolarity = LM75BD_OS_POL_ACTIVE_LOW,
        .osOperationMode = LM75BD_OS_OP_MODE_COMP,
        .devOperationMode = LM75BD_DEV_OP_MODE_NORMAL,
        .overTempThresholdCelsius = 100.0f,
        .hysteresisThresholdCelsius = 50.0f,
    };

    errCode = lm75bdInit(&config);
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        sciPrintf("Error initializing LM75BD: %d\r\n", errCode);
    }

    float temp;
    errCode = readTempLM75BD(LM75BD_OBC_I2C_ADDR, &temp);

    telemetry_data_t obcTempVal = {
        .obcTemp = temp,
        .id = TELEM_OBC_TEMP,
        .timestamp = getCurrentUnixTime()
    };

    addTelemetryData(&obcTempVal);
}

void initTempReaderHandler(void) {
    ASSERT(tempReaderTimerHandler != NULL);
    tempReaderTimerHandler = xTimerCreateStatic(
        "temp read timer",
        /*Read temp every minute according to notion*/
        pdMS_TO_TICKS(60000),
        pdTRUE,
        (void*) 0,
        vTimerCallback,
        &tempReaderTimerBuffer
    );
}
