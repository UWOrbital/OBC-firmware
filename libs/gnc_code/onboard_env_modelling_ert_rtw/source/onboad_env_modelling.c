/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: onboad_env_modelling.c
 *
 * Code generated for Simulink model 'onboad_env_modelling'.
 *
 * Model version                  : 3.78
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Mon Jan  1 12:51:32 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-R
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "onboad_env_modelling.h"
#include <math.h>
#include "rtwtypes.h"

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;

/* Model step function */
void onboad_env_modelling_step(void)
{
  real_T b;

  /* MATLAB Function: '<S1>/Regenerates from UV Values' incorporates:
   *  Inport: '<Root>/steve_values'
   */
  b = sqrt((1.0 - rtU.steve_values[0] * rtU.steve_values[0]) - rtU.steve_values
           [1] * rtU.steve_values[1]);

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  Inport: '<Root>/steve_values'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   *  Product: '<S2>/Element Product'
   *  Sum: '<S2>/Sum'
   */
  rtY.r_ref_com_est[0] = ((rtU.r_sat_com_ax1[1] * rtU.r_sat_com[2] -
    rtU.r_sat_com[1] * rtU.r_sat_com_ax1[2]) * rtU.steve_values[1] +
    rtU.r_sat_com[0] * rtU.steve_values[0]) + rtU.r_sat_com_ax1[0] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  rtY.estimated_expect_ang_acc_body[0] = rtU.commanded_mag_dipole_body[0];

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  Inport: '<Root>/steve_values'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   *  Product: '<S2>/Element Product'
   *  Sum: '<S2>/Sum'
   */
  rtY.r_ref_com_est[1] = ((rtU.r_sat_com[0] * rtU.r_sat_com_ax1[2] -
    rtU.r_sat_com_ax1[0] * rtU.r_sat_com[2]) * rtU.steve_values[1] +
    rtU.steve_values[0] * rtU.r_sat_com[1]) + rtU.r_sat_com_ax1[1] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  rtY.estimated_expect_ang_acc_body[1] = rtU.commanded_mag_dipole_body[1];

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  Inport: '<Root>/steve_values'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   *  Product: '<S2>/Element Product'
   *  Sum: '<S2>/Sum'
   */
  rtY.r_ref_com_est[2] = ((rtU.r_sat_com_ax1[0] * rtU.r_sat_com[1] -
    rtU.r_sat_com[0] * rtU.r_sat_com_ax1[1]) * rtU.steve_values[1] +
    rtU.steve_values[0] * rtU.r_sat_com[2]) + rtU.r_sat_com_ax1[2] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  rtY.estimated_expect_ang_acc_body[2] = rtU.commanded_mag_dipole_body[2];
}

/* Model initialize function */
void onboad_env_modelling_initialize(void)
{
  /* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
