/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: attitude_control.c
 *
 * Code generated for Simulink model 'attitude_control'.
 *
 * Model version                  : 3.78
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Mon Jan  1 12:47:01 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-R
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "attitude_control.h"
#include "rtwtypes.h"
#include <stddef.h>
#define NumBitsPerChar                 8U

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;
static real_T rtGetNaN(void);
static real32_T rtGetNaNF(void);

#define NOT_USING_NONFINITE_LITERALS   1

extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
static void rt_InitInfAndNaN(size_t realSize);
static boolean_T rtIsInf(real_T value);
static boolean_T rtIsInfF(real32_T value);
static boolean_T rtIsNaN(real_T value);
static boolean_T rtIsNaNF(real32_T value);
typedef struct {
  struct {
    uint32_T wordH;
    uint32_T wordL;
  } words;
} BigEndianIEEEDouble;

typedef struct {
  struct {
    uint32_T wordL;
    uint32_T wordH;
  } words;
} LittleEndianIEEEDouble;

typedef struct {
  union {
    real32_T wordLreal;
    uint32_T wordLuint;
  } wordL;
} IEEESingle;

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;
static real_T rtGetInf(void);
static real32_T rtGetInfF(void);
static real_T rtGetMinusInf(void);
static real32_T rtGetMinusInfF(void);

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetNaN(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetNaNF(void)
{
  IEEESingle nanF = { { 0.0F } };

  nanF.wordL.wordLuint = 0xFFC00000U;
  return nanF.wordL.wordLreal;
}

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
static void rt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
static boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
static boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
static boolean_T rtIsNaN(real_T value)
{
  boolean_T result = (boolean_T) 0;
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  if (bitsPerReal == 32U) {
    result = rtIsNaNF((real32_T)value);
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.fltVal = value;
    result = (boolean_T)((tmpVal.bitVal.words.wordH & 0x7FF00000) == 0x7FF00000 &&
                         ( (tmpVal.bitVal.words.wordH & 0x000FFFFF) != 0 ||
                          (tmpVal.bitVal.words.wordL != 0) ));
  }

  return result;
}

/* Test if single-precision value is not a number */
static boolean_T rtIsNaNF(real32_T value)
{
  IEEESingle tmp;
  tmp.wordL.wordLreal = value;
  return (boolean_T)( (tmp.wordL.wordLuint & 0x7F800000) == 0x7F800000 &&
                     (tmp.wordL.wordLuint & 0x007FFFFF) != 0 );
}

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetMinusInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/* Model step function */
void attitude_control_step(void)
{
  real_T rtb_Product1;
  real_T rtb_Product2;
  real_T rtb_Product2_l;
  real_T rtb_Sign;
  real_T rtb_Sum_b;

  /* Sum: '<S5>/Sum' incorporates:
   *  Inport: '<Root>/com_quat_body'
   *  Product: '<S5>/Product'
   *  Product: '<S5>/Product1'
   *  Product: '<S5>/Product2'
   *  Product: '<S5>/Product3'
   */
  rtb_Product2_l = ((rtU.com_quat_body[0] * rtU.com_quat_body[0] +
                     rtU.com_quat_body[1] * rtU.com_quat_body[1]) +
                    rtU.com_quat_body[2] * rtU.com_quat_body[2]) +
    rtU.com_quat_body[3] * rtU.com_quat_body[3];

  /* Product: '<S2>/Divide' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  rtb_Sum_b = rtU.com_quat_body[0] / rtb_Product2_l;

  /* Product: '<S2>/Divide1' incorporates:
   *  Inport: '<Root>/com_quat_body'
   *  UnaryMinus: '<S4>/Unary Minus'
   */
  rtb_Product1 = -rtU.com_quat_body[1] / rtb_Product2_l;

  /* Product: '<S2>/Divide2' incorporates:
   *  Inport: '<Root>/com_quat_body'
   *  UnaryMinus: '<S4>/Unary Minus1'
   */
  rtb_Product2 = -rtU.com_quat_body[2] / rtb_Product2_l;

  /* Product: '<S2>/Divide3' incorporates:
   *  Inport: '<Root>/com_quat_body'
   *  UnaryMinus: '<S4>/Unary Minus2'
   */
  rtb_Product2_l = -rtU.com_quat_body[3] / rtb_Product2_l;

  /* Sum: '<S6>/Sum' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   *  Product: '<S6>/Product'
   *  Product: '<S6>/Product1'
   *  Product: '<S6>/Product2'
   *  Product: '<S6>/Product3'
   */
  rtb_Sign = ((rtb_Sum_b * rtU.est_curr_quat_body[0] - rtb_Product1 *
               rtU.est_curr_quat_body[1]) - rtb_Product2 *
              rtU.est_curr_quat_body[2]) - rtb_Product2_l *
    rtU.est_curr_quat_body[3];

  /* Signum: '<S1>/Sign' */
  if (rtIsNaN(rtb_Sign)) {
    rtb_Sign = (rtNaN);
  } else if (rtb_Sign < 0.0) {
    rtb_Sign = -1.0;
  } else {
    rtb_Sign = (rtb_Sign > 0.0);
  }

  /* End of Signum: '<S1>/Sign' */

  /* Outport: '<Root>/comm_wheel_torque_body' incorporates:
   *  Gain: '<S1>/k_d'
   *  Gain: '<S1>/k_p'
   *  Inport: '<Root>/ang_vel_body'
   *  Inport: '<Root>/curr_quat_body'
   *  Product: '<S1>/Product'
   *  Product: '<S7>/Product'
   *  Product: '<S7>/Product1'
   *  Product: '<S7>/Product2'
   *  Product: '<S7>/Product3'
   *  Product: '<S8>/Product'
   *  Product: '<S8>/Product1'
   *  Product: '<S8>/Product2'
   *  Product: '<S8>/Product3'
   *  Product: '<S9>/Product'
   *  Product: '<S9>/Product1'
   *  Product: '<S9>/Product2'
   *  Product: '<S9>/Product3'
   *  Sum: '<S1>/Sum6'
   *  Sum: '<S7>/Sum'
   *  Sum: '<S8>/Sum'
   *  Sum: '<S9>/Sum'
   */
  rtY.comm_wheel_torque_body[0] = (((rtb_Sum_b * rtU.est_curr_quat_body[1] +
    rtb_Product1 * rtU.est_curr_quat_body[0]) + rtb_Product2 *
    rtU.est_curr_quat_body[3]) - rtb_Product2_l * rtU.est_curr_quat_body[2]) *
    0.2 * rtb_Sign + 0.1 * rtU.est_curr_ang_vel_body[0];
  rtY.comm_wheel_torque_body[1] = (((rtb_Sum_b * rtU.est_curr_quat_body[2] -
    rtb_Product1 * rtU.est_curr_quat_body[3]) + rtb_Product2 *
    rtU.est_curr_quat_body[0]) + rtb_Product2_l * rtU.est_curr_quat_body[1]) *
    0.2 * rtb_Sign + 0.1 * rtU.est_curr_ang_vel_body[1];
  rtY.comm_wheel_torque_body[2] = (((rtb_Sum_b * rtU.est_curr_quat_body[3] +
    rtb_Product1 * rtU.est_curr_quat_body[2]) - rtb_Product2 *
    rtU.est_curr_quat_body[1]) + rtb_Product2_l * rtU.est_curr_quat_body[0]) *
    0.2 * rtb_Sign + 0.1 * rtU.est_curr_ang_vel_body[2];
}

/* Model initialize function */
void attitude_control_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
