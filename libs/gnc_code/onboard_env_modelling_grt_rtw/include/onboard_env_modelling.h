/*
 * onboard_env_modelling.h
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

#ifndef RTW_HEADER_onboard_env_modelling_h_
#define RTW_HEADER_onboard_env_modelling_h_
#ifndef onboard_env_modelling_COMMON_INCLUDES_
#define onboard_env_modelling_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                              /* onboard_env_modelling_COMMON_INCLUDES_ */

#include "onboard_env_modelling_types.h"
#include <float.h>
#include <string.h>
#include <stddef.h>
#include "rt_nonfinite.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetFinalTime
#define rtmGetFinalTime(rtm)           ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetRTWLogInfo
#define rtmGetRTWLogInfo(rtm)          ((rtm)->rtwLogInfo)
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
#define rtmGetStopRequested(rtm)       ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
#define rtmSetStopRequested(rtm, val)  ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
#define rtmGetStopRequestedPtr(rtm)    (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
#define rtmGetT(rtm)                   ((rtm)->Timing.taskTime0)
#endif

#ifndef rtmGetTFinal
#define rtmGetTFinal(rtm)              ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetTPtr
#define rtmGetTPtr(rtm)                (&(rtm)->Timing.taskTime0)
#endif

/* Block signals (default storage) */
typedef struct {
  real_T ElementProduct[6];            /* '<S2>/Element Product' */
  real_T Sum[3];                       /* '<S2>/Sum' */
} B_onboard_env_modelling_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T commanded_mag_dipole_body[3]; /* '<Root>/magnetorquer comm' */
  real_T r_sat_com_ax1[3];             /* '<Root>/r_sat_com' */
  real_T r_sat_com[3];                 /* '<Root>/r_sat_com_ax1' */
  real_T steve_values[2];              /* '<Root>/steve_values' */
} ExtU_onboard_env_modelling_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T estimated_expect_ang_acc_body[3];
                                    /* '<Root>/estimated_expect_ang_acc_body' */
  real_T r_ref_com_est[3];             /* '<Root>/r_ref_com_est' */
} ExtY_onboard_env_modelling_T;

/* Real-time Model Data Structure */
struct tag_RTM_onboard_env_modelling_T {
  const char_T *errorStatus;
  RTWLogInfo *rtwLogInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    time_T taskTime0;
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    time_T tFinal;
    boolean_T stopRequestedFlag;
  } Timing;
};

/* Block signals (default storage) */
extern B_onboard_env_modelling_T onboard_env_modelling_B;

/* External inputs (root inport signals with default storage) */
extern ExtU_onboard_env_modelling_T onboard_env_modelling_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_onboard_env_modelling_T onboard_env_modelling_Y;

/* Model entry point functions */
extern void onboard_env_modelling_initialize(void);
extern void onboard_env_modelling_step(void);
extern void onboard_env_modelling_terminate(void);

/* Real-time Model object */
extern RT_MODEL_onboard_env_modellin_T *const onboard_env_modelling_M;

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
 * hilite_system('cubesat_alg_dev_env_v2_main/onboard_env_modelling')    - opens subsystem cubesat_alg_dev_env_v2_main/onboard_env_modelling
 * hilite_system('cubesat_alg_dev_env_v2_main/onboard_env_modelling/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'cubesat_alg_dev_env_v2_main'
 * '<S1>'   : 'cubesat_alg_dev_env_v2_main/onboard_env_modelling'
 * '<S2>'   : 'cubesat_alg_dev_env_v2_main/onboard_env_modelling/Cross Product'
 * '<S3>'   : 'cubesat_alg_dev_env_v2_main/onboard_env_modelling/MATLAB Function6'
 * '<S4>'   : 'cubesat_alg_dev_env_v2_main/onboard_env_modelling/Regenerates from UV Values'
 */
#endif                                 /* RTW_HEADER_onboard_env_modelling_h_ */
