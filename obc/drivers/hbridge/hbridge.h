#pragma once

#include "obc_errors.h"
#include "het.h"

#define MOTOR_DRIVE_HIGH 100
#define MOTOR_OPERATE_FR 50
#define MOTOR_DRIVE_LOW 0

typedef struct {
  hetRAMBASE_t* hetBase;
  uint32 FinPWM, RinPWM;
  const float maxTorque;   // The torque produced at 100% duty cycle. 
} DCMotor_t;

obc_error_code_t InitMotor(const DCMotor_t* motor);
obc_error_code_t StartMotor(const DCMotor_t* motor);
obc_error_code_t DriveMotorPWM(const DCMotor_t* motor, int64_t speed, float period);
obc_error_code_t DriveMotorTorque(const DCMotor_t* motor, float speed, float period);
obc_error_code_t ForwardMotor(const DCMotor_t* motor);
obc_error_code_t ReverseMotor(const DCMotor_t* motor);
obc_error_code_t BrakeMotor(const DCMotor_t* motor);
obc_error_code_t IdleMotor(const DCMotor_t* motor);