/*
 * attitude_determination_and_vehi.c
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

#include "attitude_determination_and_vehi.h"
#include "rtwtypes.h"
#include <string.h>
#include <math.h>
#include "attitude_determination_and_vehi_private.h"
#include "rt_nonfinite.h"

/* Block signals (default storage) */
B_attitude_determination_and__T attitude_determination_and_ve_B;

/* Block states (default storage) */
DW_attitude_determination_and_T attitude_determination_and_v_DW;

/* External inputs (root inport signals with default storage) */
ExtU_attitude_determination_a_T attitude_determination_and_ve_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_attitude_determination_a_T attitude_determination_and_ve_Y;

/* Real-time model */
static RT_MODEL_attitude_determinati_T attitude_determination_and_v_M_;
RT_MODEL_attitude_determinati_T *const attitude_determination_and_v_M =
  &attitude_determination_and_v_M_;

/* Forward declaration for local functions */
static void attitude_determinati_quatrotate(const real_T q[4], real_T v[3]);
static void attitude_determination_an_mrdiv(const real_T A[18], const real_T B[9],
  real_T Y[18]);
static void attitude_determina_quatmultiply(const real_T q[4], const real_T r[4],
  real_T qout[4]);
static real_T attitude_determination_and_norm(const real_T x[4]);

/*
 * Function for MATLAB Function: '<S1>/MEKF'
 * function v = quatrotate(q,v)
 */
static void attitude_determinati_quatrotate(const real_T q[4], real_T v[3])
{
  real_T y[9];
  real_T y_0[3];
  real_T y_tmp;
  real_T y_tmp_0;
  real_T y_tmp_1;
  real_T y_tmp_2;
  real_T y_tmp_3;
  real_T y_tmp_4;
  int32_T i;

  /* :  q0 = q(1); */
  /* :  q1 = q(2); */
  /* :  q2 = q(3); */
  /* :  q3 = q(4); */
  /* :  M = zeros(3); */
  /* :  M(1,1) = 1 - 2*q2^2 - 2*q3^2; */
  y_tmp_1 = q[3] * q[3] * 2.0;
  y_tmp_4 = (1.0 - q[2] * q[2] * 2.0) - y_tmp_1;
  y[0] = y_tmp_4;

  /* :  M(1,2) = 2*(q1*q2 + q0*q3); */
  y_tmp = q[1] * q[2];
  y_tmp_0 = q[0] * q[3];
  y[3] = (y_tmp + y_tmp_0) * 2.0;

  /* :  M(1,3) = 2*(q1*q3-q0*q2); */
  y_tmp_2 = q[1] * q[3];
  y_tmp_3 = q[0] * q[2];
  y[6] = (y_tmp_2 - y_tmp_3) * 2.0;

  /* :  M(2,1) = 2*(q1*q2-q0*q3); */
  y[1] = (y_tmp - y_tmp_0) * 2.0;

  /* :  M(2,2) = 1 - 2*q1^2 - 2*q3^2; */
  y[4] = (1.0 - q[1] * q[1] * 2.0) - y_tmp_1;

  /* :  M(2,3) = 2*(q2*q3+q0*q1); */
  y_tmp_1 = q[2] * q[3];
  y_tmp = q[0] * q[1];
  y[7] = (y_tmp_1 + y_tmp) * 2.0;

  /* :  M(3,1) = 2*(q1*q3+q0*q2); */
  y[2] = (y_tmp_2 + y_tmp_3) * 2.0;

  /* :  M(3,2) = 2*(q2*q3-q0*q1); */
  y[5] = (y_tmp_1 - y_tmp) * 2.0;

  /* :  M(3,3) = 1 - 2*q2^2 - 2*q3^2; */
  y[8] = y_tmp_4;

  /* :  v = (M*(v'))'; */
  y_tmp_1 = v[0];
  y_tmp_4 = v[1];
  y_tmp = v[2];
  for (i = 0; i < 3; i++) {
    y_tmp_0 = y[i] * y_tmp_1;
    y_tmp_0 += y[i + 3] * y_tmp_4;
    y_tmp_0 += y[i + 6] * y_tmp;
    y_0[i] = y_tmp_0;
  }

  v[0] = y_0[0];
  v[1] = y_0[1];
  v[2] = y_0[2];
}

