/*
 * onboard_env_modelling.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "onboard_env_modelling".
 *
 * Model version              : 3.77
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Mon Dec  4 21:29:29 2023
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: ARM Compatible->ARM Cortex-R
 * Code generation objective: Debugging
 * Validation result: Not run
 */

#include "onboard_env_modelling.h"
#include <math.h>
#include "rtwtypes.h"
#include <string.h>
#include "onboard_env_modelling_private.h"
#include "rt_nonfinite.h"

/* Block signals (default storage) */
B_onboard_env_modelling_T onboard_env_modelling_B;

/* External inputs (root inport signals with default storage) */
ExtU_onboard_env_modelling_T onboard_env_modelling_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_onboard_env_modelling_T onboard_env_modelling_Y;

/* Real-time model */
static RT_MODEL_onboard_env_modellin_T onboard_env_modelling_M_;
RT_MODEL_onboard_env_modellin_T *const onboard_env_modelling_M =
  &onboard_env_modelling_M_;

/* Model step function */
void onboard_env_modelling_step(void)
{
  real_T Sum;
  real_T b;
  real_T steve_values;
  real_T steve_values_0;

  /* Product: '<S2>/Element Product' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   */
  /* :  y = u; */
  onboard_env_modelling_B.ElementProduct[0] =
    onboard_env_modelling_U.r_sat_com_ax1[1] *
    onboard_env_modelling_U.r_sat_com[2];
  onboard_env_modelling_B.ElementProduct[1] = onboard_env_modelling_U.r_sat_com
    [0] * onboard_env_modelling_U.r_sat_com_ax1[2];
  onboard_env_modelling_B.ElementProduct[2] =
    onboard_env_modelling_U.r_sat_com_ax1[0] *
    onboard_env_modelling_U.r_sat_com[1];
  onboard_env_modelling_B.ElementProduct[3] = onboard_env_modelling_U.r_sat_com
    [1] * onboard_env_modelling_U.r_sat_com_ax1[2];
  onboard_env_modelling_B.ElementProduct[4] =
    onboard_env_modelling_U.r_sat_com_ax1[0] *
    onboard_env_modelling_U.r_sat_com[2];
  onboard_env_modelling_B.ElementProduct[5] = onboard_env_modelling_U.r_sat_com
    [0] * onboard_env_modelling_U.r_sat_com_ax1[1];

  /* MATLAB Function: '<S1>/Regenerates from UV Values' incorporates:
   *  Inport: '<Root>/steve_values'
   */
  /* :  r_ref_com_est_unshaped =   r_sat_com_ax1*st1 + r_sat_com_ax2*st2 ... */
  /* :                  + r_sat_com*sqrt(1-st1^2-st2^2); */
  b = sqrt((1.0 - onboard_env_modelling_U.steve_values[0] *
            onboard_env_modelling_U.steve_values[0]) -
           onboard_env_modelling_U.steve_values[1] *
           onboard_env_modelling_U.steve_values[1]);

  /* :  r_ref_com_est = reshape(r_ref_com_est_unshaped,[1 3]); */
  steve_values = onboard_env_modelling_U.steve_values[0];
  steve_values_0 = onboard_env_modelling_U.steve_values[1];

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  onboard_env_modelling_Y.estimated_expect_ang_acc_body[0] =
    onboard_env_modelling_U.commanded_mag_dipole_body[0];

  /* Sum: '<S2>/Sum' */
  Sum = onboard_env_modelling_B.ElementProduct[0] -
    onboard_env_modelling_B.ElementProduct[3];
  onboard_env_modelling_B.Sum[0] = Sum;

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   */
  onboard_env_modelling_Y.r_ref_com_est[0] = (onboard_env_modelling_U.r_sat_com
    [0] * steve_values + Sum * steve_values_0) +
    onboard_env_modelling_U.r_sat_com_ax1[0] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  onboard_env_modelling_Y.estimated_expect_ang_acc_body[1] =
    onboard_env_modelling_U.commanded_mag_dipole_body[1];

  /* Sum: '<S2>/Sum' */
  Sum = onboard_env_modelling_B.ElementProduct[1] -
    onboard_env_modelling_B.ElementProduct[4];
  onboard_env_modelling_B.Sum[1] = Sum;

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   */
  onboard_env_modelling_Y.r_ref_com_est[1] = (onboard_env_modelling_U.r_sat_com
    [1] * steve_values + Sum * steve_values_0) +
    onboard_env_modelling_U.r_sat_com_ax1[1] * b;

  /* Outport: '<Root>/estimated_expect_ang_acc_body' incorporates:
   *  Inport: '<Root>/magnetorquer comm'
   *  MATLAB Function: '<S1>/MATLAB Function6'
   */
  onboard_env_modelling_Y.estimated_expect_ang_acc_body[2] =
    onboard_env_modelling_U.commanded_mag_dipole_body[2];

  /* Sum: '<S2>/Sum' */
  Sum = onboard_env_modelling_B.ElementProduct[2] -
    onboard_env_modelling_B.ElementProduct[5];
  onboard_env_modelling_B.Sum[2] = Sum;

  /* Outport: '<Root>/r_ref_com_est' incorporates:
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/r_sat_com_ax1'
   *  MATLAB Function: '<S1>/Regenerates from UV Values'
   */
  onboard_env_modelling_Y.r_ref_com_est[2] = (onboard_env_modelling_U.r_sat_com
    [2] * steve_values + Sum * steve_values_0) +
    onboard_env_modelling_U.r_sat_com_ax1[2] * b;

  /* Matfile logging */
  rt_UpdateTXYLogVars(onboard_env_modelling_M->rtwLogInfo,
                      (&onboard_env_modelling_M->Timing.taskTime0));

  /* signal main to stop simulation */
  {                                    /* Sample time: [0.05s, 0.0s] */
    if ((rtmGetTFinal(onboard_env_modelling_M)!=-1) &&
        !((rtmGetTFinal(onboard_env_modelling_M)-
           onboard_env_modelling_M->Timing.taskTime0) >
          onboard_env_modelling_M->Timing.taskTime0 * (DBL_EPSILON))) {
      rtmSetErrorStatus(onboard_env_modelling_M, "Simulation finished");
    }
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++onboard_env_modelling_M->Timing.clockTick0)) {
    ++onboard_env_modelling_M->Timing.clockTickH0;
  }

  onboard_env_modelling_M->Timing.taskTime0 =
    onboard_env_modelling_M->Timing.clockTick0 *
    onboard_env_modelling_M->Timing.stepSize0 +
    onboard_env_modelling_M->Timing.clockTickH0 *
    onboard_env_modelling_M->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
