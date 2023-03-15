#include "obc_states.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_assert.h"
#include "telemetry.h"

static obc_state_t currStateOBC;

obc_error_code_t changeStateOBC(obc_state_t newState) {
    obc_error_code_t errCode;
    
    // TODO: Get current time instead of 0
    currStateOBC = newState;
    telemetry_data_t telemData = {.id = TELEM_OBC_STATE, .timestamp = 0, .obcState = currStateOBC};
    
    RETURN_IF_ERROR_CODE(addTelemetryData(&telemData));
    return OBC_ERR_CODE_SUCCESS;
}