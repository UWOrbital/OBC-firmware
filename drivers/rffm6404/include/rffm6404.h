pragma #once

#include "obc_errors.h"

    /**
     * @brief powers on the RFFE chip in receive mode
     *
     * @return obc_err_code_t - whether or not the chip was successfully turned on
     */
    obc_error_code_t
    rffm6404ActivateRx(void);

/**
 * @brief powers on the RFFE chip in transmit mode
 *
 * @param voltagePowerControl voltage value to use for VAPC to control power
 *
 * @return obc_err_code_t - whether or not the chip was successfully turned on
 */
obc_error_code_t rff6404ActivateTx(float voltagePowerControl);

/**
 * @brief from receive mode, activate bypass mode to turn of LNA
 *
 * @return obc_err_code_t - whether or not the operation was successful
 */
obc_error_code_t rffm6404ActivateRecvByp(void);

/**
 * @brief powers off the RFFE chip
 *
 * @return obc_err_code_t - whether or not the power off was successful
 */
obc_error_code_t rffm6404PowerOff(void);
