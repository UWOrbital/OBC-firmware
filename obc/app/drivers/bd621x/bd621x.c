#include "bd621x.h"
#include "obc_errors.h"

#include <het.h>
#include <stdbool.h>
#include <stdlib.h>

// Datasheet: https://fscdn.rohm.com/en/products/databook/datasheet/ic/motor/dc/bd621x-e.pdf

#define MTQ_DUTY_MAX 100
#define MTQ_DUTY_OFF 0U
#define N2HET_NUM_PINS 32U

// BD621x PWM input frequency limits, datasheet p.11
#define MTQ_MIN_PWM_FREQ_HZ 20000.0
#define MTQ_MAX_PWM_FREQ_HZ 100000.0

// The fixed period must stay inside the BD621x limits; checked at compile time
// so MTQ_PWM_PERIOD_US can't silently drift out of spec.
_Static_assert((1000000.0 / MTQ_PWM_PERIOD_US) >= MTQ_MIN_PWM_FREQ_HZ, "MTQ PWM frequency below BD621x minimum");
_Static_assert((1000000.0 / MTQ_PWM_PERIOD_US) <= MTQ_MAX_PWM_FREQ_HZ, "MTQ PWM frequency above BD621x maximum");

static bool isValidMtq(const mtq_t* mtq) {
  if (mtq == NULL) {
    return false;
  }

  // hetRam and hetReg must refer to the same N2HET core
  bool validCore =
      (mtq->hetRam == hetRAM1 && mtq->hetReg == hetREG1) || (mtq->hetRam == hetRAM2 && mtq->hetReg == hetREG2);

  bool validChannels = (mtq->finPwm <= pwm7) && (mtq->rinPwm <= pwm7) && (mtq->finPwm != mtq->rinPwm);

  bool validPins = (mtq->finPin < N2HET_NUM_PINS) && (mtq->rinPin < N2HET_NUM_PINS) && (mtq->finPin != mtq->rinPin);

  return validCore && validChannels && validPins;
}

obc_error_code_t mtqInit(const mtq_t* mtq) {
  if (!isValidMtq(mtq)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  // hetInit() leaves every N2HET pin as an input (DIR = 0); an input pin is
  // never driven by the HET program, so the PWM would run without reaching
  // the pin. Make both bridge-input pins outputs.
  mtq->hetReg->DIR |= ((uint32)1U << mtq->finPin) | ((uint32)1U << mtq->rinPin);

  pwmStart(mtq->hetRam, mtq->finPwm);
  pwmStart(mtq->hetRam, mtq->rinPwm);

  // Start in standby (both bridge inputs low)
  return mtqStop(mtq);
}

obc_error_code_t mtqSetOutput(const mtq_t* mtq, int32_t dutyPercent) {
  if (!isValidMtq(mtq)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  if (abs(dutyPercent) > MTQ_DUTY_MAX) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  hetSIGNAL_t signal = {.duty = (uint32)abs(dutyPercent), .period = MTQ_PWM_PERIOD_US};

  if (dutyPercent >= 0) {
    // Forward: PWM on FIN, RIN low
    pwmSetSignal(mtq->hetRam, mtq->finPwm, signal);
    pwmSetDuty(mtq->hetRam, mtq->rinPwm, MTQ_DUTY_OFF);
  } else {
    // Reverse: PWM on RIN, FIN low
    pwmSetDuty(mtq->hetRam, mtq->finPwm, MTQ_DUTY_OFF);
    pwmSetSignal(mtq->hetRam, mtq->rinPwm, signal);
  }

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t mtqStop(const mtq_t* mtq) { return mtqSetOutput(mtq, 0); }
