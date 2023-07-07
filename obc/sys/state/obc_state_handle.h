#pragma once

#include "obc_errors.h"

#include <stdint.h>

/**
 * @brief Change the current state of the OBC
 *
 * @param newState The new state of the OBC
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 * @warning This function is not thread safe. It should only be called from the supervisor task.
 */
obc_error_code_t changeStateOBC(uint8_t newState);
