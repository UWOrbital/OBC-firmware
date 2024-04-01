#include "obc_digital_watchdog.h"
#include "digital_watchdog_mgr.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"
#include "obc_print.h"

#include "gnc_manager.h"
#include "attitude_control.h"
#include "attitude_determination_and_vehi.h"
#include "onboard_env_modelling.h"
#include "vn100.h"
#include "bd621x.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_task.h>
#include <os_semphr.h>
#include <sys_common.h>
#include <gio.h>
#include <math.h>

#define DEFAULT_GNC_TASK_PERIOD_MS 50 /* 50ms period or 20Hz */
#define MAX_GNC_TASK_PERIOD_MS 100
#define DEGREES_TO_RADIANS(theta) (theta * M_PI / 180)

uint16_t GncTaskPeriod = DEFAULT_GNC_TASK_PERIOD_MS;
uint32_t cycleNum = 1;
uint32_t cyclesToDelay = 1;

vn100_binary_packet_t vn100Packet;

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
  onboard_env_model_ext_intputs.commanded_mag_dipole_body[0] = 5.83;
  onboard_env_model_ext_intputs.commanded_mag_dipole_body[1] = 2.12;
  onboard_env_model_ext_intputs.commanded_mag_dipole_body[2] = -0.4;

  onboard_env_model_ext_intputs.r_sat_com[0] = (6371 + 408) * 1000;
  onboard_env_model_ext_intputs.r_sat_com[1] = 0.0;
  onboard_env_model_ext_intputs.r_sat_com[2] = 0.0;

  onboard_env_model_ext_intputs.r_sat_com_ax1[0] = 1.0;
  onboard_env_model_ext_intputs.r_sat_com_ax1[1] = 1.0;
  onboard_env_model_ext_intputs.r_sat_com_ax1[2] = 1.0;

  onboard_env_model_ext_intputs.steve_values[0] = 0.0;
  onboard_env_model_ext_intputs.steve_values[1] = 0.0;

  /* Step the model */
  onboad_env_modelling_step();

  /* Get model outputs here */
  real_T angularBodyX = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[0];
  real_T angularBodyY = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[1];
  real_T angularBodyZ = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[2];

  real_T referenceEstimateX = onboard_env_model_ext_outputs.r_ref_com_est[0];
  real_T referenceEstimateY = onboard_env_model_ext_outputs.r_ref_com_est[1];
  real_T referenceEstimateZ = onboard_env_model_ext_outputs.r_ref_com_est[2];

  sciPrintf("Onboard modelling outputs are: \r\n");
  sciPrintf("Angular Body: (%lf,%lf,%lf) \r\n", angularBodyX, angularBodyY, angularBodyZ);
  sciPrintf("Reference Estimate: (%lf,%lf,%lf) \r\n", referenceEstimateX, referenceEstimateY, referenceEstimateZ);

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

  attitude_determination_model_ext_inputs.earth_mag_field_ref[0] = -300;
  attitude_determination_model_ext_inputs.earth_mag_field_ref[1] = 500;
  attitude_determination_model_ext_inputs.earth_mag_field_ref[2] = 28000;

  attitude_determination_model_ext_inputs.mes_aam[0] = -0.2;
  attitude_determination_model_ext_inputs.mes_aam[1] = -8.13;
  attitude_determination_model_ext_inputs.mes_aam[2] = 0.05;

  attitude_determination_model_ext_inputs.mes_mag[0] = -356;
  attitude_determination_model_ext_inputs.mes_mag[1] = 487;
  attitude_determination_model_ext_inputs.mes_mag[2] = 26840;

  attitude_determination_model_ext_inputs.mes_ss[0] = 0.94;
  attitude_determination_model_ext_inputs.mes_ss[1] = 0.75;
  attitude_determination_model_ext_inputs.mes_ss[2] = 0.67;

  attitude_determination_model_ext_inputs.omega[0] = 1.0;
  attitude_determination_model_ext_inputs.omega[1] = 1.0;
  attitude_determination_model_ext_inputs.omega[2] = 1.0;

  attitude_determination_model_ext_inputs.r_sat_com_ax1[0] = 1.0;
  attitude_determination_model_ext_inputs.r_sat_com_ax1[1] = 1.0;
  attitude_determination_model_ext_inputs.r_sat_com_ax1[2] = 1.0;

  attitude_determination_model_ext_inputs.ref_aam[0] = -0.01;
  attitude_determination_model_ext_inputs.ref_aam[1] = -8.29;
  attitude_determination_model_ext_inputs.ref_aam[2] = 0.01;

  attitude_determination_model_ext_inputs.sat_to_sun_unit_ref[0] = 1.0;
  attitude_determination_model_ext_inputs.sat_to_sun_unit_ref[1] = 0.0;
  attitude_determination_model_ext_inputs.sat_to_sun_unit_ref[2] = 0.0;

  attitude_determination_model_ext_inputs.steve_mes[0] = 0.4;
  attitude_determination_model_ext_inputs.steve_mes[1] = -0.3;
  attitude_determination_model_ext_inputs.steve_mes[2] = 0.0;

  /* Step the model */
  attitude_determination_and_vehi_step();

  /* Get model outputs here */
  real_T measuredAngularVelocityX = attitude_determination_model_ext_outputs.meas_ang_vel_body[0];
  real_T measuredAngularVelocityY = attitude_determination_model_ext_outputs.meas_ang_vel_body[1];
  real_T measuredAngularVelocityZ = attitude_determination_model_ext_outputs.meas_ang_vel_body[2];

  real_T quaterionX = attitude_determination_model_ext_outputs.meas_quat_body[0];
  real_T quaterionY = attitude_determination_model_ext_outputs.meas_quat_body[1];
  real_T quaterionZ = attitude_determination_model_ext_outputs.meas_quat_body[2];
  real_T quaterionW = attitude_determination_model_ext_outputs.meas_quat_body[3];

  sciPrintf("Attitude determintation outputs are: \r\n");
  sciPrintf("Angular Velocity: (%lf,%lf,%lf) \r\n", measuredAngularVelocityX, measuredAngularVelocityY,
            measuredAngularVelocityZ);
  sciPrintf("Quatereions: (%lf,%lf,%lf,%lf) \r\n", quaterionX, quaterionY, quaterionZ, quaterionW);

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
  attitude_control_model_ext_inputs.com_quat_body[0] = sin(DEGREES_TO_RADIANS(25));
  attitude_control_model_ext_inputs.com_quat_body[1] = cos(DEGREES_TO_RADIANS(25) / sqrt(2));
  attitude_control_model_ext_inputs.com_quat_body[2] = cos(DEGREES_TO_RADIANS(24) / sqrt(2));
  attitude_control_model_ext_inputs.com_quat_body[3] = 0.0;

  attitude_control_model_ext_inputs.est_curr_ang_vel_body[0] = 0.1;
  attitude_control_model_ext_inputs.est_curr_ang_vel_body[1] = -0.05;
  attitude_control_model_ext_inputs.est_curr_ang_vel_body[2] = 0.03;

  attitude_control_model_ext_inputs.est_curr_quat_body[0] = cos(DEGREES_TO_RADIANS(-30));
  attitude_control_model_ext_inputs.est_curr_quat_body[1] = sin(DEGREES_TO_RADIANS(-30) / sqrt(3));
  attitude_control_model_ext_inputs.est_curr_quat_body[2] = sin(DEGREES_TO_RADIANS(-30) / sqrt(3));
  attitude_control_model_ext_inputs.est_curr_quat_body[3] = 0.0;

  attitude_control_model_ext_inputs.mag_field_body[0] = 1.0;
  attitude_control_model_ext_inputs.mag_field_body[1] = 1.0;
  attitude_control_model_ext_inputs.mag_field_body[2] = 1.0;

  /* Step the model */
  attitude_control_step();

  /* Get model outputs here */
  real_T commandedDipoleX = attitude_control_model_ext_outputs.comm_mag_dipole_body[0];
  real_T commandedDipoleY = attitude_control_model_ext_outputs.comm_mag_dipole_body[1];
  real_T commandedDipoleZ = attitude_control_model_ext_outputs.comm_mag_dipole_body[2];

  real_T commandedWheelTorqueX = attitude_control_model_ext_outputs.comm_wheel_torque_body[0];
  real_T commandedWheelTorqueY = attitude_control_model_ext_outputs.comm_wheel_torque_body[1];
  real_T commandedWheelTorqueZ = attitude_control_model_ext_outputs.comm_wheel_torque_body[2];

  /* Use the outputs to control actuators */

  sciPrintf("Attitude control outputs are: \r\n");
  sciPrintf("Wheel Torque: (%lf,%lf,%lf) \r\n", commandedWheelTorqueX, commandedWheelTorqueY, commandedWheelTorqueZ);
  sciPrintf("Quatereions: (%lf,%lf,%lf) \r\n", commandedDipoleX, commandedDipoleY, commandedDipoleZ);

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

