#include "obc_digital_watchdog.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_task.h>

#include "gnc_manager.h"
#include "attitude_control.h"
#include "attitude_determination_and_vehi.h"
#include "onboard_env_modelling.h"

#include <sys_common.h>
#include <gio.h>

#define GNC_TASK_PERIOD_MS 50 /* 50ms period or 20Hz */

void rtOnboardModelStep(void);
void rtAttitudeDeterminationModelStep(void);
void rtAttitudeControlModelStep(void);

void rtOnboardModelStep(void) {
  static boolean_T OverrunFlag = false;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(onboard_env_model_rt_object, "Overrun");
    return;
  }

  OverrunFlag = true;

  /* Save FPU context here (if necessary) */

  /* Re-enable timer or interrupt here */
  /* Idea: Setup the semaphore timeout here*/

  /* Set model inputs here | Currently setting mock values for inputs */
  onboard_env_model_ext_intputs.commanded_mag_dipole_body[0] = 1.0;
  onboard_env_model_ext_intputs.commanded_mag_dipole_body[1] = 1.0;
  onboard_env_model_ext_intputs.commanded_mag_dipole_body[2] = 1.0;

  onboard_env_model_ext_intputs.r_sat_com[0] = 1.0;
  onboard_env_model_ext_intputs.r_sat_com[1] = 1.0;
  onboard_env_model_ext_intputs.r_sat_com[2] = 1.0;

  onboard_env_model_ext_intputs.r_sat_com_ax1[0] = 1.0;
  onboard_env_model_ext_intputs.r_sat_com_ax1[1] = 1.0;
  onboard_env_model_ext_intputs.r_sat_com_ax1[2] = 1.0;

  onboard_env_model_ext_intputs.steve_values[0] = 1.0;
  onboard_env_model_ext_intputs.steve_values[1] = 1.0;
  onboard_env_model_ext_intputs.steve_values[2] = 1.0;

  /* Step the model */
  onboad_env_modelling_step();

  /* Get model outputs here */
  real_T angularBodyX = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[0];
  real_T angularBodyY = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[1];
  real_T angularBodyZ = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[2];

  real_T commandEstimateX = onboard_env_model_ext_outputs.r_ref_com_est[0];
  real_T commandEstimateY = onboard_env_model_ext_outputs.r_ref_com_est[1];
  real_T commandEstimateZ = onboard_env_model_ext_outputs.r_ref_com_est[2];

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here | Reset semaphore timeoeut */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

void rtAttitudeDeterminationModelStep(void) {
  static boolean_T OverrunFlag = false;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(attitude_determinataion_model_rt_object, "Overrun");
    return;
  }

  OverrunFlag = true;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */

  attitude_determination_model_ext_inputs.earth_mag_field_ref[0] = 1.0;
  attitude_determination_model_ext_inputs.earth_mag_field_ref[1] = 1.0;
  attitude_determination_model_ext_inputs.earth_mag_field_ref[2] = 1.0;

  attitude_determination_model_ext_inputs.mes_aam[0] = 1.0;
  attitude_determination_model_ext_inputs.mes_aam[1] = 1.0;
  attitude_determination_model_ext_inputs.mes_aam[2] = 1.0;

  attitude_determination_model_ext_inputs.mes_mag[0] = 1.0;
  attitude_determination_model_ext_inputs.mes_mag[1] = 1.0;
  attitude_determination_model_ext_inputs.mes_mag[2] = 1.0;

  attitude_determination_model_ext_inputs.mes_ss[0] = 1.0;
  attitude_determination_model_ext_inputs.mes_ss[1] = 1.0;
  attitude_determination_model_ext_inputs.mes_ss[2] = 1.0;

  attitude_determination_model_ext_inputs.omega[0] = 1.0;
  attitude_determination_model_ext_inputs.omega[1] = 1.0;
  attitude_determination_model_ext_inputs.omega[2] = 1.0;

  attitude_determination_model_ext_inputs.r_sat_com_ax1[0] = 1.0;
  attitude_determination_model_ext_inputs.r_sat_com_ax1[1] = 1.0;
  attitude_determination_model_ext_inputs.r_sat_com_ax1[2] = 1.0;

  attitude_determination_model_ext_inputs.ref_aam[0] = 1.0;
  attitude_determination_model_ext_inputs.ref_aam[1] = 1.0;
  attitude_determination_model_ext_inputs.ref_aam[2] = 1.0;

  attitude_determination_model_ext_inputs.sat_to_sun_unit_ref[0] = 1.0;
  attitude_determination_model_ext_inputs.sat_to_sun_unit_ref[1] = 1.0;
  attitude_determination_model_ext_inputs.sat_to_sun_unit_ref[2] = 1.0;

  attitude_determination_model_ext_inputs.steve_mes[0] = 1.0;
  attitude_determination_model_ext_inputs.steve_mes[1] = 1.0;
  attitude_determination_model_ext_inputs.steve_mes[2] = 1.0;

  /* Step the model */
  attitude_determination_and_vehi_step();

  /* Get model outputs here */
  real_T angularVelocityX = attitude_determination_model_ext_outputs.meas_ang_vel_body[0];
  real_T angularVelocityY = attitude_determination_model_ext_outputs.meas_ang_vel_body[1];
  real_T angularVelocityZ = attitude_determination_model_ext_outputs.meas_ang_vel_body[2];

  real_T quaterionX = attitude_determination_model_ext_outputs.meas_quat_body[0];
  real_T quaterionY = attitude_determination_model_ext_outputs.meas_quat_body[1];
  real_T quaterionZ = attitude_determination_model_ext_outputs.meas_quat_body[2];
  real_T quaterionW = attitude_determination_model_ext_outputs.meas_quat_body[3];

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

