#pragma once

#include "obc_errors.h"
#include "het.h"

#include <stdint.h>

/**
 * @file bd621x.h
 * @brief Magnetorquer (MTQ) driver for the ROHM BD621x H-bridge (BD6210F-E2 on ADCS Rev 1).
 *
 * Each magnetorquer coil is driven by one BD621x H-bridge with two logic inputs:
 * FIN (forward) and RIN (reverse). The RM46 generates a PWM on one input while
 * holding the other low; the sign of the requested output selects the direction.
 *
 *   FIN   RIN   | Bridge state
 *   ------------+----------------------------------------------
 *   PWM   LOW   | Forward drive (current OUT1 -> OUT2)
 *   LOW   PWM   | Reverse drive (current OUT2 -> OUT1)
 *   LOW   LOW   | Standby / coast (outputs high-Z, coil current decays)
 *   HIGH  HIGH  | Brake (both outputs low, coil shorted) - not used by this driver
 *
 * ---------------------------------------------------------------------------
 * HOW TO CONFIGURE AN mtq_t CORRECTLY
 * ---------------------------------------------------------------------------
 * 1) Pick two PWM channels (pwm0..pwm7, defined in het.h). The channel -> N2HET
 *    pin mapping is baked into the HALCoGen-generated HET program and CANNOT be
 *    changed at runtime. To find the mapping for your board's HAL, run:
 *
 *        grep -A5 'PWCNT: PWM' obc/shared/hal/<board>/source/het.c | grep -E 'PWCNT|Pin'
 *
 *    For the RM46 LaunchPad HAL this gives:
 *        pwm0 -> N2HET1[8]    pwm1 -> N2HET1[10]   pwm2 -> N2HET1[12]
 *        pwm3 -> N2HET1[14]   pwm4 -> N2HET1[16]   pwm5 -> N2HET1[17]
 *        pwm6 -> N2HET1[18]   pwm7 -> N2HET1[19]
 *    On the LaunchPad only pwm0-4 and pwm6 reach header pins (the pinmux routes
 *    N2HET1[17]/[19] balls to MIBSPI functions). Header locations:
 *        pwm0: J8-3   pwm1: J8-1   pwm2: J4-6   pwm3: J4-5   pwm4: J4-3   pwm6: J4-2
 *
 * 2) Fill in finPin/rinPin with the N2HET pin numbers from step 1 (e.g. 12 for
 *    pwm2). The driver needs them because hetInit() leaves every N2HET pin
 *    configured as an INPUT (hetREG->DIR == 0), and an input pin is never
 *    driven by the HET program: mtqInit() sets the direction bits itself.
 *
 * 3) hetRam/hetReg must refer to the same HET core: hetRAM1 with hetREG1, or
 *    hetRAM2 with hetREG2. (Only HET1 is initialized by hetInit() in this HAL.)
 *
 * 4) Call order: hetInit() once at boot, then mtqInit() per magnetorquer, then
 *    mtqSetOutput() as needed. Note the N2HET clock domain (VCLK2) requires the
 *    re-latch workaround in mapClocks() - see the MTQ PWM bring-up report.
 *
 * ---------------------------------------------------------------------------
 * PWM FREQUENCY AND RESOLUTION
 * ---------------------------------------------------------------------------
 * The BD621x accepts PWM input frequencies of 20-100 kHz (datasheet p.11). This
 * driver uses a fixed 50 us period (20 kHz, MTQ_PWM_PERIOD_US). With the HET
 * loop-resolution tick of ~1.75 us (128 / 73.33 MHz VCLK2) a 50 us period has
 * ~28 steps, so the effective duty resolution is ~3.5 %. Raising the frequency
 * reduces resolution proportionally.
 *
 * ---------------------------------------------------------------------------
 * RELATING DUTY CYCLE TO COIL CURRENT / DIPOLE MOMENT
 * ---------------------------------------------------------------------------
 * ADCS Rev 1 has no current-sense routing, so current is open-loop:
 *     I_avg ~= duty * V_supply / (R_coil + 2 * 0.05 ohm + R_on(bridge))
 * valid when the coil L/R time constant >> 50 us; otherwise characterize
 * I-vs-duty once on the bench. Dipole moment follows as m = N * I * A.
 *
 * Datasheet: https://fscdn.rohm.com/en/products/databook/datasheet/ic/motor/dc/bd621x-e.pdf
 */

/** @brief Fixed PWM period in microseconds (20 kHz; BD621x allows 20-100 kHz). */
#define MTQ_PWM_PERIOD_US 20.0

/**
 * @struct mtq_t
 * @brief One magnetorquer channel (one BD621x H-bridge).
 *
 * See the configuration guide at the top of this file for how to fill this in.
 *
 * @param hetRam  HET RAM of the core running the PWM program (hetRAM1/hetRAM2).
 * @param hetReg  Register frame of the SAME core (hetREG1/hetREG2); used to set
 *                the pin direction bits.
 * @param finPwm  PWM channel (pwm0..pwm7) wired to the BD621x FIN input.
 * @param rinPwm  PWM channel (pwm0..pwm7) wired to the BD621x RIN input.
 * @param finPin  N2HET pin number driven by finPwm (from the het.c PWCNT table).
 * @param rinPin  N2HET pin number driven by rinPwm.
 */
typedef struct {
  hetRAMBASE_t* hetRam;
  hetBASE_t* hetReg;
  uint32_t finPwm;
  uint32_t rinPwm;
  uint32_t finPin;
  uint32_t rinPin;
} mtq_t;

/**
 * @brief Initialize one magnetorquer channel: validate the config, set the two
 *        N2HET pins as outputs, start both PWM channels and force 0 % output
 *        (standby - both bridge inputs low).
 *
 * Must be called after hetInit() and before any mtqSetOutput() call.
 *
 * @param mtq Pointer to the magnetorquer config (see struct docs).
 * @return OBC_ERR_CODE_SUCCESS, or OBC_ERR_CODE_INVALID_ARG if mtq is NULL or
 *         any field is invalid (bad HET core pointers, pwm > pwm7, pin > 31,
 *         or FIN and RIN sharing a channel/pin).
 */
obc_error_code_t mtqInit(const mtq_t* mtq);

/**
 * @brief Drive the magnetorquer at a signed duty cycle.
 *
 * Positive duty -> PWM on FIN (forward polarity), RIN held low.
 * Negative duty -> PWM on RIN (reverse polarity), FIN held low.
 * Zero          -> both low (standby/coast; coil current decays through the
 *                  bridge body diodes).
 *
 * The average coil voltage is approximately duty * V_supply; see the file
 * header for converting duty to current and dipole moment.
 *
 * @param mtq         Pointer to an initialized magnetorquer config.
 * @param dutyPercent Signed duty cycle, -100..+100. 100 %% drives the pin
 *                    constantly high (no switching edges - this is normal).
 * @return OBC_ERR_CODE_SUCCESS, or OBC_ERR_CODE_INVALID_ARG if mtq is invalid
 *         or |dutyPercent| > 100.
 */
obc_error_code_t mtqSetOutput(const mtq_t* mtq, int32_t dutyPercent);

/**
 * @brief Stop driving the coil: both bridge inputs low (standby/coast).
 *
 * Equivalent to mtqSetOutput(mtq, 0). The PWM channels stay running at 0 %
 * duty so a later mtqSetOutput() takes effect immediately.
 *
 * @param mtq Pointer to an initialized magnetorquer config.
 * @return OBC_ERR_CODE_SUCCESS, or OBC_ERR_CODE_INVALID_ARG if mtq is invalid.
 */
obc_error_code_t mtqStop(const mtq_t* mtq);
