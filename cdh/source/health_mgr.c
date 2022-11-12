#include "health_mgr.h"


static uint8_t healthStatusCode;

/**
 * @brief	HealthMgr task.
 * @param	pvParameters	Task parameters.
 */
static void vHealthMgrTask(void * pvParameters);


void initHealthMgr(void) {
    healthStatusCode = 0;
    
}

void healthMgrTakeAction(void) {
    switch (healthStatusCode) {
    case HEALTH_STATUS_NORMAL:
        // do nothing
        break;
    case HEALTH_STATUS_BAD1:
        // send a command to do something
        break;
    case HEALTH_STATUS_BAD2:
        // do another thing
        break;
    default:
        // some debug should handle every case
        break;
    }
}

static void vHealthMgrTask(void * pvParameters) {
    while (1) {

        healthMgrTakeAction();
    }
}