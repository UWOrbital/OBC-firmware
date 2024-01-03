/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: attitude_determination_and_vehi.c
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

#include "attitude_determination_and_vehi.h"
#include "rtwtypes.h"
#include <string.h>
#include <math.h>

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;

/* Forward declaration for local functions */
static void quatrotate(const real_T q[4], real_T v[3]);
static void mrdiv(const real_T A[18], const real_T B_0[9], real_T Y[18]);
static void quatmultiply(const real_T q[4], const real_T r[4], real_T qout[4]);
static real_T norm(const real_T x[4]);

/* Function for MATLAB Function: '<S1>/MEKF' */
static void quatrotate(const real_T q[4], real_T v[3])
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
  y_tmp_1 = q[3] * q[3] * 2.0;
  y_tmp_4 = (1.0 - q[2] * q[2] * 2.0) - y_tmp_1;
  y[0] = y_tmp_4;
  y_tmp = q[1] * q[2];
  y_tmp_0 = q[0] * q[3];
  y[3] = (y_tmp + y_tmp_0) * 2.0;
  y_tmp_2 = q[1] * q[3];
  y_tmp_3 = q[0] * q[2];
  y[6] = (y_tmp_2 - y_tmp_3) * 2.0;
  y[1] = (y_tmp - y_tmp_0) * 2.0;
  y[4] = (1.0 - q[1] * q[1] * 2.0) - y_tmp_1;
  y_tmp_1 = q[2] * q[3];
  y_tmp = q[0] * q[1];
  y[7] = (y_tmp_1 + y_tmp) * 2.0;
  y[2] = (y_tmp_2 + y_tmp_3) * 2.0;
  y[5] = (y_tmp_1 - y_tmp) * 2.0;
  y[8] = y_tmp_4;
  y_tmp_1 = v[1];
  y_tmp_4 = v[0];
  y_tmp = v[2];
  for (i = 0; i < 3; i++) {
    y_0[i] = (y[i + 3] * y_tmp_1 + y[i] * y_tmp_4) + y[i + 6] * y_tmp;
  }

  v[0] = y_0[0];
  v[1] = y_0[1];
  v[2] = y_0[2];
}

