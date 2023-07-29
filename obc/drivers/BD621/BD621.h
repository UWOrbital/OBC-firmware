#pragma once

#include "obc_errors.h"
#include "het.h"

#define MOTOR_DRIVE_HIGH 100
#define MOTOR_OPERATE_FR 50
#define MOTOR_DRIVE_LOW 0


/**
* @struct DCMotor_t 
* @brief Struct used to hold the motor's pwm modules and which HET driver its running on
* @param hetBase: pointer to HET RAM
    hetRAM1: HET1 RAM pointer
    hetRAM2: HET2 RAM pointer 
* @param FinPWM: pointer to pwm driver that controls FIN (Forward pin of motor driver) 
* @param RinPWM: pointer to pwm driver that controls RIN (Reverse pin of motor driver)
    pwm0: Pwm 0
    pwm1: Pwm 1
    pwm2: Pwm 2
    pwm3: Pwm 3
    pwm4: Pwm 4
    pwm5: Pwm 5
    pwm6: Pwm 6
    pwm7: Pwm 7
* @param  maxTorque: The torque produced at 100% duty cycle. 
**/
typedef struct {
  hetRAMBASE_t* hetBase;
  uint32 FinPWM, RinPWM;
  const float maxTorque;  
} DCMotor_t;

/**
* @brief Starts the PWM channels.
* @param motor: pointer to motor struct
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL.
**/
obc_error_code_t StartMotor(const DCMotor_t* motor);

/**
* @brief Drives the motor at the specified duty cycle and period.
* @param motor: pointer to motor struct.
* @param duty: The duty cycle give as a int32_t casted downt to uint32 with considerations for sign. 
* @param period: The PWM period in us. 
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL.
**/
obc_error_code_t DriveMotorPWM(const DCMotor_t* motor, int32_t speed, float period);

/**
* @brief Drives the motor at the specified speed and period.
* @param motor: pointer to motor struct.
* @param speed: The signed speed value at which the motor should be run. Set to maxiumum torque for the motor
if it greater than that in magnitude. 
* @param period: The PWM period in us. 
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL.
**/
obc_error_code_t DriveMotorTorque(const DCMotor_t* motor, float speed, float period);

/**
* @brief Drives the motor clockwise at MOTOR_OPERATE_FR (%) duty cycle
* @param motor: pointer to motor struct. 
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL.
*/
obc_error_code_t ForwardMotor(const DCMotor_t* motor);

/**
* @brief Drives the motor counterclockwise at MOTOR_OPERATE_FR (%) duty cycle
* @param motor: pointer to motor struct.
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL. 
**/
obc_error_code_t ReverseMotor(const DCMotor_t* motor);

/** 
* @brief Sets PWM signal to 100, setting the motor to HIGH until signal is changed.
* @param DC_Motor: pointer to motor struct.
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL.
**/
obc_error_code_t BrakeMotor(const DCMotor_t* motor);

/**
* @brief Sets the motor to idle mode.  
* @param motor: pointer to motor struct. 
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL.
**/
obc_error_code_t IdleMotor(const DCMotor_t* motor);
