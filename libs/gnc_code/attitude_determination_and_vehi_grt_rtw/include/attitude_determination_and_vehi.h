/*
 * attitude_determination_and_vehi.h
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "attitude_determination_and_vehi".
 *
 * Model version              : 3.77
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Mon Dec  4 21:30:29 2023
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: ARM Compatible->ARM Cortex-R
 * Code generation objective: Debugging
 * Validation result: Not run
 */

#ifndef RTW_HEADER_attitude_determination_and_vehi_h_
#define RTW_HEADER_attitude_determination_and_vehi_h_
#ifndef attitude_determination_and_vehi_COMMON_INCLUDES_
#define attitude_determination_and_vehi_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                    /* attitude_determination_and_vehi_COMMON_INCLUDES_ */

#include "attitude_determination_and_vehi_types.h"
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

#define attitude_determination_and_vehi_M (attitude_determination_and_v_M)

/* Block signals (default storage) */
typedef struct {
  real_T DataStoreRead1[4];            /* '<S1>/Data Store Read1' */
  real_T DataStoreRead2[3];            /* '<S1>/Data Store Read2' */
  real_T Transpose1[3];                /* '<S1>/Transpose1' */
  real_T Reshape1[3];                  /* '<S1>/Reshape1' */
  real_T P[36];                        /* '<S1>/MEKF' */
  real_T beta[3];                      /* '<S1>/MEKF' */
  real_T hat_omega[3];                 /* '<S1>/MEKF' */
} B_attitude_determination_and__T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T P[36];                        /* '<S1>/Data Store Memory' */
  real_T q_n2m[4];                     /* '<S1>/Data Store Memory1' */
  real_T beta[3];                      /* '<S1>/Data Store Memory2' */
} DW_attitude_determination_and_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: diag([1*ones(1,3), 1*ones(1,3)])
   * Referenced by: '<S1>/Data Store Memory'
   */
  real_T DataStoreMemory_InitialValue[36];
} ConstP_attitude_determination_T;

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
} ExtU_attitude_determination_a_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T meas_ang_vel_body[3];         /* '<Root>/meas_ang_vel_body' */
  real_T meas_quat_body[4];            /* '<Root>/meas_quat_body' */
} ExtY_attitude_determination_a_T;

/* Real-time Model Data Structure */
struct tag_RTM_attitude_determinatio_T {
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
extern B_attitude_determination_and__T attitude_determination_and_ve_B;

/* Block states (default storage) */
extern DW_attitude_determination_and_T attitude_determination_and_v_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_attitude_determination_a_T attitude_determination_and_ve_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_attitude_determination_a_T attitude_determination_and_ve_Y;

/* Constant parameters (default storage) */
extern const ConstP_attitude_determination_T attitude_determination_a_ConstP;

/* Model entry point functions */
extern void attitude_determination_and_vehi_initialize(void);
extern void attitude_determination_and_vehi_step(void);
extern void attitude_determination_and_vehi_terminate(void);

/* Real-time Model object */
extern RT_MODEL_attitude_determinati_T *const attitude_determination_and_v_M;

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
