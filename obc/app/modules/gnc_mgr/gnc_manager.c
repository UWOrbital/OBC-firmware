#include "obc_digital_watchdog.h"
#include "digital_watchdog_mgr.h"
#include "obc_errors.h"
#include "obc_scheduler_config.h"
#include "obc_print.h"
#include "obc_logging.h"
#include "obc_general_util.h"
#include "gnc_manager.h"
#include "attitude_control.h"
#include "attitude_determination_and_vehi.h"
#include "onboard_env_modelling.h"
#include "vn100.h"
#include "bd621x.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_task.h>
#include <sys_common.h>
#include <gio.h>

#include <math.h>

#define DEFAULT_GNC_TASK_PERIOD_MS 50 /* 50ms period or 20Hz */
#define MAX_GNC_TASK_PERIOD_MS 100
#define DEGREES_TO_RADIANS(theta) (theta * M_PI / 180)

/* MTQ commanding test config (see startGncMtqTest in gnc_manager.h).
 * The duty ramps in MTQ_TEST_DUTY_STEP_PERCENT increments, one step per GNC
 * task cycle, between 0 and MTQ_TEST_MAX_DUTY_PERCENT. The peak is held for
 * MTQ_TEST_HOLD_MS before the polarity flips. */
#define MTQ_TEST_MAX_DUTY_PERCENT 75
#define MTQ_TEST_DUTY_STEP_PERCENT 5
#define MTQ_TEST_HOLD_MS 10000U

/* Same X-axis wiring as the test_app_bd621x bench example:
 * FIN on pwm2/N2HET1[12] (J4-6), RIN on pwm3/N2HET1[14] (J4-5) */
static const mtq_t gncMtqX = {
    .hetRam = hetRAM1, .hetReg = hetREG1, .finPwm = pwm2, .rinPwm = pwm3, .finPin = 12U, .rinPin = 14U};

typedef enum {
  MTQ_TEST_STATE_RAMP_UP,           /* forward polarity, ramping 0 -> +75 % */
  MTQ_TEST_STATE_HOLD,              /* holding +75 % for MTQ_TEST_HOLD_MS */
  MTQ_TEST_STATE_RAMP_DOWN_REVERSE, /* reverse polarity, ramping -75 % -> 0 */
} mtq_test_state_t;

/* Written by the command callbacks (command manager task), read by the GNC
 * task; single bool so the cross-task access is atomic */
static volatile bool mtqTestRequested = false;

/* GNC-task-private test state */
static bool mtqTestRunning = false;
static mtq_test_state_t mtqTestState = MTQ_TEST_STATE_RAMP_UP;
static int32_t mtqTestDuty = 0;
static TickType_t mtqTestHoldStartTicks = 0;

uint32_t cycleNum = 1;
uint32_t taskRateDivisor = 1;

vn100_binary_packet_t vn100LastValidPacket;

static void rtOnboardModelStep(void);
static void rtAttitudeDeterminationModelStep(void);
static void rtAttitudeControlModelStep(void);
static bool stepMtqTest(void);

static void rtOnboardModelStep(void) {
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
  onboard_env_modelling_step();

  /* Get model outputs here */
  real_T angularBodyX = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[0];
  real_T angularBodyY = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[1];
  real_T angularBodyZ = onboard_env_model_ext_outputs.estimated_expect_ang_acc_body[2];

  real_T referenceEstimateX = onboard_env_model_ext_outputs.r_ref_com_est[0];
  real_T referenceEstimateY = onboard_env_model_ext_outputs.r_ref_com_est[1];
  real_T referenceEstimateZ = onboard_env_model_ext_outputs.r_ref_com_est[2];

  UNUSED(angularBodyX);
  UNUSED(angularBodyY);
  UNUSED(angularBodyZ);

  UNUSED(referenceEstimateX);
  UNUSED(referenceEstimateY);
  UNUSED(referenceEstimateZ);
}

static void rtAttitudeDeterminationModelStep(void) {
  /* Set model inputs here - Arbitrary for now */

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

  UNUSED(measuredAngularVelocityX);
  UNUSED(measuredAngularVelocityY);
  UNUSED(measuredAngularVelocityZ);

  UNUSED(quaterionX);
  UNUSED(quaterionY);
  UNUSED(quaterionZ);
  UNUSED(quaterionW);
}

static void rtAttitudeControlModelStep(void) {
  /* Set model inputs here - Arbitrary for now */
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

  UNUSED(commandedDipoleX);
  UNUSED(commandedDipoleY);
  UNUSED(commandedDipoleZ);

  UNUSED(commandedWheelTorqueX);
  UNUSED(commandedWheelTorqueY);
  UNUSED(commandedWheelTorqueZ);
}