obc_error_code_t setGncTaskPeriod(uint16_t periodMs) {
  /* If the period exceeds 50ms, set to block for another interval (e.g 100ms is one blocked cycle for 50ms and then
   * running the full GNC code for the other 50ms)*/
  if (periodMs > DEFAULT_GNC_TASK_PERIOD_MS) {
    cyclesToDelay = periodMs / DEFAULT_GNC_TASK_PERIOD_MS;
    return OBC_ERR_CODE_SUCCESS;
  } else if (periodMs <= 0) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  GncTaskPeriod = periodMs;
  return OBC_ERR_CODE_SUCCESS;
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
    /* Check in with the watchdog */
    digitalWatchdogTaskCheckIn(OBC_SCHEDULER_CONFIG_ID_GNC_MGR);
    if (cycleNum != cyclesToDelay) {
      cycleNum++;
      continue;
    } else {
      /* Reset the cycleNum back to 1 if cycleNum % cyclesToDelay == 0, meaning that it has delayed enough times and
       * cycleNum can increment again. */
      cycleNum = 1;
    }

    /* Place GNC Tasks here */

    TickType_t startTime = xTaskGetTickCount();
    sciPrintf("Starting GNC Task at %d ms \r\n", startTime);

    obc_error_code_t errCode;
    errCode = vn100ReadBinaryOutputs(&vn100Packet);

    if (errCode != OBC_ERR_CODE_SUCCESS) {
      sciPrintf("Error reading from VN-100 - Error code %d \r\n", errCode);
    }

    /* Refresh GNC outputs */
    rtOnboardModelStep();
    TickType_t onboardModelElapsedTime = xTaskGetTickCount() - startTime;

    rtAttitudeDeterminationModelStep();
    TickType_t attitudeDeterminationElapsedTime = xTaskGetTickCount() - (onboardModelElapsedTime + startTime);

    rtAttitudeControlModelStep();
    TickType_t attitudeControlElapsedTime = xTaskGetTickCount() - (attitudeDeterminationElapsedTime + startTime);

    TickType_t totalElapsedTime = xTaskGetTickCount() - startTime;

    sciPrintf("Elapsed Time of onboard model: %d ms \r\n", onboardModelElapsedTime);
    sciPrintf("Elapsed Time of attitude determination model: %d ms \r\n", attitudeDeterminationElapsedTime);
    sciPrintf("Elapsed Time of attitude control model: %d ms \r\n", attitudeControlElapsedTime);
    sciPrintf("Total elapsed Time of all algorithms: %d ms \r\n", totalElapsedTime);

    /* This will automatically update the xLastWakeTime variable to be the last unblocked time, set to delay for 50ms */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(GncTaskPeriod));
  }
}
