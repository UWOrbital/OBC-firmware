#pragma once

#include "obc_errors.h"
#include "het.h"

// Datasheet reference: https://fscdn.rohm.com/en/products/databook/datasheet/ic/motor/dc/bd621x-e.pdf

#define MOTOR_DRIVE_HIGH 100          // 100% duty cycle, indiicating high signal.
#define MOTOR_DRIVE_LOW 0             // 0% duty cycle, indiicating low signal.
#define MOTOR_SPEED_LOWER_BOUND 0.01  // Arbitrarily chosen lower bound for motor speed.
#define MOTOR_MAX_FREQUENCY 100000    // Max frequency allowed for the brushless motor,  Pg. 11 in the datasheet.
#define MOTOR_MIN_FREQUENCY 20000     // Min frequency allowed for the brushless motor, Pg. 11 in the datasheet.

#define MOTOR_MAX_DUTY 100  // The maximum duty cycle allowed in percent.
/**
* @struct DC_motor_t
* @brief Struct used to hold the motor's pwm modules and which HET driver its running on
* @param hetBase: pointer to HET RAM
    hetRAM1: HET1 RAM pointer
    hetRAM2: HET2 RAM pointer
* @param finPwm: pwm driver that controls FIN (Forward pin of motor driver)
* @param rinPwm: pwm driver that controls RIN (Reverse pin of motor driver)
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
  uint32_t finPwm, rinPwm;
  const float maxSpeed;
} DC_motor_t;

/**
 * @brief Starts the PWM channels.
 * @param motor: pointer to motor struct
 * @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL
 * or DC_motor_t struct parameters are invalid.
 **/
obc_error_code_t startMotor(const DC_motor_t* motor);

/**
* @brief Drives the motor at the specified speed and period.
* @param motor: pointer to motor struct.
* @param speed: The signed speed value at which the motor should be run. Set to maxiumum torque for the motor
if it greater than that in magnitude. Same units as maxSpeed.
* @param period: The PWM period in us.
* @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL
 * or DC_motor_t struct parameters are invalid.
*/
obc_error_code_t driveMotorTorque(const DC_motor_t* motor, float speed, float64 period);

/**
 * @brief Sets PWM signal to 100, setting the motor to HIGH until signal is changed.
 * @param DC_Motor: pointer to motor struct.
 * @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL
 * or DC_motor_t struct parameters are invalid.
 **/
obc_error_code_t brakeMotor(const DC_motor_t* motor);

/**
 * @brief Sets the motor to idle mode.
 * @param motor: pointer to motor struct.
 * @return Returns OBC_ERR_CODE_SUCCESS if successful, OBC_ERR_CODE_INVALID_ARG if motor is a pointer to NULL
 * or DC_motor_t struct parameters are invalid.
 **/
obc_error_code_t idleMotor(const DC_motor_t* motor);