obc_error_code_t startGncMtqTest(void) {
  mtqTestRequested = true;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopGncMtqTest(void) {
  mtqTestRequested = false;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Run one step of the MTQ commanding test. Called once per GNC task
 * cycle; all mtqSetOutput calls happen here so the driver is only ever touched
 * from the GNC task.
 *
 * @return true if the test currently owns the actuators (the normal GNC
 * pipeline should be skipped this cycle), false otherwise
 */
static bool stepMtqTest(void) {
  obc_error_code_t errCode;

  if (!mtqTestRequested) {
    if (mtqTestRunning) {
      /* Stop command received: leave the MTQ off (standby, both inputs low) */
      LOG_IF_ERROR_CODE(mtqStop(&gncMtqX));
      mtqTestRunning = false;
    }
    return false;
  }

  if (!mtqTestRunning) {
    /* Start command received: begin a fresh cycle from 0 %, forward polarity */
    mtqTestRunning = true;
    mtqTestState = MTQ_TEST_STATE_RAMP_UP;
    mtqTestDuty = 0;
  }

  switch (mtqTestState) {
    case MTQ_TEST_STATE_RAMP_UP:
      mtqTestDuty += MTQ_TEST_DUTY_STEP_PERCENT;
      if (mtqTestDuty >= MTQ_TEST_MAX_DUTY_PERCENT) {
        mtqTestDuty = MTQ_TEST_MAX_DUTY_PERCENT;
        mtqTestState = MTQ_TEST_STATE_HOLD;
        mtqTestHoldStartTicks = xTaskGetTickCount();
      }
      LOG_IF_ERROR_CODE(mtqSetOutput(&gncMtqX, mtqTestDuty));
      break;

    case MTQ_TEST_STATE_HOLD:
      /* Output is already at +75 %; once the hold expires, flip instantly to
       * reverse polarity at 75 % and start ramping down */
      if ((xTaskGetTickCount() - mtqTestHoldStartTicks) >= pdMS_TO_TICKS(MTQ_TEST_HOLD_MS)) {
        mtqTestState = MTQ_TEST_STATE_RAMP_DOWN_REVERSE;
        mtqTestDuty = MTQ_TEST_MAX_DUTY_PERCENT;
        LOG_IF_ERROR_CODE(mtqSetOutput(&gncMtqX, -mtqTestDuty));
      }
      break;

    case MTQ_TEST_STATE_RAMP_DOWN_REVERSE:
      mtqTestDuty -= MTQ_TEST_DUTY_STEP_PERCENT;
      if (mtqTestDuty <= 0) {
        /* Reached 0: switch instantly back to forward polarity; the next cycle
         * re-enters RAMP_UP and the whole pattern repeats indefinitely */
        mtqTestDuty = 0;
        mtqTestState = MTQ_TEST_STATE_RAMP_UP;
      }
      LOG_IF_ERROR_CODE(mtqSetOutput(&gncMtqX, -mtqTestDuty));
      break;
  }

  return true;
}

obc_error_code_t setGncTaskPeriod(uint16_t periodMs) {
  /* If the period exceeds 50ms, set to block for another interval (e.g 100ms is one blocked cycle for 50ms and then
   * running the full GNC code for the other 50ms)*/
  if ((periodMs > MAX_GNC_TASK_PERIOD_MS) || (periodMs < DEFAULT_GNC_TASK_PERIOD_MS)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  taskRateDivisor = periodMs / DEFAULT_GNC_TASK_PERIOD_MS;
  return OBC_ERR_CODE_SUCCESS;
}

void obcTaskInitGncMgr(void) {
  obc_error_code_t errCode;

  /* Initialize the onboard modelling environment */
  onboard_env_modelling_initialize();

  /* Initialize the attitude determination algorithms */
  attitude_determination_and_vehi_initialize();

  /* Initialize the attitude control algorithms */
  attitude_control_initialize();

  /* Initialize the magnetorquer channel used by the MTQ commanding test
   * (starts with both bridge inputs low / 0 % output) */
  LOG_IF_ERROR_CODE(mtqInit(&gncMtqX));
}

void obcTaskFunctionGncMgr(void *pvParameters) {
  TickType_t xLastWakeTime;

  /* Initialize the last wake time to the current time */
  xLastWakeTime = xTaskGetTickCount();

  /* Run GNC tasks periodically at 20 Hz */
  while (1) {
    /* Check in with the watchdog */
    digitalWatchdogTaskCheckIn(OBC_SCHEDULER_CONFIG_ID_GNC_MGR);
    if (cycleNum <= taskRateDivisor) {
      cycleNum++;
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(DEFAULT_GNC_TASK_PERIOD_MS));
      continue;
    } else {
      /* Reset the cycleNum back to 1 if cycleNum % taskRateDivisor == 0, meaning that it has delayed enough times and
       * cycleNum can increment again. */
      cycleNum = 1;
    }

    /* Place GNC Tasks here */
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

    /* MTQ commanding test: while it is running it owns the actuators, so the
     * normal GNC pipeline (sensor reads + model steps) is skipped until the
     * stop command is received */
    if (stepMtqTest()) {
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(DEFAULT_GNC_TASK_PERIOD_MS));
      continue;
    }

    /* Read from sensors */
    vn100_binary_packet_t vn100CurrentPacket = {0};

#ifdef CONFIG_VN100
    LOG_IF_ERROR_CODE(vn100ReadBinaryOutputs(&vn100CurrentPacket));
#endif

    if (errCode == OBC_ERR_CODE_SUCCESS) {
      /* TODO: Double check with GNC what to do if any sensor read fails
         i.e should we not adjust our actuators at all or run the step with previous values.
         I have a feeling that the best thing would be to use the last valid value though */
      memcpy(&vn100LastValidPacket, &vn100CurrentPacket, sizeof(vn100CurrentPacket));
    }

    /* Refresh GNC outputs */
    rtOnboardModelStep();

    rtAttitudeDeterminationModelStep();

    rtAttitudeControlModelStep();

    /* This will automatically update the xLastWakeTime variable to be the last unblocked time, set to delay for 50ms */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(DEFAULT_GNC_TASK_PERIOD_MS));
  }
}