/* Function for MATLAB Function: '<S1>/MEKF' */
static void attitude_determination_an_mrdiv(const real_T A[18], const real_T B[9],
  real_T Y[18])
{
  real_T b_A[9];
  real_T a21;
  real_T maxval;
  int32_T r1;
  int32_T r2;
  int32_T r3;
  int32_T rtemp;
  memcpy(&b_A[0], &B[0], 9U * sizeof(real_T));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = fabs(B[0]);
  a21 = fabs(B[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (fabs(B[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  b_A[r2] = B[r2] / B[r1];
  b_A[r3] /= b_A[r1];
  b_A[r2 + 3] -= b_A[r1 + 3] * b_A[r2];
  b_A[r3 + 3] -= b_A[r1 + 3] * b_A[r3];
  b_A[r2 + 6] -= b_A[r1 + 6] * b_A[r2];
  b_A[r3 + 6] -= b_A[r1 + 6] * b_A[r3];
  if (fabs(b_A[r3 + 3]) > fabs(b_A[r2 + 3])) {
    rtemp = r2;
    r2 = r3;
    r3 = rtemp;
  }

  b_A[r3 + 3] /= b_A[r2 + 3];
  b_A[r3 + 6] -= b_A[r3 + 3] * b_A[r2 + 6];
  for (rtemp = 0; rtemp < 6; rtemp++) {
    int32_T Y_tmp;
    int32_T Y_tmp_0;
    int32_T Y_tmp_1;
    Y_tmp = 6 * r1 + rtemp;
    Y[Y_tmp] = A[rtemp] / b_A[r1];
    Y_tmp_0 = 6 * r2 + rtemp;
    Y[Y_tmp_0] = A[rtemp + 6] - b_A[r1 + 3] * Y[Y_tmp];
    Y_tmp_1 = 6 * r3 + rtemp;
    Y[Y_tmp_1] = A[rtemp + 12] - b_A[r1 + 6] * Y[Y_tmp];
    Y[Y_tmp_0] /= b_A[r2 + 3];
    Y[Y_tmp_1] -= b_A[r2 + 6] * Y[Y_tmp_0];
    Y[Y_tmp_1] /= b_A[r3 + 6];
    Y[Y_tmp_0] -= b_A[r3 + 3] * Y[Y_tmp_1];
    Y[Y_tmp] -= Y[Y_tmp_1] * b_A[r3];
    Y[Y_tmp] -= Y[Y_tmp_0] * b_A[r2];
  }
}

/* Function for MATLAB Function: '<S1>/MEKF' */
static void attitude_determina_quatmultiply(const real_T q[4], const real_T r[4],
  real_T qout[4])
{
  qout[0] = ((q[0] * r[0] - q[1] * r[1]) - q[2] * r[2]) - q[3] * r[3];
  qout[1] = (q[0] * r[1] + r[0] * q[1]) + (q[2] * r[3] - r[2] * q[3]);
  qout[2] = (q[0] * r[2] + r[0] * q[2]) + (r[1] * q[3] - q[1] * r[3]);
  qout[3] = (q[0] * r[3] + r[0] * q[3]) + (q[1] * r[2] - r[1] * q[2]);
}

/* Function for MATLAB Function: '<S1>/MEKF' */
static real_T attitude_determination_and_norm(const real_T x[4])
{
  real_T absxk;
  real_T scale;
  real_T t;
  real_T y;
  scale = 3.3121686421112381E-170;
  absxk = fabs(x[0]);
  if (absxk > 3.3121686421112381E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.3121686421112381E-170;
    y = t * t;
  }

  absxk = fabs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = fabs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = fabs(x[3]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  return scale * sqrt(y);
}

/* Model step function */
void attitude_determination_and_vehi_step(void)
{
  real_T F[36];
  real_T F_0[36];
  real_T F_1[36];
  real_T P[36];
  real_T a[36];
  real_T e_a[36];
  real_T H[18];
  real_T K[18];
  real_T P_0[18];
  real_T H_0[9];
  real_T delta_x[6];
  real_T q_n2m[4];
  real_T tmp[4];
  real_T tmp_0[4];
  real_T beta[3];
  real_T tmp_1[3];
  real_T H_1;
  real_T H_2;
  real_T K_0;
  real_T P_1;
  real_T beta_0;
  int32_T F_tmp;
  int32_T H_tmp;
  int32_T a_tmp;
  int32_T i;
  int8_T b;
  static const int8_T b_0[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

  static const real_T c[9] = { 0.010000000000000002, 0.0, 0.0, 0.0,
    0.010000000000000002, 0.0, 0.0, 0.0, 0.010000000000000002 };

  static const int8_T e_a_0[36] = { -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 };

  /* DataStoreRead: '<S1>/Data Store Read1' */
  attitude_determination_and_ve_B.DataStoreRead1[0] =
    attitude_determination_and_v_DW.q_n2m[0];
  attitude_determination_and_ve_B.DataStoreRead1[1] =
    attitude_determination_and_v_DW.q_n2m[1];
  attitude_determination_and_ve_B.DataStoreRead1[2] =
    attitude_determination_and_v_DW.q_n2m[2];
  attitude_determination_and_ve_B.DataStoreRead1[3] =
    attitude_determination_and_v_DW.q_n2m[3];

  /* DataStoreRead: '<S1>/Data Store Read2' */
  attitude_determination_and_ve_B.DataStoreRead2[0] =
    attitude_determination_and_v_DW.beta[0];
  attitude_determination_and_ve_B.DataStoreRead2[1] =
    attitude_determination_and_v_DW.beta[1];
  attitude_determination_and_ve_B.DataStoreRead2[2] =
    attitude_determination_and_v_DW.beta[2];

  /* Outport: '<Root>/meas_quat_body' incorporates:
   *  Reshape: '<S1>/Reshape'
   */
  attitude_determination_and_ve_Y.meas_quat_body[0] =
    attitude_determination_and_ve_B.DataStoreRead1[0];
  attitude_determination_and_ve_Y.meas_quat_body[1] =
    attitude_determination_and_ve_B.DataStoreRead1[1];
  attitude_determination_and_ve_Y.meas_quat_body[2] =
    attitude_determination_and_ve_B.DataStoreRead1[2];
  attitude_determination_and_ve_Y.meas_quat_body[3] =
    attitude_determination_and_ve_B.DataStoreRead1[3];

  /* Reshape: '<S1>/Reshape2' */
  beta_0 = attitude_determination_and_ve_B.DataStoreRead2[0];

  /* Math: '<S1>/Transpose1' incorporates:
   *  Inport: '<Root>/sat_to_sun_unit_ref'
   */
  attitude_determination_and_ve_B.Transpose1[0] =
    attitude_determination_and_ve_U.sat_to_sun_unit_ref[0];

  /* Reshape: '<S1>/Reshape1' incorporates:
   *  Inport: '<Root>/earth_mag_field_ref'
   */
  attitude_determination_and_ve_B.Reshape1[0] =
    attitude_determination_and_ve_U.earth_mag_field_ref[0];

  /* MATLAB Function: '<S1>/MEKF' */
  beta[0] = beta_0;

  /* Reshape: '<S1>/Reshape2' */
  beta_0 = attitude_determination_and_ve_B.DataStoreRead2[1];

  /* Math: '<S1>/Transpose1' incorporates:
   *  Inport: '<Root>/sat_to_sun_unit_ref'
   */
  attitude_determination_and_ve_B.Transpose1[1] =
    attitude_determination_and_ve_U.sat_to_sun_unit_ref[1];

  /* Reshape: '<S1>/Reshape1' incorporates:
   *  Inport: '<Root>/earth_mag_field_ref'
   */
  attitude_determination_and_ve_B.Reshape1[1] =
    attitude_determination_and_ve_U.earth_mag_field_ref[1];

  /* MATLAB Function: '<S1>/MEKF' */
  beta[1] = beta_0;

  /* Reshape: '<S1>/Reshape2' */
  beta_0 = attitude_determination_and_ve_B.DataStoreRead2[2];

  /* Math: '<S1>/Transpose1' incorporates:
   *  Inport: '<Root>/sat_to_sun_unit_ref'
   */
  attitude_determination_and_ve_B.Transpose1[2] =
    attitude_determination_and_ve_U.sat_to_sun_unit_ref[2];

  /* Reshape: '<S1>/Reshape1' incorporates:
   *  Inport: '<Root>/earth_mag_field_ref'
   */
  attitude_determination_and_ve_B.Reshape1[2] =
    attitude_determination_and_ve_U.earth_mag_field_ref[2];

  /* MATLAB Function: '<S1>/MEKF' incorporates:
   *  Outport: '<Root>/meas_quat_body'
   */
  beta[2] = beta_0;
  q_n2m[0] = attitude_determination_and_ve_Y.meas_quat_body[0];
  q_n2m[1] = attitude_determination_and_ve_Y.meas_quat_body[1];
  q_n2m[2] = attitude_determination_and_ve_Y.meas_quat_body[2];
  q_n2m[3] = attitude_determination_and_ve_Y.meas_quat_body[3];
  attitude_determination_and_ve_B.beta[0] = beta[0];
  attitude_determination_and_ve_B.beta[1] = beta[1];
  attitude_determination_and_ve_B.beta[2] = beta[2];

  /* Outport: '<Root>/meas_quat_body' incorporates:
   *  MATLAB Function: '<S1>/MEKF'
   */
  attitude_determination_and_ve_Y.meas_quat_body[0] = q_n2m[0];
  attitude_determination_and_ve_Y.meas_quat_body[1] = q_n2m[1];
  attitude_determination_and_ve_Y.meas_quat_body[2] = q_n2m[2];
  attitude_determination_and_ve_Y.meas_quat_body[3] = q_n2m[3];
  for (i = 0; i < 36; i++) {
    /* DataStoreRead: '<S1>/Data Store Read' */
    P_1 = attitude_determination_and_v_DW.P[i];

    /* MATLAB Function: '<S1>/MEKF' */
    attitude_determination_and_ve_B.P[i] = P_1;
    P[i] = P_1;
  }

  /* MATLAB Function: '<S1>/MEKF' incorporates:
   *  Constant: '<S1>/Angular Velocity Mode Selection'
   *  Constant: '<S1>/Kalman Filter Timestep'
   *  Constant: '<S1>/Magnetometer Mode Selection'
   *  Constant: '<S1>/STEVE Mode Selection'
   *  Constant: '<S1>/Sun Sensor Mode Selection'
   *  Constant: '<S1>/mes_var_ss'
   *  Inport: '<Root>/mes_mag'
   *  Inport: '<Root>/mes_ss'
   *  Inport: '<Root>/omega'
   *  Inport: '<Root>/r_sat_com'
   *  Inport: '<Root>/steve_mes'
   *  Math: '<S1>/Transpose1'
   *  Outport: '<Root>/meas_quat_body'
   *  Reshape: '<S1>/Reshape1'
   */
  /* :  delta_x = zeros(6,1); */
  for (i = 0; i < 6; i++) {
    delta_x[i] = 0.0;
  }

  /* :  G = [-eye(3), zeros(3); */
  /* :      zeros(3), eye(3)]; */
  /* :  Q = [mes_var_ss^2*eye(3), zeros(3); */
  /* :      zeros(3), mes_var_mg^2*eye(3)]; */
  beta_0 = 0.25 * 0.25;

  /* :  if use_sun_sensor */
  if (1.0 != 0.0) {
    /* :  [q_n2m,P,delta_x] = mekf(mes_ss,ref_ss,P,mes_var_ss,q_n2m,delta_x); */
    /* :  b_est = quatrotate(q_n2m,r); */
    beta[0] = attitude_determination_and_ve_B.Transpose1[0];
    beta[1] = attitude_determination_and_ve_B.Transpose1[1];
    beta[2] = attitude_determination_and_ve_B.Transpose1[2];
    attitude_determinati_quatrotate
      (attitude_determination_and_ve_Y.meas_quat_body, beta);

    /* :  b01 = b_est(1); */
    /* :  b02 = b_est(2); */
    /* :  b03 = b_est(3); */
    /* :  H = [   0  -b03,  b02,  0,  0,  0; */
    /* :            b03,    0, -b01,  0,  0,  0; */
    /* :           -b02,  b01,    0,  0,  0,  0]; */
    H[0] = 0.0;
    H[3] = -beta[2];
    H[6] = beta[1];
    H[9] = 0.0;
    H[12] = 0.0;
    H[15] = 0.0;
    H[1] = beta[2];
    H[4] = 0.0;
    H[7] = -beta[0];
    H[10] = 0.0;
    H[13] = 0.0;
    H[16] = 0.0;
    H[2] = -beta[1];
    H[5] = beta[0];
    H[8] = 0.0;
    H[11] = 0.0;
    H[14] = 0.0;
    H[17] = 0.0;

    /* :  K = P*transpose(H)/(H*P*transpose(H)+R^2*eye(3)); */
    for (i = 0; i < 6; i++) {
      for (a_tmp = 0; a_tmp < 3; a_tmp++) {
        H_1 = 0.0;
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          H_2 = H[3 * H_tmp + a_tmp];
          H_1 += attitude_determination_and_ve_B.P[6 * i + H_tmp] * H_2;
          P_1 += attitude_determination_and_ve_B.P[6 * H_tmp + i] * H_2;
        }

        K[a_tmp + 3 * i] = H_1;
        P_0[i + 6 * a_tmp] = P_1;
      }
    }

    /* :  epsilon = (b - quatrotate(q_n2m,r)); */
    /* :  delta_x = delta_x + K*(epsilon'-H*delta_x); */
    for (i = 0; i < 3; i++) {
      for (a_tmp = 0; a_tmp < 3; a_tmp++) {
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          P_1 += K[3 * H_tmp + i] * H[3 * H_tmp + a_tmp];
        }

        H_tmp = 3 * a_tmp + i;
        H_0[H_tmp] = (real_T)b_0[H_tmp] * beta_0 + P_1;
      }

      P_1 = 0.0;
      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 += H[3 * a_tmp + i] * 0.0;
      }

      tmp_1[i] = (attitude_determination_and_ve_U.mes_ss[i] - beta[i]) - P_1;
    }

    attitude_determination_an_mrdiv(P_0, H_0, K);
    P_1 = tmp_1[0];
    H_1 = tmp_1[1];
    H_2 = tmp_1[2];
    for (i = 0; i < 6; i++) {
      K_0 = K[i] * P_1;
      K_0 += K[i + 6] * H_1;
      K_0 += K[i + 12] * H_2;
      delta_x[i] = K_0;
    }

    /* :  P = (eye(6) - K*H)*P; */
    memset(&F[0], 0, 36U * sizeof(real_T));
    for (i = 0; i < 6; i++) {
      F[i + 6 * i] = 1.0;
    }

    for (i = 0; i < 6; i++) {
      H_1 = K[i];
      H_2 = K[i + 6];
      K_0 = K[i + 12];
      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 = H[3 * a_tmp] * H_1;
        P_1 += H[3 * a_tmp + 1] * H_2;
        P_1 += H[3 * a_tmp + 2] * K_0;
        F_tmp = 6 * a_tmp + i;
        F_0[F_tmp] = F[F_tmp] - P_1;
      }

      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          P_1 += F_0[6 * H_tmp + i] * attitude_determination_and_ve_B.P[6 *
            a_tmp + H_tmp];
        }

        P[i + 6 * a_tmp] = P_1;
      }
    }
  }

  /* :  if use_magnometer */
  if (1.0 != 0.0) {
    /* :  [q_n2m,P,delta_x] = mekf(mes_mag,ref_mag,P,mes_var_mg,q_n2m,delta_x); */
    /* :  b_est = quatrotate(q_n2m,r); */
    beta[0] = attitude_determination_and_ve_B.Reshape1[0];
    beta[1] = attitude_determination_and_ve_B.Reshape1[1];
    beta[2] = attitude_determination_and_ve_B.Reshape1[2];
    attitude_determinati_quatrotate
      (attitude_determination_and_ve_Y.meas_quat_body, beta);

    /* :  b01 = b_est(1); */
    /* :  b02 = b_est(2); */
    /* :  b03 = b_est(3); */
    /* :  H = [   0  -b03,  b02,  0,  0,  0; */
    /* :            b03,    0, -b01,  0,  0,  0; */
    /* :           -b02,  b01,    0,  0,  0,  0]; */
    H[0] = 0.0;
    H[3] = -beta[2];
    H[6] = beta[1];
    H[9] = 0.0;
    H[12] = 0.0;
    H[15] = 0.0;
    H[1] = beta[2];
    H[4] = 0.0;
    H[7] = -beta[0];
    H[10] = 0.0;
    H[13] = 0.0;
    H[16] = 0.0;
    H[2] = -beta[1];
    H[5] = beta[0];
    H[8] = 0.0;
    H[11] = 0.0;
    H[14] = 0.0;
    H[17] = 0.0;

    /* :  K = P*transpose(H)/(H*P*transpose(H)+R^2*eye(3)); */
    for (i = 0; i < 6; i++) {
      for (a_tmp = 0; a_tmp < 3; a_tmp++) {
        H_1 = 0.0;
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          H_2 = H[3 * H_tmp + a_tmp];
          H_1 += P[6 * i + H_tmp] * H_2;
          P_1 += P[6 * H_tmp + i] * H_2;
        }

        K[a_tmp + 3 * i] = H_1;
        P_0[i + 6 * a_tmp] = P_1;
      }
    }

    /* :  epsilon = (b - quatrotate(q_n2m,r)); */
    /* :  delta_x = delta_x + K*(epsilon'-H*delta_x); */
    for (i = 0; i < 3; i++) {
      for (a_tmp = 0; a_tmp < 3; a_tmp++) {
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          P_1 += K[3 * H_tmp + i] * H[3 * H_tmp + a_tmp];
        }

        H_tmp = 3 * a_tmp + i;
        H_0[H_tmp] = (real_T)b_0[H_tmp] * beta_0 + P_1;
      }

      P_1 = 0.0;
      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 += H[3 * a_tmp + i] * delta_x[a_tmp];
      }

      tmp_1[i] = (attitude_determination_and_ve_U.mes_mag[i] - beta[i]) - P_1;
    }

    attitude_determination_an_mrdiv(P_0, H_0, K);
    P_1 = tmp_1[0];
    H_1 = tmp_1[1];
    H_2 = tmp_1[2];
    for (i = 0; i < 6; i++) {
      K_0 = K[i] * P_1;
      K_0 += K[i + 6] * H_1;
      K_0 += K[i + 12] * H_2;
      delta_x[i] += K_0;
    }

    /* :  P = (eye(6) - K*H)*P; */
    memset(&F[0], 0, 36U * sizeof(real_T));
    for (i = 0; i < 6; i++) {
      F[i + 6 * i] = 1.0;
    }

    for (i = 0; i < 6; i++) {
      H_1 = K[i];
      H_2 = K[i + 6];
      K_0 = K[i + 12];
      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 = H[3 * a_tmp] * H_1;
        P_1 += H[3 * a_tmp + 1] * H_2;
        P_1 += H[3 * a_tmp + 2] * K_0;
        F_tmp = 6 * a_tmp + i;
        F_0[F_tmp] = F[F_tmp] - P_1;
      }

      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        H_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          H_1 += F_0[6 * H_tmp + i] * P[6 * a_tmp + H_tmp];
        }

        F_1[i + 6 * a_tmp] = H_1;
      }
    }

    memcpy(&P[0], &F_1[0], 36U * sizeof(real_T));
  }

  /* :  if use_steve_mode */
  if (0.0 != 0.0) {
    /* :  steve_mes_reshape = reshape(steve_mes,[1 3]); */
    /* :  steve_ref_reshape = reshape(steve_ref,[1 3]); */
    /* :  [q_n2m,P,delta_x] = mekf(steve_mes_reshape,steve_ref_reshape,P,0.1,q_n2m,delta_x); */
    /* :  b_est = quatrotate(q_n2m,r); */
    beta[0] = attitude_determination_and_ve_U.r_sat_com_ax1[0];
    beta[1] = attitude_determination_and_ve_U.r_sat_com_ax1[1];
    beta[2] = attitude_determination_and_ve_U.r_sat_com_ax1[2];
    attitude_determinati_quatrotate
      (attitude_determination_and_ve_Y.meas_quat_body, beta);

    /* :  b01 = b_est(1); */
    /* :  b02 = b_est(2); */
    /* :  b03 = b_est(3); */
    /* :  H = [   0  -b03,  b02,  0,  0,  0; */
    /* :            b03,    0, -b01,  0,  0,  0; */
    /* :           -b02,  b01,    0,  0,  0,  0]; */
    H[0] = 0.0;
    H[3] = -beta[2];
    H[6] = beta[1];
    H[9] = 0.0;
    H[12] = 0.0;
    H[15] = 0.0;
    H[1] = beta[2];
    H[4] = 0.0;
    H[7] = -beta[0];
    H[10] = 0.0;
    H[13] = 0.0;
    H[16] = 0.0;
    H[2] = -beta[1];
    H[5] = beta[0];
    H[8] = 0.0;
    H[11] = 0.0;
    H[14] = 0.0;
    H[17] = 0.0;

    /* :  K = P*transpose(H)/(H*P*transpose(H)+R^2*eye(3)); */
    for (i = 0; i < 6; i++) {
      for (a_tmp = 0; a_tmp < 3; a_tmp++) {
        H_1 = 0.0;
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          H_2 = H[3 * H_tmp + a_tmp];
          H_1 += P[6 * i + H_tmp] * H_2;
          P_1 += P[6 * H_tmp + i] * H_2;
        }

        K[a_tmp + 3 * i] = H_1;
        P_0[i + 6 * a_tmp] = P_1;
      }
    }

    /* :  epsilon = (b - quatrotate(q_n2m,r)); */
    /* :  delta_x = delta_x + K*(epsilon'-H*delta_x); */
    for (i = 0; i < 3; i++) {
      for (a_tmp = 0; a_tmp < 3; a_tmp++) {
        P_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          P_1 += K[3 * H_tmp + i] * H[3 * H_tmp + a_tmp];
        }

        H_tmp = 3 * a_tmp + i;
        H_0[H_tmp] = c[H_tmp] + P_1;
      }

      P_1 = 0.0;
      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 += H[3 * a_tmp + i] * delta_x[a_tmp];
      }

      tmp_1[i] = (attitude_determination_and_ve_U.steve_mes[i] - beta[i]) - P_1;
    }

    attitude_determination_an_mrdiv(P_0, H_0, K);
    P_1 = tmp_1[0];
    H_1 = tmp_1[1];
    H_2 = tmp_1[2];
    for (i = 0; i < 6; i++) {
      K_0 = K[i] * P_1;
      K_0 += K[i + 6] * H_1;
      K_0 += K[i + 12] * H_2;
      delta_x[i] += K_0;
    }

    /* :  P = (eye(6) - K*H)*P; */
    memset(&F[0], 0, 36U * sizeof(real_T));
    for (i = 0; i < 6; i++) {
      F[i + 6 * i] = 1.0;
    }

    for (i = 0; i < 6; i++) {
      H_1 = K[i];
      H_2 = K[i + 6];
      K_0 = K[i + 12];
      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        P_1 = H[3 * a_tmp] * H_1;
        P_1 += H[3 * a_tmp + 1] * H_2;
        P_1 += H[3 * a_tmp + 2] * K_0;
        F_tmp = 6 * a_tmp + i;
        F_0[F_tmp] = F[F_tmp] - P_1;
      }

      for (a_tmp = 0; a_tmp < 6; a_tmp++) {
        H_1 = 0.0;
        for (H_tmp = 0; H_tmp < 6; H_tmp++) {
          H_1 += F_0[6 * H_tmp + i] * P[6 * a_tmp + H_tmp];
        }

        F_1[i + 6 * a_tmp] = H_1;
      }
    }

    memcpy(&P[0], &F_1[0], 36U * sizeof(real_T));
  }

  /* :  x_extra = [0 ; delta_x(1:3)]'; */
  /* :  q_n2m = q_n2m + (0.5*quatmultiply(q_n2m,x_extra)); */
  tmp[0] = 0.0;
  tmp[1] = delta_x[0];
  tmp[2] = delta_x[1];
  tmp[3] = delta_x[2];
  attitude_determina_quatmultiply(attitude_determination_and_ve_Y.meas_quat_body,
    tmp, tmp_0);
  q_n2m[0] = 0.5 * tmp_0[0] + attitude_determination_and_ve_Y.meas_quat_body[0];
  q_n2m[1] = 0.5 * tmp_0[1] + attitude_determination_and_ve_Y.meas_quat_body[1];
  q_n2m[2] = 0.5 * tmp_0[2] + attitude_determination_and_ve_Y.meas_quat_body[2];
  q_n2m[3] = 0.5 * tmp_0[3] + attitude_determination_and_ve_Y.meas_quat_body[3];

  /* :  q_n2m = q_n2m/norm(q_n2m); */
  P_1 = attitude_determination_and_norm(q_n2m);
  q_n2m[0] /= P_1;
  q_n2m[1] /= P_1;
  q_n2m[2] /= P_1;
  q_n2m[3] /= P_1;

  /* :  hat_omega = zeros(3,1); */
  attitude_determination_and_ve_B.hat_omega[0] = 0.0;
  attitude_determination_and_ve_B.hat_omega[1] = 0.0;
  attitude_determination_and_ve_B.hat_omega[2] = 0.0;

  /* :  if use_ang_vel */
  if (1.0 != 0.0) {
    /* :  hat_omega(1) = omega(1) - beta(1); */
    attitude_determination_and_ve_B.hat_omega[0] =
      attitude_determination_and_ve_U.omega[0] -
      attitude_determination_and_ve_B.beta[0];

    /* :  hat_omega(2) = omega(2) - beta(2); */
    attitude_determination_and_ve_B.hat_omega[1] =
      attitude_determination_and_ve_U.omega[1] -
      attitude_determination_and_ve_B.beta[1];

    /* :  hat_omega(3) = omega(3) - beta(3); */
    attitude_determination_and_ve_B.hat_omega[2] =
      attitude_determination_and_ve_U.omega[2] -
      attitude_determination_and_ve_B.beta[2];
  }

  /* :  x_extra = [0; hat_omega]'; */
  /* :  dot_q_n2m = 0.5*quatmultiply(q_n2m,x_extra); */
  /* :  q_n2m = q_n2m + dot_q_n2m*dt; */
  tmp[0] = 0.0;
  tmp[1] = attitude_determination_and_ve_B.hat_omega[0];
  tmp[2] = attitude_determination_and_ve_B.hat_omega[1];
  tmp[3] = attitude_determination_and_ve_B.hat_omega[2];
  attitude_determina_quatmultiply(q_n2m, tmp, tmp_0);
  q_n2m[0] += 0.5 * tmp_0[0] * 0.1;
  q_n2m[1] += 0.5 * tmp_0[1] * 0.1;
  q_n2m[2] += 0.5 * tmp_0[2] * 0.1;
  q_n2m[3] += 0.5 * tmp_0[3] * 0.1;

  /* :  q_n2m = q_n2m/norm(q_n2m); */
  P_1 = attitude_determination_and_norm(q_n2m);

  /* Outport: '<Root>/meas_quat_body' incorporates:
   *  MATLAB Function: '<S1>/MEKF'
   */
  attitude_determination_and_ve_Y.meas_quat_body[0] = q_n2m[0] / P_1;
  attitude_determination_and_ve_Y.meas_quat_body[1] = q_n2m[1] / P_1;
  attitude_determination_and_ve_Y.meas_quat_body[2] = q_n2m[2] / P_1;
  attitude_determination_and_ve_Y.meas_quat_body[3] = q_n2m[3] / P_1;

  /* MATLAB Function: '<S1>/MEKF' */
  /* :  w = [ 0  -hat_omega(3),  hat_omega(2); */
  /* :            hat_omega(3),    0, -hat_omega(1); */
  /* :           -hat_omega(2),  hat_omega(1),    0]; */
  /* :  F = [-w, eye(3); */
  /* :          zeros(3), zeros(3)]; */
  F[0] = -0.0;
  F[6] = attitude_determination_and_ve_B.hat_omega[2];
  F[12] = -attitude_determination_and_ve_B.hat_omega[1];
  F[1] = -attitude_determination_and_ve_B.hat_omega[2];
  F[7] = -0.0;
  F[13] = attitude_determination_and_ve_B.hat_omega[0];
  F[2] = attitude_determination_and_ve_B.hat_omega[1];
  F[8] = -attitude_determination_and_ve_B.hat_omega[0];
  F[14] = -0.0;
  for (i = 0; i < 3; i++) {
    F_tmp = (i + 3) * 6;
    F[F_tmp] = b_0[3 * i];
    F[F_tmp + 1] = b_0[3 * i + 1];
    F[F_tmp + 2] = b_0[3 * i + 2];
  }

  for (i = 0; i < 6; i++) {
    F[6 * i + 3] = 0.0;
    F[6 * i + 4] = 0.0;
    F[6 * i + 5] = 0.0;
  }

  /* :  dot_P = F*P + P*F'+G*Q*G'; */
  /* :  P = P + dot_P*dt; */
  for (i = 0; i < 6; i++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      H_1 = 0.0;
      P_1 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        F_tmp = 6 * H_tmp + i;
        H_1 += P[6 * a_tmp + H_tmp] * F[F_tmp];
        P_1 += F[6 * H_tmp + a_tmp] * P[F_tmp];
      }

      H_tmp = 6 * a_tmp + i;
      F_1[H_tmp] = P_1;
      F_0[H_tmp] = H_1;
    }
  }

  for (i = 0; i < 3; i++) {
    b = b_0[3 * i];
    P_1 = beta_0 * (real_T)b;
    a[6 * i] = P_1;
    a_tmp = (i + 3) * 6;
    a[a_tmp] = 0.0;
    a[6 * i + 3] = 0.0;
    a[a_tmp + 3] = P_1;
    b = b_0[3 * i + 1];
    P_1 = beta_0 * (real_T)b;
    a[6 * i + 1] = P_1;
    a[a_tmp + 1] = 0.0;
    a[6 * i + 4] = 0.0;
    a[a_tmp + 4] = P_1;
    b = b_0[3 * i + 2];
    P_1 = beta_0 * (real_T)b;
    a[6 * i + 2] = P_1;
    a[a_tmp + 2] = 0.0;
    a[6 * i + 5] = 0.0;
    a[a_tmp + 5] = P_1;
  }

  for (i = 0; i < 6; i++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      beta_0 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        beta_0 += (real_T)e_a_0[6 * H_tmp + i] * a[6 * a_tmp + H_tmp];
      }

      e_a[i + 6 * a_tmp] = beta_0;
    }

    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_1 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_1 += e_a[6 * H_tmp + i] * (real_T)e_a_0[6 * a_tmp + H_tmp];
      }

      F_tmp = 6 * a_tmp + i;
      F[F_tmp] = (F_0[F_tmp] + F_1[F_tmp]) + P_1;
    }
  }

  /* DataStoreWrite: '<S1>/Bias of Gyroscope' */
  attitude_determination_and_v_DW.beta[0] =
    attitude_determination_and_ve_B.beta[0];
  attitude_determination_and_v_DW.beta[1] =
    attitude_determination_and_ve_B.beta[1];
  attitude_determination_and_v_DW.beta[2] =
    attitude_determination_and_ve_B.beta[2];
  for (i = 0; i < 36; i++) {
    /* MATLAB Function: '<S1>/MEKF' incorporates:
     *  Constant: '<S1>/Kalman Filter Timestep'
     */
    P_1 = F[i] * 0.1 + P[i];
    attitude_determination_and_ve_B.P[i] = P_1;

    /* DataStoreWrite: '<S1>/Data Store Write' */
    attitude_determination_and_v_DW.P[i] = P_1;
  }

  /* DataStoreWrite: '<S1>/Data Store Write1' incorporates:
   *  Outport: '<Root>/meas_quat_body'
   */
  attitude_determination_and_v_DW.q_n2m[0] =
    attitude_determination_and_ve_Y.meas_quat_body[0];
  attitude_determination_and_v_DW.q_n2m[1] =
    attitude_determination_and_ve_Y.meas_quat_body[1];
  attitude_determination_and_v_DW.q_n2m[2] =
    attitude_determination_and_ve_Y.meas_quat_body[2];
  attitude_determination_and_v_DW.q_n2m[3] =
    attitude_determination_and_ve_Y.meas_quat_body[3];

  /* Outport: '<Root>/meas_ang_vel_body' incorporates:
   *  Math: '<S1>/Transpose2'
   */
  attitude_determination_and_ve_Y.meas_ang_vel_body[0] =
    attitude_determination_and_ve_B.hat_omega[0];
  attitude_determination_and_ve_Y.meas_ang_vel_body[1] =
    attitude_determination_and_ve_B.hat_omega[1];
  attitude_determination_and_ve_Y.meas_ang_vel_body[2] =
    attitude_determination_and_ve_B.hat_omega[2];

  /* Matfile logging */
  rt_UpdateTXYLogVars(attitude_determination_and_v_M->rtwLogInfo,
                      (&attitude_determination_and_v_M->Timing.taskTime0));

  /* signal main to stop simulation */
  {                                    /* Sample time: [0.05s, 0.0s] */
    if ((rtmGetTFinal(attitude_determination_and_v_M)!=-1) &&
        !((rtmGetTFinal(attitude_determination_and_v_M)-
           attitude_determination_and_v_M->Timing.taskTime0) >
          attitude_determination_and_v_M->Timing.taskTime0 * (DBL_EPSILON))) {
      rtmSetErrorStatus(attitude_determination_and_v_M, "Simulation finished");
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
  if (!(++attitude_determination_and_v_M->Timing.clockTick0)) {
    ++attitude_determination_and_v_M->Timing.clockTickH0;
  }

  attitude_determination_and_v_M->Timing.taskTime0 =
    attitude_determination_and_v_M->Timing.clockTick0 *
    attitude_determination_and_v_M->Timing.stepSize0 +
    attitude_determination_and_v_M->Timing.clockTickH0 *
    attitude_determination_and_v_M->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
void attitude_determination_and_vehi_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)attitude_determination_and_v_M, 0,
                sizeof(RT_MODEL_attitude_determinati_T));
  rtmSetTFinal(attitude_determination_and_v_M, 7200.0);
  attitude_determination_and_v_M->Timing.stepSize0 = 0.05;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    attitude_determination_and_v_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(attitude_determination_and_v_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(attitude_determination_and_v_M->rtwLogInfo, (NULL));
    rtliSetLogT(attitude_determination_and_v_M->rtwLogInfo, "");
    rtliSetLogX(attitude_determination_and_v_M->rtwLogInfo, "");
    rtliSetLogXFinal(attitude_determination_and_v_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(attitude_determination_and_v_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(attitude_determination_and_v_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(attitude_determination_and_v_M->rtwLogInfo, 0);
    rtliSetLogDecimation(attitude_determination_and_v_M->rtwLogInfo, 1);
    rtliSetLogY(attitude_determination_and_v_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(attitude_determination_and_v_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(attitude_determination_and_v_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &attitude_determination_and_ve_B), 0,
                sizeof(B_attitude_determination_and__T));

  /* states (dwork) */
  (void) memset((void *)&attitude_determination_and_v_DW, 0,
                sizeof(DW_attitude_determination_and_T));

  /* external inputs */
  (void)memset(&attitude_determination_and_ve_U, 0, sizeof
               (ExtU_attitude_determination_a_T));

  /* external outputs */
  (void)memset(&attitude_determination_and_ve_Y, 0, sizeof
               (ExtY_attitude_determination_a_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(attitude_determination_and_v_M->rtwLogInfo,
    0.0, rtmGetTFinal(attitude_determination_and_v_M),
    attitude_determination_and_v_M->Timing.stepSize0, (&rtmGetErrorStatus
    (attitude_determination_and_v_M)));

  /* Start for DataStoreMemory: '<S1>/Data Store Memory' */
  memcpy(&attitude_determination_and_v_DW.P[0],
         attitude_determination_a_ConstP.DataStoreMemory_InitialValue, 36U *
         sizeof(real_T));

  /* Start for DataStoreMemory: '<S1>/Data Store Memory1' */
  attitude_determination_and_v_DW.q_n2m[0] = 1.0;
  attitude_determination_and_v_DW.q_n2m[1] = 0.0;
  attitude_determination_and_v_DW.q_n2m[2] = 0.0;
  attitude_determination_and_v_DW.q_n2m[3] = 0.0;

  /* Start for DataStoreMemory: '<S1>/Data Store Memory2' */
  attitude_determination_and_v_DW.beta[0] = 0.0;
  attitude_determination_and_v_DW.beta[1] = 0.0;
  attitude_determination_and_v_DW.beta[2] = 0.0;
}

/* Model terminate function */
void attitude_determination_and_vehi_terminate(void)
{
  /* (no terminate code required) */
}
