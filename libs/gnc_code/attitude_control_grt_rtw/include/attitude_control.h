/*
 * attitude_control.h
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

#ifndef RTW_HEADER_attitude_control_h_
#define RTW_HEADER_attitude_control_h_
#ifndef attitude_control_COMMON_INCLUDES_
#define attitude_control_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                                 /* attitude_control_COMMON_INCLUDES_ */

#include "attitude_control_types.h"
#include "rt_nonfinite.h"
#include "rtGetNaN.h"
#include <float.h>
#include <string.h>
#include <stddef.h>

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
  real_T Product;                      /* '<S5>/Product' */
  real_T Product1;                     /* '<S5>/Product1' */
  real_T Product2;                     /* '<S5>/Product2' */
  real_T Product3;                     /* '<S5>/Product3' */
  real_T Sum;                          /* '<S5>/Sum' */
  real_T Divide;                       /* '<S2>/Divide' */
  real_T Product_d;                    /* '<S6>/Product' */
  real_T UnaryMinus;                   /* '<S4>/Unary Minus' */
  real_T Divide1;                      /* '<S2>/Divide1' */
  real_T Product1_o;                   /* '<S6>/Product1' */
  real_T UnaryMinus1;                  /* '<S4>/Unary Minus1' */
  real_T Divide2;                      /* '<S2>/Divide2' */
  real_T Product2_i;                   /* '<S6>/Product2' */
  real_T UnaryMinus2;                  /* '<S4>/Unary Minus2' */
  real_T Divide3;                      /* '<S2>/Divide3' */
  real_T Product3_p;                   /* '<S6>/Product3' */
  real_T Sum_d;                        /* '<S6>/Sum' */
  real_T Sign;                         /* '<S1>/Sign' */
  real_T Product_p;                    /* '<S7>/Product' */
  real_T Product1_f;                   /* '<S7>/Product1' */
  real_T Product2_m;                   /* '<S7>/Product2' */
  real_T Product3_o;                   /* '<S7>/Product3' */
  real_T Sum_m;                        /* '<S7>/Sum' */
  real_T Product_pw;                   /* '<S8>/Product' */
  real_T Product1_n;                   /* '<S8>/Product1' */
  real_T Product2_iw;                  /* '<S8>/Product2' */
  real_T Product3_e;                   /* '<S8>/Product3' */
  real_T Sum_l;                        /* '<S8>/Sum' */
  real_T Product_i;                    /* '<S9>/Product' */
  real_T Product1_h;                   /* '<S9>/Product1' */
  real_T Product2_ig;                  /* '<S9>/Product2' */
  real_T Product3_d;                   /* '<S9>/Product3' */
  real_T Sum_o;                        /* '<S9>/Sum' */
  real_T k_p[3];                       /* '<S1>/k_p' */
  real_T Product_k[3];                 /* '<S1>/Product' */
  real_T k_d[3];                       /* '<S1>/k_d' */
} B_attitude_control_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T est_curr_ang_vel_body[3];     /* '<Root>/ang_vel_body' */
  real_T est_curr_quat_body[4];        /* '<Root>/curr_quat_body' */
  real_T com_quat_body[4];             /* '<Root>/com_quat_body' */
  real_T mag_field_body[3];            /* '<Root>/mag_field_body' */
} ExtU_attitude_control_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T comm_wheel_torque_body[3];    /* '<Root>/comm_wheel_torque_body' */
  real_T comm_mag_dipole_body[3];      /* '<Root>/comm_mag_dipole_body' */
} ExtY_attitude_control_T;


/* Parameters (default storage) */
struct P_attitude_control_T_ {
  real_T k_p_Gain;                     /* Expression: 0.2
                                        * Referenced by: '<S1>/k_p'
                                        */
  real_T k_d_Gain;                     /* Expression: 0.1
                                        * Referenced by: '<S1>/k_d'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_attitude_control_T {
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
extern B_attitude_control_T attitude_control_B;

/* External inputs (root inport signals with default storage) */
extern ExtU_attitude_control_T attitude_control_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_attitude_control_T attitude_control_Y;

/* Model entry point functions */
extern void attitude_control_initialize(void);
extern void attitude_control_step(void);
extern void attitude_control_terminate(void);

/* Real-time Model object */
extern RT_MODEL_attitude_control_T *const attitude_control_M;

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
