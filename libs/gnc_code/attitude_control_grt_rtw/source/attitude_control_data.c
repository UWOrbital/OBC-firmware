/*
 * attitude_control_data.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "attitude_control".
 *
 * Model version              : 3.74
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Mon Nov  6 22:53:24 2023
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objective: Debugging
 * Validation result: Not run
 */

#include "attitude_control.h"

/* Block parameters (default storage) */
P_attitude_control_T attitude_control_P = {
  /* Expression: 0.2
   * Referenced by: '<S1>/k_p'
   */
  0.2,

  /* Expression: 0.1
   * Referenced by: '<S1>/k_d'
   */
  0.1
};
