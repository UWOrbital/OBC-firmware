#include "obc_states.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_assert.h"
#include "obc_time.h"
#include "obc_persistent_store.h"
#include "telemetry_manager.h"

static obc_state_t currStateOBC;

// This function is not thread safe. It should only be called from the supervisor task.
obc_error_code_t changeStateOBC(obc_state_t newState) {
  obc_error_code_t errCode;

  currStateOBC = newState;
  telemetry_data_t telemData = {.id = TELEM_OBC_STATE, .timestamp = getCurrentUnixTime(), .obcState = currStateOBC};
  RETURN_IF_ERROR_CODE(addTelemetryData(&telemData));

  obc_state_persist_data_t persist = {.obcState = newState};
  RETURN_IF_ERROR_CODE(setPersistentObcState(&persist));

  return OBC_ERR_CODE_SUCCESS;
}
