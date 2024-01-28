/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: attitude_determination_and_vehi.h
 *
 * Code generated for Simulink model 'attitude_determination_and_vehi'.
 *
 * Model version                  : 3.78
 * Simulink Coder version         : 9.9 (R2023a) 19-Nov-2022
 * C/C++ source code generated on : Mon Jan  1 12:50:14 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-R
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_attitude_determination_and_vehi_h_
#define RTW_HEADER_attitude_determination_and_vehi_h_
#ifndef attitude_determination_and_vehi_COMMON_INCLUDES_
#define attitude_determination_and_vehi_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                    /* attitude_determination_and_vehi_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_attitude_determination RT_MODEL_attitude_determination;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T P_o[36];                      /* '<S1>/Data Store Memory' */
  real_T q_n2m[4];                     /* '<S1>/Data Store Memory1' */
  real_T beta[3];                      /* '<S1>/Data Store Memory2' */
} DW;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: diag([1*ones(1,3), 1*ones(1,3)])
   * Referenced by: '<S1>/Data Store Memory'
   */
  real_T DataStoreMemory_InitialValue[36];
} ConstP;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T earth_mag_field_ref[3];       /* '<Root>/earth_mag_field_ref' */
  real_T sat_to_sun_unit_ref[3];       /* '<Root>/sat_to_sun_unit_ref' */
  real_T omega[3];                     /* '<Root>/omega' */
  real_T mes_ss[3];                    /* '<Root>/mes_ss' */
  real_T r_sat_com_ax1[3];             /* '<Root>/r_sat_com' */
  real_T mes_mag[3];                   /* '<Root>/mes_mag' */
  real_T ref_aam[3];                   /* '<Root>/ref_aam' */
  real_T mes_aam[3];                   /* '<Root>/mes_aam' */
  real_T steve_mes[3];                 /* '<Root>/steve_mes' */
} attitude_determination_model_ext_inputs_t;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T meas_ang_vel_body[3];         /* '<Root>/meas_ang_vel_body' */
  real_T meas_quat_body[4];            /* '<Root>/meas_quat_body' */
} attitude_determination_model_ext_outputs_t;

/* Real-time Model Data Structure */
struct tag_RTM_attitude_determination {
  const char_T * volatile errorStatus;
};

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern attitude_determination_model_ext_inputs_t attitude_determination_model_ext_inputs;

/* External outputs (root outports fed by signals with default storage) */
extern attitude_determination_model_ext_outputs_t attitude_determination_model_ext_outputs;

/* Constant parameters (default storage) */
extern const ConstP rtConstP;

/* Model entry point functions */
extern void attitude_determination_and_vehi_initialize(void);
extern void attitude_determination_and_vehi_step(void);

/* Real-time Model object */
extern RT_MODEL_attitude_determination *const attitude_determinataion_model_rt_object;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Estimated Angular Velocity' : Unused code path elimination
 * Block '<S1>/Estimated Attitude' : Unused code path elimination
 * Block '<S1>/MEKF Covariance Matrix' : Unused code path elimination
 * Block '<S1>/Reshape' : Reshape block reduction
 * Block '<S1>/Reshape1' : Reshape block reduction
 * Block '<S1>/Reshape2' : Reshape block reduction
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
 * hilite_system('cubesat_alg_dev_env_v2_main/attitude_determination_and_vehicle_est')    - opens subsystem cubesat_alg_dev_env_v2_main/attitude_determination_and_vehicle_est
 * hilite_system('cubesat_alg_dev_env_v2_main/attitude_determination_and_vehicle_est/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'cubesat_alg_dev_env_v2_main'
 * '<S1>'   : 'cubesat_alg_dev_env_v2_main/attitude_determination_and_vehicle_est'
 * '<S2>'   : 'cubesat_alg_dev_env_v2_main/attitude_determination_and_vehicle_est/MEKF'
 */
#endif                       /* RTW_HEADER_attitude_determination_and_vehi_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
