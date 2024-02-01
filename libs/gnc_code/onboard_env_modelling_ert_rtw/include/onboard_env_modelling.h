/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: onboad_env_modelling.h
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

#ifndef RTW_HEADER_onboad_env_modelling_h_
#define RTW_HEADER_onboad_env_modelling_h_
#ifndef onboad_env_modelling_COMMON_INCLUDES_
#define onboad_env_modelling_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                               /* onboad_env_modelling_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_onboard_model RT_MODEL_onboard_model;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T commanded_mag_dipole_body[3]; /* '<Root>/magnetorquer comm' */
  real_T r_sat_com[3];                 /* '<Root>/r_sat_com' */
  real_T r_sat_com_ax1[3];             /* '<Root>/r_sat_com_ax1' */
  real_T steve_values[2];              /* '<Root>/steve_values' */
} onboard_env_model_ext_intputs_t;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T estimated_expect_ang_acc_body[3];
                                    /* '<Root>/estimated_expect_ang_acc_body' */
  real_T r_ref_com_est[3];             /* '<Root>/r_ref_com_est' */
} onboard_env_model_ext_outputs_t;

/* Real-time Model Data Structure */
struct tag_RTM_onboard_model {
  const char_T * volatile errorStatus;
};

/* External inputs (root inport signals with default storage) */
extern onboard_env_model_ext_intputs_t onboard_env_model_ext_intputs;

/* External outputs (root outports fed by signals with default storage) */
extern onboard_env_model_ext_outputs_t onboard_env_model_ext_outputs;

/* Model entry point functions */
extern void onboad_env_modelling_initialize(void);
extern void onboad_env_modelling_step(void);

/* Real-time Model object */
extern RT_MODEL_onboard_model *const onboard_env_model_rt_object;

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
 * hilite_system('cubesat_alg_dev_env_v2_main/onboad_env_modelling')    - opens subsystem cubesat_alg_dev_env_v2_main/onboad_env_modelling
 * hilite_system('cubesat_alg_dev_env_v2_main/onboad_env_modelling/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'cubesat_alg_dev_env_v2_main'
 * '<S1>'   : 'cubesat_alg_dev_env_v2_main/onboad_env_modelling'
 * '<S2>'   : 'cubesat_alg_dev_env_v2_main/onboad_env_modelling/Cross Product'
 * '<S3>'   : 'cubesat_alg_dev_env_v2_main/onboad_env_modelling/MATLAB Function6'
 * '<S4>'   : 'cubesat_alg_dev_env_v2_main/onboad_env_modelling/Regenerates from UV Values'
 */
#endif                                 /* RTW_HEADER_onboad_env_modelling_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
