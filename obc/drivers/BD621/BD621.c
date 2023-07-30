#include "BD621.h"
#include <stdlib.h>
#include "het.h"

static bool isValidPwm(uint32_t pwm);
static bool isValidHetBase(hetRAMBASE_t* hetRam);
static bool isValidMotorParameters(const DC_motor_t* motor);

obc_error_code_t startMotor(const DC_motor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;

  pwmStart(motor->hetBase, motor->finPwm);
  pwmStart(motor->hetBase, motor->rinPwm);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t driveMotorPWM(const DC_motor_t* motor, int32_t duty, float64 period) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;

  float64 frequency = 1 / period;
  if ((frequency < MOTOR_MIN_FREQUENCY) || (frequency > MOTOR_MAX_FREQUENCY)) return OBC_ERR_CODE_INVALID_ARG;

  if (abs(duty) > 100) duty = 100;
  hetSIGNAL_t signal = {.duty = duty, .period = period};

  if (duty >= 0) {
    pwmSetSignal(motor->hetBase, motor->finPwm, signal);
    pwmSetDuty(motor->hetBase, motor->rinPwm, MOTOR_DRIVE_LOW);
  } else {
    pwmSetDuty(motor->hetBase, motor->finPwm, MOTOR_DRIVE_LOW);
    pwmSetSignal(motor->hetBase, motor->rinPwm, signal);
  }
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t driveMotorTorque(const DC_motor_t* motor, float speed, float64 period) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;
  if (motor->maxSpeed < MOTOR_SPEED_LOWER_BOUND) return OBC_ERR_CODE_INVALID_ARG;

  float64 frequency = 1 / period;
  if ((frequency < MOTOR_MIN_FREQUENCY) || (frequency > MOTOR_MAX_FREQUENCY)) return OBC_ERR_CODE_INVALID_ARG;

  int32_t duty = (int32_t)(abs(speed / motor->maxSpeed) * 100);
  if (duty > 100) duty = 100;
  if (speed <= 0) duty = -duty;

  return driveMotorPWM(motor, duty, period);
}

obc_error_code_t forwardMotor(const DC_motor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;

  pwmSetDuty(motor->hetBase, motor->finPwm, MOTOR_OPERATE_FR);
  pwmSetDuty(motor->hetBase, motor->rinPwm, MOTOR_DRIVE_LOW);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t reverseMotor(const DC_motor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;

  pwmSetDuty(motor->hetBase, motor->finPwm, MOTOR_DRIVE_LOW);
  pwmSetDuty(motor->hetBase, motor->rinPwm, MOTOR_OPERATE_FR);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t brakeMotor(const DC_motor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;

  pwmSetDuty(motor->hetBase, motor->finPwm, MOTOR_DRIVE_HIGH);
  pwmSetDuty(motor->hetBase, motor->rinPwm, MOTOR_DRIVE_HIGH);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t idleMotor(const DC_motor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (!isValidMotorParameters(motor)) return OBC_ERR_CODE_INVALID_ARG;

  pwmStop(motor->hetBase, motor->finPwm);
  pwmStop(motor->hetBase, motor->rinPwm);
  return OBC_ERR_CODE_SUCCESS;
}

static bool isValidPwm(uint32_t pwm) {
  return ((pwm == pwm0) || (pwm == pwm1) || (pwm == pwm2) || (pwm == pwm3) || (pwm == pwm4) || (pwm == pwm5) ||
          (pwm == pwm6) || (pwm == pwm7));
}
static bool isValidHetBase(hetRAMBASE_t* hetRam) { return (hetRam == hetRAM1 || hetRam == hetRAM2); }

static bool isValidMotorParameters(const DC_motor_t* motor) {
  return (isValidPwm(motor->finPwm) && isValidPwm(motor->rinPwm) && isValidHetBase(motor->hetBase));
}
