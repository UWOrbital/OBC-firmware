#include "ephermeris.h"

#include <os_semphr.h>
#include <FreeRTOS.h>
#include <string.h>

// Only 1 task can access the manager at a time
static SemaphoreHandle_t managerMutex = NULL;
static StaticSemaphore_t managerBuffer;

// Only 1 task can access the file at a time
static SemaphoreHandle_t fileMutex = NULL;
static StaticSemaphore_t fileBuffer;

static uint8_t initialized = 0;  // There's probably a better way to do this
static position_data_manager_t manager;

void initSunPosition(void) {
    if (managerMutex == NULL) {
        managerMutex = xSemaphoreCreateBinaryStatic(&managerBuffer);
    }

    ASSERT(managerMutex != NULL);

    if (fileMutex == NULL) {
        fileMutex = xSemaphoreCreateBinaryStatic(&fileBuffer);
    }

    ASSERT(fileMutex != NULL);

    if (!initialized) {
        ASSERT(sunManagerInit(&manager) == OBC_ERR_CODE_SUCCESS);
        initialized = 1;
    }
}   

obc_error_code_t sunPositionGet(julian_date_t jd, position_data_t *buffer) {
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}

obc_error_code_t sunPositionNext(position_data_t *buffer) {
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}

obc_error_code_t sunPositionShiftTo(julian_date_t jd) {
    return OBC_ERR_CODE_SUN_POSITION_NOT_IMPLEMENTED;
}
