#pragma once

#include "obc_errors.h"

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Set the GNC task period. Will be used for when we want to run GNC tasks more or less frequently (QEYnet
 * pointing, attitude adjustments)
 * @param periodMs period to set the GNC periodic task time to. Note that periodMS can be set to any value between
 * 0-50ms, but can only be increased by intervals of 50ms (e.g 100ms, 150ms, 200ms, ...)
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */
obc_error_code_t setGncTaskPeriod(uint16_t periodMs);

/**
 * @brief Start the magnetorquer commanding test. Safe to call from any task
 * (used by the CMD_GNC_MTQ_TEST_START command callback).
 *
 * The GNC task then repeats this cycle indefinitely, taking over the actuators
 * from the normal GNC pipeline:
 *   1. ramp the MTQ duty cycle from 0 %% to +75 %% (forward polarity)
 *   2. hold +75 %% for 10 seconds
 *   3. flip instantly to reverse polarity at 75 %% and ramp back down to 0 %%
 *   4. flip instantly back to forward polarity and start again from step 1
 *
 * @return OBC_ERR_CODE_SUCCESS
 */
obc_error_code_t startGncMtqTest(void);

/**
 * @brief Stop the magnetorquer commanding test (CMD_GNC_MTQ_TEST_STOP command
 * callback). The GNC task puts the MTQ into standby (both bridge inputs low)
 * on its next cycle and resumes the normal GNC pipeline.
 *
 * @return OBC_ERR_CODE_SUCCESS
 */
obc_error_code_t stopGncMtqTest(void);
