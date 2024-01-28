/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: attitude_control.h
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

#ifndef RTW_HEADER_attitude_control_h_
#define RTW_HEADER_attitude_control_h_
#ifndef attitude_control_COMMON_INCLUDES_
#define attitude_control_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                                 /* attitude_control_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_attitude_control RT_MODEL_attitude_control;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T est_curr_ang_vel_body[3];     /* '<Root>/ang_vel_body' */
  real_T est_curr_quat_body[4];        /* '<Root>/curr_quat_body' */
  real_T com_quat_body[4];             /* '<Root>/com_quat_body' */
  real_T mag_field_body[3];            /* '<Root>/mag_field_body' */
} attitude_control_model_ext_inputs_t;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T comm_wheel_torque_body[3];    /* '<Root>/comm_wheel_torque_body' */
  real_T comm_mag_dipole_body[3];      /* '<Root>/comm_mag_dipole_body' */
} attitude_control_model_ext_outputs_t;

/* Real-time Model Data Structure */
struct tag_RTM_attitude_control {
  const char_T * volatile errorStatus;
};

/* External inputs (root inport signals with default storage) */
extern attitude_control_model_ext_inputs_t attitude_control_model_ext_inputs;

/* External outputs (root outports fed by signals with default storage) */
extern attitude_control_model_ext_outputs_t attitude_control_model_ext_outputs;

/* Model entry point functions */
extern void attitude_control_initialize(void);
extern void attitude_control_step(void);

/* Real-time Model object */
extern RT_MODEL_attitude_control *const attitude_control_model_rt_object;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/reaction_wheel_torque' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('cubesat_alg_dev_env_v2_main/attitude_control')    - opens subsystem cubesat_alg_dev_env_v2_main/attitude_control
 * hilite_system('cubesat_alg_dev_env_v2_main/attitude_control/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'cubesat_alg_dev_env_v2_main'
 * '<S1>'   : 'cubesat_alg_dev_env_v2_main/attitude_control'
 * '<S2>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Inverse1'
 * '<S3>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Multiplication'
 * '<S4>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Inverse1/Quaternion Conjugate'
 * '<S5>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Inverse1/Quaternion Norm'
 * '<S6>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Multiplication/q0'
 * '<S7>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Multiplication/q1'
 * '<S8>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Multiplication/q2'
 * '<S9>'   : 'cubesat_alg_dev_env_v2_main/attitude_control/Quaternion Multiplication/q3'
 */
#endif                                 /* RTW_HEADER_attitude_control_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
