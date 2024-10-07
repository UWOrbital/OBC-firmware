#ifndef CONFIG_BD621X
#include "obc_errors.h"
#include "obc_general_util.h"
#include "bd621x.h"

obc_error_code_t startMotor(const DC_motor_t* motor) {
  UNUSED(motor);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setMotorSpeed(const DC_motor_t* motor, float speed, float64 period) {
  UNUSED(motor);
  UNUSED(speed);
  UNUSED(period);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t brakeMotor(const DC_motor_t* motor) {
  UNUSED(motor);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t idleMotor(const DC_motor_t* motor) {
  UNUSED(motor);
  return OBC_ERR_CODE_SUCCESS;
}

#endif  // CONFIG_BD621X
