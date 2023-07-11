#include "obc_errors.h"
#include "obc_logging.h"
#include "gio.h"
#include "max5360.h"

#define RFFM6404_TR_PIN_NUM 6U
#define RFFM6404_EN_PIN_NUM 7U
#define RFFM6404_BYP_PIN_NUM 2U

#define RFFM6404_TR_PIN_PORT gioPORTA
#define RFFM6404_EN_PIN_PORT gioPORTA
#define RFFM6404_BYP_PIN_PORT gioPORTB

/**
 * @brief powers on the RFFE chip in receive mode
 *
 * @return obc_err_code_t - whether or not the chip was successfully turned on
 */
obc_error_code_t rffm6404ActivateRx(void) {
  RETURN_IF_ERROR_CODE(max5360PowerOff());
  gioSetBit(RFFM6404_TR_PIN_PORT, RFFM6404_TR_PIN_NUM, 0);
  gioSetBit(RFFM6404_EN_PIN_PORT, RFFM6404_EN_PIN_NUM, 1);
  gioSetBit(RFFM6404_BYP_PIN_PORT, RFFM6404_BYP_PIN_NUM, 0);
}

/**
 * @brief powers on the RFFE chip in transmit mode
 *
 * @param voltagePowerControl voltage value to use for VAPC to control power
 *
 * @return obc_err_code_t - whether or not the chip was successfully turned on
 */
obc_error_code_t rff6404ActivateTx(float voltagePowerControl) {
  RETURN_IF_ERROR_CODE(max5360WriteVoltage(voltagePowerControl));
  gioSetBit(RFFM6404_TR_PIN_PORT, RFFM6404_TR_PIN_NUM, 1);
  gioSetBit(RFFM6404_EN_PIN_PORT, RFFM6404_EN_PIN_NUM, 1);
  gioSetBit(RFFM6404_BYP_PIN_PORT, RFFM6404_BYP_PIN_NUM, 0);
}

/**
 * @brief from receive mode, activate bypass mode to turn of LNA
 *
 * @return obc_err_code_t - whether or not the operation was successful
 */
obc_error_code_t rffm6404ActivateRecvByp(void) {
  RETURN_IF_ERROR_CODE(max5360PowerOff());
  gioSetBit(RFFM6404_TR_PIN_PORT, RFFM6404_TR_PIN_NUM, 0);
  gioSetBit(RFFM6404_EN_PIN_PORT, RFFM6404_EN_PIN_NUM, 1);
  gioSetBit(RFFM6404_BYP_PIN_PORT, RFFM6404_BYP_PIN_NUM, 1);
}

/**
 * @brief powers off the RFFE chip
 *
 * @return obc_err_code_t - whether or not the power off was successful
 */
obc_error_code_t rffm6404PowerOff(void) {
  RETURN_IF_ERROR_CODE(max5360PowerOff())
  gioSetBit(RFFM6404_EN_PIN_PORT, RFFM6404_EN_PIN_NUM, 0);
}