void onboard_env_modelling_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)onboard_env_modelling_M, 0,
                sizeof(RT_MODEL_onboard_env_modellin_T));
  rtmSetTFinal(onboard_env_modelling_M, 7200.0);
  onboard_env_modelling_M->Timing.stepSize0 = 0.05;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    onboard_env_modelling_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(onboard_env_modelling_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(onboard_env_modelling_M->rtwLogInfo, (NULL));
    rtliSetLogT(onboard_env_modelling_M->rtwLogInfo, "");
    rtliSetLogX(onboard_env_modelling_M->rtwLogInfo, "");
    rtliSetLogXFinal(onboard_env_modelling_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(onboard_env_modelling_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(onboard_env_modelling_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(onboard_env_modelling_M->rtwLogInfo, 0);
    rtliSetLogDecimation(onboard_env_modelling_M->rtwLogInfo, 1);
    rtliSetLogY(onboard_env_modelling_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(onboard_env_modelling_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(onboard_env_modelling_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &onboard_env_modelling_B), 0,
                sizeof(B_onboard_env_modelling_T));

  /* external inputs */
  (void)memset(&onboard_env_modelling_U, 0, sizeof(ExtU_onboard_env_modelling_T));

  /* external outputs */
  (void)memset(&onboard_env_modelling_Y, 0, sizeof(ExtY_onboard_env_modelling_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(onboard_env_modelling_M->rtwLogInfo, 0.0,
    rtmGetTFinal(onboard_env_modelling_M),
    onboard_env_modelling_M->Timing.stepSize0, (&rtmGetErrorStatus
    (onboard_env_modelling_M)));
}

/* Model terminate function */
void onboard_env_modelling_terminate(void)
{
  /* (no terminate code required) */
}
