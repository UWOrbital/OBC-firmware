#pragma once

#include "obc_errors.h"

#include <sys_common.h>

typedef enum {
  AT_MPPT,
  LEFT_MPPT,
  RIGHT_MPPT,
} mppt_state_t;

typedef struct panel_info {
  mppt_state_t state;
  float voltage;
  float current;
  float power;
  float dutyRatio;
  float dV;
  float dI;
  float dP;
  float newMPPVoltage;
} panel_info_t;
