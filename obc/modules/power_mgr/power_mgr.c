#include "power_mgr.h"
#include "obc_print.h"
#include "obc_scheduler_config.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>
#include <math.h>

#define MPPT_PERIOD (pdMS_TO_TICKS(500))  // TODO: Figure out proper period interval

#define SOLAR_PANEL_COUNT 3

#define BATTERY_SENSE_SCALING 0.382022f  // From voltage divider resulting in Vbat (6.8KR/17.8KR)

#define MPPT_STEP_SCALING_FACTOR 1.0f  // TODO: Figure out proper scaling factor

static panel_info_t panels[SOLAR_PANEL_COUNT];

static float batteryVoltage;
static float measuredSolarPannelVolts[SOLAR_PANEL_COUNT];
static float measuredSolarPannelCurrents[SOLAR_PANEL_COUNT];

obc_error_code_t mppt_step(void);
obc_error_code_t update_panel_info(panel_info_t *panel, float v_in, float i_in);
obc_error_code_t update_battery_voltage(void);
obc_error_code_t set_panel_mppt_voltage(void);

mppt_state_t getMPPTState(panel_info_t *panel);

void obcTaskInitPowerMgr(void) {}

void obcTaskFunctionPowerMgr(void *pvParameters) {
  while (1) {
    mppt_step();
    vTaskDelay(MPPT_PERIOD);
  }
}

obc_error_code_t mppt_step() {
  // Improved Incremental Conductance MPPT from https://doi.org/10.1049/iet-rpg.2015.0203
  // TODO: Proper error handling
  update_battery_voltage();
  for (int i = 0; i < SOLAR_PANEL_COUNT; i++) {
    update_panel_info(&panels[i], measuredSolarPannelVolts[i], measuredSolarPannelCurrents[i]);
  }

  set_panel_mppt_voltage();

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t update_battery_voltage() {
  // TODO: Read battery sense load voltage and scale it by BATTERY_SENSE_SCALING
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t update_panel_info(panel_info_t *panel, float v_in, float i_in) {
  // Update voltage and current info
  panel->dV = v_in - panel->voltage;
  panel->dI = i_in - panel->current;
  panel->dP = v_in * i_in - panel->power;
  panel->voltage = i_in;
  panel->current = i_in;
  panel->power = v_in * i_in;
  // Update mppt state
  panel->state = getMPPTState(panel);

  // Calculate new pannel voltage
  float dDutyRatio = MPPT_STEP_SCALING_FACTOR * fabsf(panel->dP);

  // Calculate new duty ratio
  if (panel->state == LEFT_MPPT) {
    panel->dutyRatio += dDutyRatio;
  } else if (panel->state == RIGHT_MPPT) {
    panel->dutyRatio -= dDutyRatio;
  }

  panel->newMPPVoltage = batteryVoltage * (1.0f - panel->dutyRatio);

  return OBC_ERR_CODE_SUCCESS;
}

mppt_state_t getMPPTState(panel_info_t *panel) {
  mppt_state_t state;
  if (panel->voltage * panel->dI + panel->current * panel->dV == 0) {
    state = AT_MPPT;
  } else {
    if (panel->voltage * panel->dI + panel->current * panel->dV > 0) {
      if (panel->dV > 0) {
        state = LEFT_MPPT;
      } else {
        state = RIGHT_MPPT;
      }
    } else {
      if (panel->dV > 0) {
        state = RIGHT_MPPT;
      } else {
        state = LEFT_MPPT;
      }
    }
  }

  return state;
}

obc_error_code_t set_panel_mppt_voltage(void) {
  // TODO: Set potentiometer to voltage designated by panel->newMPPVoltage for all 3 panels
  return OBC_ERR_CODE_SUCCESS;
}