void rtAttitudeControlModelStep(void) {
  static boolean_T OverrunFlag = false;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(attitude_control_model_rt_object, "Overrun");
    return;
  }

  OverrunFlag = true;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */

  attitude_control_model_ext_inputs.com_quat_body[0] = 1.0;
  attitude_control_model_ext_inputs.com_quat_body[1] = 1.0;
  attitude_control_model_ext_inputs.com_quat_body[2] = 1.0;

  attitude_control_model_ext_inputs.est_curr_ang_vel_body[0] = 1.0;
  attitude_control_model_ext_inputs.est_curr_ang_vel_body[1] = 1.0;
  attitude_control_model_ext_inputs.est_curr_ang_vel_body[2] = 1.0;

  attitude_control_model_ext_inputs.est_curr_quat_body[0] = 1.0;
  attitude_control_model_ext_inputs.est_curr_quat_body[1] = 1.0;
  attitude_control_model_ext_inputs.est_curr_quat_body[2] = 1.0;

  attitude_control_model_ext_inputs.mag_field_body[0] = 1.0;
  attitude_control_model_ext_inputs.mag_field_body[1] = 1.0;
  attitude_control_model_ext_inputs.mag_field_body[2] = 1.0;

  /* Step the model */
  attitude_control_step();

  /* Get model outputs here */
  real_T dipoleX = attitude_control_model_ext_outputs.comm_mag_dipole_body[0];
  real_T dipoleY = attitude_control_model_ext_outputs.comm_mag_dipole_body[1];
  real_T dipoleZ = attitude_control_model_ext_outputs.comm_mag_dipole_body[2];

  real_T wheelTorqueX = attitude_control_model_ext_outputs.comm_wheel_torque_body[0];
  real_T wheelTorqueY = attitude_control_model_ext_outputs.comm_wheel_torque_body[1];
  real_T wheelTorqueZ = attitude_control_model_ext_outputs.comm_wheel_torque_body[2];

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

void obcTaskInitGncMgr(void) {
  /* Initialize the onboard modelling environment */
  onboad_env_modelling_initialize();

  /* Initialize the attitude determination algorithms */
  attitude_determination_and_vehi_initialize();

  /* Initialize the attitude control algorithms */
  attitude_control_initialize();
}

void obcTaskFunctionGncMgr(void *pvParameters) {
  TickType_t xLastWakeTime;

  /* Initialize the last wake time to the current time */
  xLastWakeTime = xTaskGetTickCount();

  /* Run GNC tasks periodically at 20 Hz */
  while (1) {
    feedDigitalWatchdog();

    /* Place GNC Tasks here */

    /* Refresh GNC outputs */
    rtOnboardModelStep();

    rtAttitudeDeterminationModelStep();

    rtAttitudeControlModelStep();

    /* This will automatically update the xLastWakeTime variable to be the last unblocked time, set to delay for 50ms */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(GNC_TASK_PERIOD_MS));
  }
}
