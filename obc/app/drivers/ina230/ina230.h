#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include "obc_errors.h"
#include <stdint.h>

#ifdef USE_MOCK_I2C
#ifndef TICK_TYPE_H
typedef uint32_t TickType_t;
#endif
#else
#include "os_portmacro.h"
#endif

#define INA230_I2C_ADDRESS_ONE 0b1000000U
#define INA230_I2C_ADDRESS_TWO 0b1000001U

// ------------------  INA230 IC Related General Configuration Addresses/Bitfields ------------- //
#define INA230_CONFIG_REGISTER_ADDR 0x00U
#define INA230_MASK_ENABLE_REGISTER_ADDR 0x06U
#define INA230_ALERT_LIMIT_REGISTER_ADDR 0x07U
#define INA230_CALIBRATION_REGISTER_ADDR 0x05U
#define INA230_SHUNT_VOLTAGE_REGISTER_ADDR 0x01U
#define INA230_BUS_VOLTAGE_REGISTER_ADDR 0x02U
#define INA230_POWER_REGISTER_ADDR 0x03U
#define INA230_CURRENT_REGISTER_ADDR 0x04U

#define INA230_CONFIG_MODE_SHIFT 0U
#define INA230_CONFIG_SHU_SHIFT 3U
#define INA230_CONFIG_AVG_SHIFT 9U
#define INA230_CONFIG_BUS_SHIFT 6U

// ------------------  INA230 IC Configuration Masks/Flags ------------- //
#define INA230_MASK_ENABLE_SHUNT_OVER_ALERT_MASK (0b1 << 15)
#define INA230_MASK_ENABLE_SHUNT_UNDER_ALERT_MASK (0b1 << 14)
#define INA230_MASK_ENABLE_BUS_OVER_ALERT_MASK (0b1 << 13)
#define INA230_MASK_ENABLE_BUS_UNDER_ALERT_MASK (0b1 << 12)
#define INA230_MASK_ENABLE_POWER_OVER_ALERT_MASK (0b1 << 11)
#define INA230_MASK_ENABLE_TRANSPARENT_MODE_SET_MASK 1U

// ------------------  INA230 IC Configuration Modes ------------- //

// Operating Modes
#define INA230_MODE_POWER_DOWN 0b000
#define INA230_MODE_SHUNT_TRIGGERED 0b001
#define INA230_MODE_BUS_TRIGGERED 0b010
#define INA230_MODE_SHUNT_BUS_TRIGGERED 0b011
#define INA230_MODE_SHUNT_CONTINUOUS 0b101
#define INA230_MODE_BUS_CONTINUOUS 0b110
#define INA230_MODE_SHUNT_BUS_CONTINUOUS 0b111

// Conversion Time (time for sensor to take measurements)
#define INA230_CONV_TIME_140US 0b000
#define INA230_CONV_TIME_204US 0b001
#define INA230_CONV_TIME_332US 0b010
#define INA230_CONV_TIME_588US 0b011
#define INA230_CONV_TIME_1100US 0b100  // 1.1ms: good balance
#define INA230_CONV_TIME_2116US 0b101
#define INA230_CONV_TIME_4156US 0b110
#define INA230_CONV_TIME_8244US 0b111  // Most accurate

// Averaging Modes (number of measurements taken and averaged)
#define INA230_AVG_1 0b000  // No averaging
#define INA230_AVG_4 0b001
#define INA230_AVG_16 0b010  // Good default
#define INA230_AVG_64 0b011
#define INA230_AVG_128 0b100
#define INA230_AVG_256 0b101
#define INA230_AVG_512 0b110
#define INA230_AVG_1024 0b111  // Maximum smoothing

// Macros for Defaults
#define INA230_DEFAULT_MODE INA230_MODE_SHUNT_BUS_CONTINUOUS
#define INA230_DEFAULT_SHUNT_CONV_TIME INA230_CONV_TIME_1100US
#define INA230_DEFAULT_BUS_CONV_TIME INA230_CONV_TIME_1100US
#define INA230_DEFAULT_AVERAGING INA230_AVG_16

// Mask and Alerts
#define INA230_MASK_ENABLE_NONE 0x0000
#define INA230_ALERT_LIMIT_NONE 0x0000

// Macros for LSB, Shunt Resistor, and Calibration Value
#define INA230_SHUNT_VOLTAGE_LSB 0.0000025f
#define INA230_BUS_VOLTAGE_LSB 0.00125f
#define INA230_CURRENT_LSB 0.001f   // 1 mA, current least significant bit
#define INA230_SHUNT_RESISTOR 0.1f  // 0.1 ohms, shunt resistor value
#define INA230_CALIBRATION_VALUE (uint16_t)(0.00512 / (INA230_CURRENT_LSB * INA230_SHUNT_RESISTOR))
#define INA230_POWER_LSB_MULTIPLIER 25

typedef enum {
  INA230_DEVICE_ONE = 0x00,
  INA230_DEVICE_TWO = 0x01,
  INA230_DEVICE_THREE = 0x02,
  INA230_DEVICE_FOUR = 0x03,
  INA230_DEVICE_FIVE = 0x04,
  INA230_DEVICE_SIX = 0x05,
  INA230_DEVICE_SEVEN = 0x06,
  INA230_DEVICE_EIGHT = 0x07,
  INA230_DEVICE_NINE = 0x08,
  INA230_DEVICE_TEN = 0x09,
  INA230_DEVICE_ELEVEN = 0x0A,
  INA230_DEVICE_COUNT = 0x0B
} ina230_device_t;

// function pointers to switch between mock and real data
extern obc_error_code_t (*i2cReadRegFuncPtr)(uint8_t, uint8_t, uint8_t*, uint16_t, TickType_t);
extern obc_error_code_t (*i2cWriteRegFuncPtr)(uint8_t, uint8_t, uint8_t*, uint16_t);

obc_error_code_t initINA230();
obc_error_code_t readAndDisableIfAlert(ina230_device_t device);
obc_error_code_t getINA230ShuntVoltage(ina230_device_t device, float* shuntVoltage);
obc_error_code_t disableNoAlert(ina230_device_t device);
obc_error_code_t getINA230BusVoltage(ina230_device_t device, float* busVoltage);
obc_error_code_t getINA230Power(ina230_device_t device, float* power);
obc_error_code_t getINA230Current(ina230_device_t device, float* power);

#ifdef __cplusplus
}
#endif
