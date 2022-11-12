#include "health_mgr.h"


static health_status_t healthStatusCode;

static health_param_t paramStore[NUM_HEALTH_PARAMS];

/**
 * @brief	HealthMgr task.
 * @param	pvParameters	Task parameters.
 */
static void vHealthMgrTask(void* pvParameters);


void initHealthMgr(void) {
    healthStatusCode = HEALTH_STATUS_NORMAL;

}

void heathParamSetExpected(health_param_t* param) {
    uint32_t exp = 0;
    param->expected = exp;
}

void heathParamSetValue(health_param_t* param) {
    uint32_t val = 0;
    param->value = val;
}

uint8_t heathVerifParam(health_param_t* param) {
    uint8_t result = 0;     // do it like this instead of return in case, if we want to indicate 0 success or 1 success
    switch (param->verif) {
        case HEALTH_PARAM_VERIF_EQUALS:
            result = param->value == param->expected;
            break;
        case HEALTH_PARAM_VERIF_GREATER_THAN:
            result = param->value > param->expected;
            break;
        case HEALTH_PARAM_VERIF_LESS_THAN:
            result = param->value < param->expected;
            break;
        default:
            // some debug should handle every case
            break;
    }
    return result;
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

static void vHealthMgrTask(void* pvParameters) {
    while (1) {
        // some logic here to set the health status 
        healthMgrTakeAction();
    }
}
