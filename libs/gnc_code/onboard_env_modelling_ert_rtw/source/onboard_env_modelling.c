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

#include "onboard_env_modelling.h"
#include <math.h>
#include "rtwtypes.h"

/* External inputs (root inport signals with default storage) */
onboard_env_model_ext_intputs_t onboard_env_model_ext_intputs;

/* External outputs (root outports fed by signals with default storage) */
onboard_env_model_ext_outputs_t onboard_env_model_ext_outputs;

/* Real-time model */
static RT_MODEL_onboard_model rtM_;
RT_MODEL_onboard_model *const onboard_env_model_rt_object = &rtM_;

/* Model step function */
void onboad_env_modelling_step(void)
{
  real_T b;

  /* MATLAB Function: '<S1>/Regenerates from UV Values' incorporates:
   *  Inport: '<Root>/steve_values'
   */
  b = sqrt((1.0 - onboard_env_model_ext_intputs.steve_values[0] * onboard_env_model_ext_intputs.steve_values[0]) - onboard_env_model_ext_intputs.steve_values
           [1] * onboard_env_model_ext_intputs.steve_values[1]);

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  Inport: '<Root>/steve_values'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   *  Product: '<S2>/Element Product'
   *  Sum: '<S2>/Sum'
   */
  onboard_env_model_ext_outputs.r_ref_com_est[0] = ((onboard_env_model_ext_intputs.r_sat_com_ax1[1] * onboard_env_model_ext_intputs.r_sat_com[2] -
    onboard_env_model_ext_intputs.r_sat_com[1] * onboard_env_model_ext_intputs.r_sat_com_ax1[2]) * onboard_env_model_ext_intputs.steve_values[1] +
    onboard_env_model_ext_intputs.r_sat_com[0] * onboard_env_model_ext_intputs.steve_values[0]) + onboard_env_model_ext_intputs.r_sat_com_ax1[0] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[0] = onboard_env_model_ext_intputs.commanded_mag_dipole_body[0];

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  Inport: '<Root>/steve_values'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   *  Product: '<S2>/Element Product'
   *  Sum: '<S2>/Sum'
   */
  onboard_env_model_ext_outputs.r_ref_com_est[1] = ((onboard_env_model_ext_intputs.r_sat_com[0] * onboard_env_model_ext_intputs.r_sat_com_ax1[2] -
    onboard_env_model_ext_intputs.r_sat_com_ax1[0] * onboard_env_model_ext_intputs.r_sat_com[2]) * onboard_env_model_ext_intputs.steve_values[1] +
    onboard_env_model_ext_intputs.steve_values[0] * onboard_env_model_ext_intputs.r_sat_com[1]) + onboard_env_model_ext_intputs.r_sat_com_ax1[1] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[1] = onboard_env_model_ext_intputs.commanded_mag_dipole_body[1];

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  Inport: '<Root>/steve_values'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   *  Product: '<S2>/Element Product'
   *  Sum: '<S2>/Sum'
   */
  onboard_env_model_ext_outputs.r_ref_com_est[2] = ((onboard_env_model_ext_intputs.r_sat_com_ax1[0] * onboard_env_model_ext_intputs.r_sat_com[1] -
    onboard_env_model_ext_intputs.r_sat_com[0] * onboard_env_model_ext_intputs.r_sat_com_ax1[1]) * onboard_env_model_ext_intputs.steve_values[1] +
    onboard_env_model_ext_intputs.steve_values[0] * onboard_env_model_ext_intputs.r_sat_com[2]) + onboard_env_model_ext_intputs.r_sat_com_ax1[2] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[2] = onboard_env_model_ext_intputs.commanded_mag_dipole_body[2];
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
