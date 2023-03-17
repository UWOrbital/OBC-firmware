#ifndef COMMON_INCLUDE_OBC_STATES_H_
#define COMMON_INCLUDE_OBC_STATES_H_

#include "obc_errors.h"

// Must be less than 256 states
typedef enum {
    OBC_STATE_INITIALIZING,
    OBC_STATE_NORMAL,
    OBC_STATE_LOW_POWER,
} obc_state_t;

/**
 * @brief Change the current state of the OBC
 * 
 * @param newState The new state of the OBC
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 * @warning This function is not thread safe. It should only be called from the supervisor task.
 */
obc_error_code_t changeStateOBC(obc_state_t newState);

#endif /* COMMON_INCLUDE_OBC_STATES_H_ */