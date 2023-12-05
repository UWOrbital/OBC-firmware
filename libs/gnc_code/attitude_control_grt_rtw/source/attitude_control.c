/*
 * attitude_control.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "attitude_control".
 *
 * Model version              : 3.77
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Mon Dec  4 21:31:21 2023
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: ARM Compatible->ARM Cortex-R
 * Code generation objective: Debugging
 * Validation result: Not run
 */

#include "attitude_control.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include <string.h>
#include "attitude_control_private.h"

/* Block signals (default storage) */
B_attitude_control_T attitude_control_B;

/* External inputs (root inport signals with default storage) */
ExtU_attitude_control_T attitude_control_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_attitude_control_T attitude_control_Y;

/* Real-time model */
static RT_MODEL_attitude_control_T attitude_control_M_;
RT_MODEL_attitude_control_T *const attitude_control_M = &attitude_control_M_;

/* Model step function */
void attitude_control_step(void)
{
  real_T k_d;
  real_T u;

  /* Product: '<S5>/Product' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.Product = attitude_control_U.com_quat_body[0] *
    attitude_control_U.com_quat_body[0];

  /* Product: '<S5>/Product1' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.Product1 = attitude_control_U.com_quat_body[1] *
    attitude_control_U.com_quat_body[1];

  /* Product: '<S5>/Product2' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.Product2 = attitude_control_U.com_quat_body[2] *
    attitude_control_U.com_quat_body[2];

  /* Product: '<S5>/Product3' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.Product3 = attitude_control_U.com_quat_body[3] *
    attitude_control_U.com_quat_body[3];

  /* Sum: '<S5>/Sum' */
  attitude_control_B.Sum = ((attitude_control_B.Product +
    attitude_control_B.Product1) + attitude_control_B.Product2) +
    attitude_control_B.Product3;

  /* Product: '<S2>/Divide' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.Divide = attitude_control_U.com_quat_body[0] /
    attitude_control_B.Sum;

  /* Product: '<S6>/Product' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product_d = attitude_control_B.Divide *
    attitude_control_U.est_curr_quat_body[0];

  /* UnaryMinus: '<S4>/Unary Minus' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.UnaryMinus = -attitude_control_U.com_quat_body[1];

  /* Product: '<S2>/Divide1' */
  attitude_control_B.Divide1 = attitude_control_B.UnaryMinus /
    attitude_control_B.Sum;

  /* Product: '<S6>/Product1' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product1_o = attitude_control_B.Divide1 *
    attitude_control_U.est_curr_quat_body[1];

  /* UnaryMinus: '<S4>/Unary Minus1' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.UnaryMinus1 = -attitude_control_U.com_quat_body[2];

  /* Product: '<S2>/Divide2' */
  attitude_control_B.Divide2 = attitude_control_B.UnaryMinus1 /
    attitude_control_B.Sum;

  /* Product: '<S6>/Product2' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product2_i = attitude_control_B.Divide2 *
    attitude_control_U.est_curr_quat_body[2];

  /* UnaryMinus: '<S4>/Unary Minus2' incorporates:
   *  Inport: '<Root>/com_quat_body'
   */
  attitude_control_B.UnaryMinus2 = -attitude_control_U.com_quat_body[3];

  /* Product: '<S2>/Divide3' */
  attitude_control_B.Divide3 = attitude_control_B.UnaryMinus2 /
    attitude_control_B.Sum;

  /* Product: '<S6>/Product3' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product3_p = attitude_control_B.Divide3 *
    attitude_control_U.est_curr_quat_body[3];

  /* Sum: '<S6>/Sum' */
  attitude_control_B.Sum_d = ((attitude_control_B.Product_d -
    attitude_control_B.Product1_o) - attitude_control_B.Product2_i) -
    attitude_control_B.Product3_p;

  /* Signum: '<S1>/Sign' */
  u = attitude_control_B.Sum_d;
  if (rtIsNaN(u)) {
    /* Signum: '<S1>/Sign' */
    attitude_control_B.Sign = (rtNaN);
  } else if (u < 0.0) {
    /* Signum: '<S1>/Sign' */
    attitude_control_B.Sign = -1.0;
  } else {
    /* Signum: '<S1>/Sign' */
    attitude_control_B.Sign = (u > 0.0);
  }

  /* End of Signum: '<S1>/Sign' */

  /* Product: '<S7>/Product' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product_p = attitude_control_B.Divide *
    attitude_control_U.est_curr_quat_body[1];

  /* Product: '<S7>/Product1' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product1_f = attitude_control_B.Divide1 *
    attitude_control_U.est_curr_quat_body[0];

  /* Product: '<S7>/Product2' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product2_m = attitude_control_B.Divide2 *
    attitude_control_U.est_curr_quat_body[3];

  /* Product: '<S7>/Product3' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product3_o = attitude_control_B.Divide3 *
    attitude_control_U.est_curr_quat_body[2];

  /* Sum: '<S7>/Sum' */
  attitude_control_B.Sum_m = ((attitude_control_B.Product_p +
    attitude_control_B.Product1_f) + attitude_control_B.Product2_m) -
    attitude_control_B.Product3_o;

  /* Product: '<S8>/Product' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product_pw = attitude_control_B.Divide *
    attitude_control_U.est_curr_quat_body[2];

  /* Product: '<S8>/Product1' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product1_n = attitude_control_B.Divide1 *
    attitude_control_U.est_curr_quat_body[3];

  /* Product: '<S8>/Product2' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product2_iw = attitude_control_B.Divide2 *
    attitude_control_U.est_curr_quat_body[0];

  /* Product: '<S8>/Product3' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product3_e = attitude_control_B.Divide3 *
    attitude_control_U.est_curr_quat_body[1];

  /* Sum: '<S8>/Sum' */
  attitude_control_B.Sum_l = ((attitude_control_B.Product_pw -
    attitude_control_B.Product1_n) + attitude_control_B.Product2_iw) +
    attitude_control_B.Product3_e;

  /* Product: '<S9>/Product' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product_i = attitude_control_B.Divide *
    attitude_control_U.est_curr_quat_body[3];

  /* Product: '<S9>/Product1' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product1_h = attitude_control_B.Divide1 *
    attitude_control_U.est_curr_quat_body[2];

  /* Product: '<S9>/Product2' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product2_ig = attitude_control_B.Divide2 *
    attitude_control_U.est_curr_quat_body[1];

  /* Product: '<S9>/Product3' incorporates:
   *  Inport: '<Root>/curr_quat_body'
   */
  attitude_control_B.Product3_d = attitude_control_B.Divide3 *
    attitude_control_U.est_curr_quat_body[0];

  /* Sum: '<S9>/Sum' */
  attitude_control_B.Sum_o = ((attitude_control_B.Product_i +
    attitude_control_B.Product1_h) - attitude_control_B.Product2_ig) +
    attitude_control_B.Product3_d;

  /* Gain: '<S1>/k_p' */
  attitude_control_B.k_p[0] = 0.2 * attitude_control_B.Sum_m;
  attitude_control_B.k_p[1] = 0.2 * attitude_control_B.Sum_l;
  attitude_control_B.k_p[2] = 0.2 * attitude_control_B.Sum_o;

  /* Product: '<S1>/Product' */
  u = attitude_control_B.Sign * attitude_control_B.k_p[0];
  attitude_control_B.Product_k[0] = u;

  /* Gain: '<S1>/k_d' incorporates:
   *  Inport: '<Root>/ang_vel_body'
   */
  k_d = 0.1 * attitude_control_U.est_curr_ang_vel_body[0];
  attitude_control_B.k_d[0] = k_d;

  /* Outport: '<Root>/comm_wheel_torque_body' incorporates:
   *  Gain: '<S1>/k_d'
   *  Sum: '<S1>/Sum6'
   */
  attitude_control_Y.comm_wheel_torque_body[0] = u + k_d;

  /* Product: '<S1>/Product' */
  u = attitude_control_B.Sign * attitude_control_B.k_p[1];
  attitude_control_B.Product_k[1] = u;

  /* Gain: '<S1>/k_d' incorporates:
   *  Inport: '<Root>/ang_vel_body'
   */
  k_d = 0.1 * attitude_control_U.est_curr_ang_vel_body[1];
  attitude_control_B.k_d[1] = k_d;

  /* Outport: '<Root>/comm_wheel_torque_body' incorporates:
   *  Gain: '<S1>/k_d'
   *  Sum: '<S1>/Sum6'
   */
  attitude_control_Y.comm_wheel_torque_body[1] = u + k_d;

  /* Product: '<S1>/Product' */
  u = attitude_control_B.Sign * attitude_control_B.k_p[2];
  attitude_control_B.Product_k[2] = u;

  /* Gain: '<S1>/k_d' incorporates:
   *  Inport: '<Root>/ang_vel_body'
   */
  k_d = 0.1 * attitude_control_U.est_curr_ang_vel_body[2];
  attitude_control_B.k_d[2] = k_d;

  /* Outport: '<Root>/comm_wheel_torque_body' incorporates:
   *  Gain: '<S1>/k_d'
   *  Sum: '<S1>/Sum6'
   */
  attitude_control_Y.comm_wheel_torque_body[2] = u + k_d;

  /* Matfile logging */
  rt_UpdateTXYLogVars(attitude_control_M->rtwLogInfo,
                      (&attitude_control_M->Timing.taskTime0));

  /* signal main to stop simulation */
  {                                    /* Sample time: [0.05s, 0.0s] */
    if ((rtmGetTFinal(attitude_control_M)!=-1) &&
        !((rtmGetTFinal(attitude_control_M)-attitude_control_M->Timing.taskTime0)
          > attitude_control_M->Timing.taskTime0 * (DBL_EPSILON))) {
      rtmSetErrorStatus(attitude_control_M, "Simulation finished");
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
  if (!(++attitude_control_M->Timing.clockTick0)) {
    ++attitude_control_M->Timing.clockTickH0;
  }

  attitude_control_M->Timing.taskTime0 = attitude_control_M->Timing.clockTick0 *
    attitude_control_M->Timing.stepSize0 +
    attitude_control_M->Timing.clockTickH0 *
    attitude_control_M->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
void attitude_control_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)attitude_control_M, 0,
                sizeof(RT_MODEL_attitude_control_T));
  rtmSetTFinal(attitude_control_M, 7200.0);
  attitude_control_M->Timing.stepSize0 = 0.05;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    attitude_control_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(attitude_control_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(attitude_control_M->rtwLogInfo, (NULL));
    rtliSetLogT(attitude_control_M->rtwLogInfo, "");
    rtliSetLogX(attitude_control_M->rtwLogInfo, "");
    rtliSetLogXFinal(attitude_control_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(attitude_control_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(attitude_control_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(attitude_control_M->rtwLogInfo, 0);
    rtliSetLogDecimation(attitude_control_M->rtwLogInfo, 1);
    rtliSetLogY(attitude_control_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(attitude_control_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(attitude_control_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &attitude_control_B), 0,
                sizeof(B_attitude_control_T));

  /* external inputs */
  (void)memset(&attitude_control_U, 0, sizeof(ExtU_attitude_control_T));

  /* external outputs */
  (void)memset(&attitude_control_Y, 0, sizeof(ExtY_attitude_control_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(attitude_control_M->rtwLogInfo, 0.0,
    rtmGetTFinal(attitude_control_M), attitude_control_M->Timing.stepSize0,
    (&rtmGetErrorStatus(attitude_control_M)));

  /* ConstCode for Outport: '<Root>/comm_mag_dipole_body' */
  attitude_control_Y.comm_mag_dipole_body[0] = 0.0;
  attitude_control_Y.comm_mag_dipole_body[1] = 0.0;
  attitude_control_Y.comm_mag_dipole_body[2] = 0.0;
}

/* Model terminate function */
void attitude_control_terminate(void)
{
  /* (no terminate code required) */
}