/* Function for MATLAB Function: '<S1>/MEKF' */
static void mrdiv(const real_T A[18], const real_T B_0[9], real_T Y[18])
{
  real_T b_A[9];
  real_T a21;
  real_T maxval;
  int32_T r1;
  int32_T r2;
  int32_T r3;
  int32_T rtemp;
  memcpy(&b_A[0], &B_0[0], 9U * sizeof(real_T));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = fabs(B_0[0]);
  a21 = fabs(B_0[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (fabs(B_0[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  b_A[r2] = B_0[r2] / B_0[r1];
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
static void quatmultiply(const real_T q[4], const real_T r[4], real_T qout[4])
{
  qout[0] = ((q[0] * r[0] - q[1] * r[1]) - q[2] * r[2]) - q[3] * r[3];
  qout[1] = (q[0] * r[1] + r[0] * q[1]) + (q[2] * r[3] - r[2] * q[3]);
  qout[2] = (q[0] * r[2] + r[0] * q[2]) + (r[1] * q[3] - q[1] * r[3]);
  qout[3] = (q[0] * r[3] + r[0] * q[3]) + (q[1] * r[2] - r[1] * q[2]);
}

/* Function for MATLAB Function: '<S1>/MEKF' */
static real_T norm(const real_T x[4])
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
  real_T P_0[36];
  real_T P_o_tmp_0[36];
  real_T a[36];
  real_T H[18];
  real_T H_0[18];
  real_T K[18];
  real_T P_1[18];
  real_T H_1[9];
  real_T delta_x[6];
  real_T q_n2m[4];
  real_T tmp[4];
  real_T tmp_0[4];
  real_T rtb_hat_omega[3];
  real_T tmp_1[3];
  real_T F_2;
  real_T P_2;
  real_T rtb_q_n2m_idx_0;
  real_T rtb_q_n2m_idx_1;
  real_T rtb_q_n2m_idx_2;
  real_T rtb_q_n2m_idx_3;
  int32_T F_tmp;
  int32_T H_tmp;
  int32_T a_tmp;
  int32_T k;
  int8_T P_o_tmp[36];
  int8_T K_tmp[9];
  static const int8_T b[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

  static const int8_T e_a[36] = { -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 };

  /* MATLAB Function: '<S1>/MEKF' incorporates:
   *  Constant: '<S1>/Kalman Filter Timestep'
   *  DataStoreRead: '<S1>/Data Store Read1'
   *  DataStoreRead: '<S1>/Data Store Read2'
   *  DataStoreWrite: '<S1>/Data Store Write'
   *  Inport: '<Root>/earth_mag_field_ref'
   *  Inport: '<Root>/mes_mag'
   *  Inport: '<Root>/mes_ss'
   *  Inport: '<Root>/omega'
   *  Inport: '<Root>/sat_to_sun_unit_ref'
   *  Math: '<S1>/Transpose1'
   */
  rtb_hat_omega[0] = rtU.sat_to_sun_unit_ref[0];
  rtb_hat_omega[1] = rtU.sat_to_sun_unit_ref[1];
  rtb_hat_omega[2] = rtU.sat_to_sun_unit_ref[2];
  quatrotate(rtDW.q_n2m, rtb_hat_omega);
  H[0] = 0.0;
  H[3] = -rtb_hat_omega[2];
  H[6] = rtb_hat_omega[1];
  H[9] = 0.0;
  H[12] = 0.0;
  H[15] = 0.0;
  H[1] = rtb_hat_omega[2];
  H[4] = 0.0;
  H[7] = -rtb_hat_omega[0];
  H[10] = 0.0;
  H[13] = 0.0;
  H[16] = 0.0;
  H[2] = -rtb_hat_omega[1];
  H[5] = rtb_hat_omega[0];
  H[8] = 0.0;
  H[11] = 0.0;
  H[14] = 0.0;
  H[17] = 0.0;
  for (k = 0; k < 3; k++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      K[a_tmp + 6 * k] = H[3 * a_tmp + k];
    }
  }

  for (k = 0; k < 9; k++) {
    K_tmp[k] = b[k];
  }

  for (k = 0; k < 6; k++) {
    for (a_tmp = 0; a_tmp < 3; a_tmp++) {
      rtb_q_n2m_idx_0 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        rtb_q_n2m_idx_0 += H[3 * H_tmp + a_tmp] * rtDW.P_o[6 * k + H_tmp];
      }

      H_0[a_tmp + 3 * k] = rtb_q_n2m_idx_0;
    }
  }

  for (k = 0; k < 3; k++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += rtDW.P_o[6 * H_tmp + a_tmp] * K[6 * k + H_tmp];
      }

      P_1[a_tmp + 6 * k] = P_2;
    }

    for (a_tmp = 0; a_tmp < 3; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += H_0[3 * H_tmp + k] * K[6 * a_tmp + H_tmp];
      }

      H_tmp = 3 * a_tmp + k;
      H_1[H_tmp] = (real_T)K_tmp[H_tmp] * 0.0625 + P_2;
    }
  }

  mrdiv(P_1, H_1, K);
  for (k = 0; k < 3; k++) {
    P_2 = 0.0;
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 += H[3 * a_tmp + k] * 0.0;
    }

    tmp_1[k] = (rtU.mes_ss[k] - rtb_hat_omega[k]) - P_2;
  }

  P_2 = tmp_1[1];
  rtb_q_n2m_idx_0 = tmp_1[0];
  rtb_q_n2m_idx_1 = tmp_1[2];
  for (k = 0; k < 6; k++) {
    delta_x[k] = (K[k + 6] * P_2 + K[k] * rtb_q_n2m_idx_0) + K[k + 12] *
      rtb_q_n2m_idx_1;
  }

  memset(&F[0], 0, 36U * sizeof(real_T));
  for (k = 0; k < 6; k++) {
    F[k + 6 * k] = 1.0;
  }

  for (k = 0; k < 6; k++) {
    rtb_q_n2m_idx_0 = K[k + 6];
    rtb_q_n2m_idx_1 = K[k];
    rtb_q_n2m_idx_2 = K[k + 12];
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      F_tmp = 6 * a_tmp + k;
      F_0[F_tmp] = F[F_tmp] - ((H[3 * a_tmp + 1] * rtb_q_n2m_idx_0 + H[3 * a_tmp]
        * rtb_q_n2m_idx_1) + H[3 * a_tmp + 2] * rtb_q_n2m_idx_2);
    }

    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += F_0[6 * H_tmp + k] * rtDW.P_o[6 * a_tmp + H_tmp];
      }

      P_0[k + 6 * a_tmp] = P_2;
    }
  }

  rtb_hat_omega[0] = rtU.earth_mag_field_ref[0];
  rtb_hat_omega[1] = rtU.earth_mag_field_ref[1];
  rtb_hat_omega[2] = rtU.earth_mag_field_ref[2];
  quatrotate(rtDW.q_n2m, rtb_hat_omega);
  H[0] = 0.0;
  H[3] = -rtb_hat_omega[2];
  H[6] = rtb_hat_omega[1];
  H[9] = 0.0;
  H[12] = 0.0;
  H[15] = 0.0;
  H[1] = rtb_hat_omega[2];
  H[4] = 0.0;
  H[7] = -rtb_hat_omega[0];
  H[10] = 0.0;
  H[13] = 0.0;
  H[16] = 0.0;
  H[2] = -rtb_hat_omega[1];
  H[5] = rtb_hat_omega[0];
  H[8] = 0.0;
  H[11] = 0.0;
  H[14] = 0.0;
  H[17] = 0.0;
  for (k = 0; k < 3; k++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      F_tmp = 3 * a_tmp + k;
      K[a_tmp + 6 * k] = H[F_tmp];
      rtb_q_n2m_idx_0 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        rtb_q_n2m_idx_0 += H[3 * H_tmp + k] * P_0[6 * a_tmp + H_tmp];
      }

      H_0[F_tmp] = rtb_q_n2m_idx_0;
    }
  }

  for (k = 0; k < 3; k++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += P_0[6 * H_tmp + a_tmp] * K[6 * k + H_tmp];
      }

      P_1[a_tmp + 6 * k] = P_2;
    }

    for (a_tmp = 0; a_tmp < 3; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += H_0[3 * H_tmp + k] * K[6 * a_tmp + H_tmp];
      }

      H_tmp = 3 * a_tmp + k;
      H_1[H_tmp] = (real_T)K_tmp[H_tmp] * 0.0625 + P_2;
    }
  }

  mrdiv(P_1, H_1, K);
  for (k = 0; k < 3; k++) {
    P_2 = 0.0;
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 += H[3 * a_tmp + k] * delta_x[a_tmp];
    }

    tmp_1[k] = (rtU.mes_mag[k] - rtb_hat_omega[k]) - P_2;
  }

  P_2 = tmp_1[1];
  rtb_q_n2m_idx_0 = tmp_1[0];
  rtb_q_n2m_idx_1 = tmp_1[2];
  for (k = 0; k < 6; k++) {
    delta_x[k] += (K[k + 6] * P_2 + K[k] * rtb_q_n2m_idx_0) + K[k + 12] *
      rtb_q_n2m_idx_1;
  }

  memset(&F[0], 0, 36U * sizeof(real_T));
  for (k = 0; k < 6; k++) {
    F[k + 6 * k] = 1.0;
  }

  for (k = 0; k < 6; k++) {
    rtb_q_n2m_idx_0 = K[k + 6];
    rtb_q_n2m_idx_1 = K[k];
    rtb_q_n2m_idx_2 = K[k + 12];
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      F_tmp = 6 * a_tmp + k;
      F_0[F_tmp] = F[F_tmp] - ((H[3 * a_tmp + 1] * rtb_q_n2m_idx_0 + H[3 * a_tmp]
        * rtb_q_n2m_idx_1) + H[3 * a_tmp + 2] * rtb_q_n2m_idx_2);
    }

    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      F_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        F_2 += F_0[6 * H_tmp + k] * P_0[6 * a_tmp + H_tmp];
      }

      F_1[k + 6 * a_tmp] = F_2;
    }
  }

  memcpy(&P_0[0], &F_1[0], 36U * sizeof(real_T));
  tmp[0] = 0.0;
  tmp[1] = delta_x[0];
  tmp[2] = delta_x[1];
  tmp[3] = delta_x[2];
  quatmultiply(rtDW.q_n2m, tmp, tmp_0);
  q_n2m[0] = 0.5 * tmp_0[0] + rtDW.q_n2m[0];
  q_n2m[1] = 0.5 * tmp_0[1] + rtDW.q_n2m[1];
  q_n2m[2] = 0.5 * tmp_0[2] + rtDW.q_n2m[2];
  q_n2m[3] = 0.5 * tmp_0[3] + rtDW.q_n2m[3];
  P_2 = norm(q_n2m);
  q_n2m[0] /= P_2;
  q_n2m[1] /= P_2;
  q_n2m[2] /= P_2;
  q_n2m[3] /= P_2;
  rtb_hat_omega[0] = rtU.omega[0] - rtDW.beta[0];
  rtb_hat_omega[1] = rtU.omega[1] - rtDW.beta[1];
  rtb_hat_omega[2] = rtU.omega[2] - rtDW.beta[2];
  tmp[0] = 0.0;
  tmp[1] = rtb_hat_omega[0];
  tmp[2] = rtb_hat_omega[1];
  tmp[3] = rtb_hat_omega[2];
  quatmultiply(q_n2m, tmp, tmp_0);
  q_n2m[0] += 0.5 * tmp_0[0] * 0.1;
  q_n2m[1] += 0.5 * tmp_0[1] * 0.1;
  q_n2m[2] += 0.5 * tmp_0[2] * 0.1;
  q_n2m[3] += 0.5 * tmp_0[3] * 0.1;
  P_2 = norm(q_n2m);
  rtb_q_n2m_idx_0 = q_n2m[0] / P_2;
  rtb_q_n2m_idx_1 = q_n2m[1] / P_2;
  rtb_q_n2m_idx_2 = q_n2m[2] / P_2;
  rtb_q_n2m_idx_3 = q_n2m[3] / P_2;
  for (k = 0; k < 9; k++) {
    K_tmp[k] = b[k];
  }

  F[0] = -0.0;
  F[6] = rtb_hat_omega[2];
  F[12] = -rtb_hat_omega[1];
  F[1] = -rtb_hat_omega[2];
  F[7] = -0.0;
  F[13] = rtb_hat_omega[0];
  F[2] = rtb_hat_omega[1];
  F[8] = -rtb_hat_omega[0];
  F[14] = -0.0;
  for (k = 0; k < 3; k++) {
    F_tmp = (k + 3) * 6;
    F[F_tmp] = K_tmp[3 * k];
    F[F_tmp + 1] = K_tmp[3 * k + 1];
    F[F_tmp + 2] = K_tmp[3 * k + 2];
  }

  for (k = 0; k < 6; k++) {
    F[6 * k + 3] = 0.0;
    F[6 * k + 4] = 0.0;
    F[6 * k + 5] = 0.0;
  }

  for (k = 0; k < 36; k++) {
    P_o_tmp[k] = e_a[k];
  }

  for (k = 0; k < 6; k++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      F_2 = 0.0;
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        F_tmp = 6 * H_tmp + k;
        F_2 += P_0[6 * a_tmp + H_tmp] * F[F_tmp];
        P_2 += F[6 * H_tmp + a_tmp] * P_0[F_tmp];
      }

      H_tmp = 6 * a_tmp + k;
      F_1[H_tmp] = P_2;
      F_0[H_tmp] = F_2;
    }
  }

  for (k = 0; k < 3; k++) {
    P_2 = (real_T)K_tmp[3 * k] * 0.0625;
    a[6 * k] = P_2;
    a_tmp = (k + 3) * 6;
    a[a_tmp] = 0.0;
    a[6 * k + 3] = 0.0;
    a[a_tmp + 3] = P_2;
    P_2 = (real_T)K_tmp[3 * k + 1] * 0.0625;
    a[6 * k + 1] = P_2;
    a[a_tmp + 1] = 0.0;
    a[6 * k + 4] = 0.0;
    a[a_tmp + 4] = P_2;
    P_2 = (real_T)K_tmp[3 * k + 2] * 0.0625;
    a[6 * k + 2] = P_2;
    a[a_tmp + 2] = 0.0;
    a[6 * k + 5] = 0.0;
    a[a_tmp + 5] = P_2;
  }

  for (k = 0; k < 6; k++) {
    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += (real_T)P_o_tmp[6 * H_tmp + k] * a[6 * a_tmp + H_tmp];
      }

      P_o_tmp_0[k + 6 * a_tmp] = P_2;
    }

    for (a_tmp = 0; a_tmp < 6; a_tmp++) {
      P_2 = 0.0;
      for (H_tmp = 0; H_tmp < 6; H_tmp++) {
        P_2 += P_o_tmp_0[6 * H_tmp + k] * (real_T)P_o_tmp[6 * a_tmp + H_tmp];
      }

      F_tmp = 6 * a_tmp + k;
      F[F_tmp] = (F_0[F_tmp] + F_1[F_tmp]) + P_2;
    }
  }

  for (k = 0; k < 36; k++) {
    rtDW.P_o[k] = F[k] * 0.1 + P_0[k];
  }

  /* End of MATLAB Function: '<S1>/MEKF' */

  /* DataStoreWrite: '<S1>/Data Store Write1' */
  rtDW.q_n2m[0] = rtb_q_n2m_idx_0;
  rtDW.q_n2m[1] = rtb_q_n2m_idx_1;
  rtDW.q_n2m[2] = rtb_q_n2m_idx_2;
  rtDW.q_n2m[3] = rtb_q_n2m_idx_3;

  /* Outport: '<Root>/meas_ang_vel_body' incorporates:
   *  Math: '<S1>/Transpose2'
   */
  rtY.meas_ang_vel_body[0] = rtb_hat_omega[0];
  rtY.meas_ang_vel_body[1] = rtb_hat_omega[1];
  rtY.meas_ang_vel_body[2] = rtb_hat_omega[2];

  /* Outport: '<Root>/meas_quat_body' */
  rtY.meas_quat_body[0] = rtb_q_n2m_idx_0;
  rtY.meas_quat_body[1] = rtb_q_n2m_idx_1;
  rtY.meas_quat_body[2] = rtb_q_n2m_idx_2;
  rtY.meas_quat_body[3] = rtb_q_n2m_idx_3;
}

/* Model initialize function */
void attitude_determination_and_vehi_initialize(void)
{
  /* Start for DataStoreMemory: '<S1>/Data Store Memory' */
  memcpy(&rtDW.P_o[0], &rtConstP.DataStoreMemory_InitialValue[0], 36U * sizeof
         (real_T));

  /* Start for DataStoreMemory: '<S1>/Data Store Memory1' */
  rtDW.q_n2m[0] = 1.0;
  rtDW.q_n2m[1] = 0.0;
  rtDW.q_n2m[2] = 0.0;
  rtDW.q_n2m[3] = 0.0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
