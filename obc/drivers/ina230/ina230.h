#pragma once

typedef enum {
  POWER_DOWN,
  SHUNT_VOLT_TRIG,
  BUS_VOLT_TRIG,
  SHUNT_AND_BUS_TRIG,
  SHUNT_VOLT_CTS,
  BUS_VOLT_CTS,
  SHUNT_AND_BUS_CTS
} ina230_opmode_t;

typedef enum { SDA, SCL, GND, VS } ina230_pin_t;

/**
 * @struct Configuration struct for INA230 sensor
 *
 * @param sampleSize Number of samples collected and averaged for calculations (1, 4, 16, 64, 128, 256, 512, or 1024)
 * @param busVoltCT Conversion time for bus voltage measurement
 * @param shuntVoltCT Conversion time for shunt voltage measurement
 * @param opMode Operating mode for sensor
 * @param a0 What the address pin A0 is connected to
 * @param a1 What the address pin A1 is connected to
 */
typedef struct {
  uint16_t sampleSize;
  float busVoltCT;
  float shuntVoltCT;
  ina230_opmode_t opMode;
  ina230_pin_t a0;
  ina230_pin_t a1;
} ina230_config_t;
