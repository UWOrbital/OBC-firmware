#include "hbridge.h"
#include <stdlib.h>

/*
@brief initializes motor with default configurations set in HalCoGen
    Default config is duty cycle of 50% and period of 1000ns; TCLK = 110MHz
@param motor: pointer to motor struct
*/
obc_error_code_t InitMotor(const DCMotor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  hetInit();
  return OBC_ERR_CODE_SUCCESS;
}


/*
@brief Starts the PWM channels.
@param motor: pointer to motor struct
*/
obc_error_code_t StartMotor(const DCMotor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  pwmStart(motor->hetBase, motor->FinPWM);
  pwmStart(motor->hetBase, motor->RinPWM);
  return OBC_ERR_CODE_SUCCESS;
}

/*
@brief Drives the motor at the specified duty cycle and period.
@param motor: pointer to motor struct.
@param duty: The duty cycle give as a int32_t casted downt to uint32 with considerations for sign. 
@param period: The PWM period in us. 
*/
obc_error_code_t DriveMotorPWM(const DCMotor_t* motor, int32_t duty, float period) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (abs(duty) > 100) duty = 100;

  hetSIGNAL_t signal = {
    .duty = duty,
    .period = period
  };
  
  if (duty >= 0) {
    pwmSetSignal(motor->hetBase, motor->FinPWM, signal);
    pwmSetDuty(motor->hetBase, motor->RinPWM, MOTOR_DRIVE_LOW);
  } else {
    pwmSetDuty(motor->hetBase, motor->FinPWM, MOTOR_DRIVE_LOW);
    pwmSetSignal(motor->hetBase, motor->RinPWM, signal);
  }
  return OBC_ERR_CODE_SUCCESS;

}

/*
@brief Drives the motor at the specified speed and period.
@param motor: pointer to motor struct.
@param speed: The signed speed value at which the motor should be run. Set to maxiumum torque for the motor
if it greater than that in magnitude. 
@param period: The PWM period in us. 
*/
obc_error_code_t DriveMotorTorque(const DCMotor_t* motor, float speed, float period) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;

  uint32 duty = (uint32)  (abs(speed/motor->maxTorque) * 100);
  if (duty > 100) duty = 100;

  hetSIGNAL_t signal = {
    .duty = duty,
    .period = period
  };
  
  if (speed >= 0) {
    pwmSetSignal(motor->hetBase, motor->FinPWM, signal);
    pwmSetDuty(motor->hetBase, motor->RinPWM, MOTOR_DRIVE_LOW);
  } else {
    pwmSetDuty(motor->hetBase, motor->FinPWM, MOTOR_DRIVE_LOW);
    pwmSetSignal(motor->hetBase, motor->RinPWM, signal);
  }
  return OBC_ERR_CODE_SUCCESS;
}

/*
@brief Drives the motor clockwise at MOTOR_OPERATE_FR (%) duty cycle
@param motor: pointer to motor struct. 
*/
obc_error_code_t ForwardMotor(const DCMotor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  pwmSetDuty(motor->hetBase, motor->FinPWM, MOTOR_OPERATE_FR);
  pwmSetDuty(motor->hetBase, motor->RinPWM, MOTOR_DRIVE_LOW);
  return OBC_ERR_CODE_SUCCESS;
}

/*
@brief Drives the motor counterclockwise at MOTOR_OPERATE_FR (%) duty cycle
@param motor: pointer to motor struct. 
*/
obc_error_code_t ReverseMotor(const DCMotor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  pwmSetDuty(motor->hetBase, motor->FinPWM, MOTOR_DRIVE_LOW);
  pwmSetDuty(motor->hetBase, motor->RinPWM, MOTOR_OPERATE_FR);
  return OBC_ERR_CODE_SUCCESS;
}

/* 
@brief Sets PWM signal to 100, setting the motor to HIGH until signal is changed.
@param DC_Motor: pointer to motor struct.
*/
obc_error_code_t BrakeMotor(const DCMotor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  pwmSetDuty(motor->hetBase, motor->FinPWM, MOTOR_DRIVE_HIGH);
  pwmSetDuty(motor->hetBase, motor->RinPWM, MOTOR_DRIVE_HIGH);
  return OBC_ERR_CODE_SUCCESS;
}

/*
@brief Sets the motor to idle mode. 
@param motor: pointer to motor struct. 
*/
obc_error_code_t IdleMotor(const DCMotor_t* motor) {
  if (motor == NULL) return OBC_ERR_CODE_INVALID_ARG;
  pwmStop(motor->hetBase, motor->FinPWM);
  pwmStop(motor->hetBase, motor->RinPWM);
  return OBC_ERR_CODE_SUCCESS;
}


