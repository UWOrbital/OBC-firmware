#pragma once

#include "obc_errors.h"
#include "het.h"

#define MOTOR_DRIVE_HIGH 100
#define MOTOR_OPERATE_FR 50
#define MOTOR_DRIVE_LOW 0


/*
@brief DCMotor_t struct used to hold the motor's pwm modules and which HET driver its running on
@param hetBase: pointer to HET RAM
    hetRAM1: HET1 RAM pointer
    hetRAM2: HET2 RAM pointer 
@param FinPWM: pointer to pwm driver that controls FIN (Forward pin of motor driver) 
@param RinPWM: pointer to pwm driver that controls RIN (Reverse pin of motor driver)
    pwm0: Pwm 0
    pwm1: Pwm 1
    pwm2: Pwm 2
    pwm3: Pwm 3
    pwm4: Pwm 4
    pwm5: Pwm 5
    pwm6: Pwm 6
    pwm7: Pwm 7
  @param  maxTorque: // The torque produced at 100% duty cycle. 
*/
typedef struct {
  hetRAMBASE_t* hetBase;
  uint32 FinPWM, RinPWM;
  const float maxTorque;  
} DCMotor_t;

obc_error_code_t InitMotor(const DCMotor_t* motor);
obc_error_code_t StartMotor(const DCMotor_t* motor);
obc_error_code_t DriveMotorPWM(const DCMotor_t* motor, int32_t speed, float period);
obc_error_code_t DriveMotorTorque(const DCMotor_t* motor, float speed, float period);
obc_error_code_t ForwardMotor(const DCMotor_t* motor);
obc_error_code_t ReverseMotor(const DCMotor_t* motor);
obc_error_code_t BrakeMotor(const DCMotor_t* motor);
obc_error_code_t IdleMotor(const DCMotor_t* motor);