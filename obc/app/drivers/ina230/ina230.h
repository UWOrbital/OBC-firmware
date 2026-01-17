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

// macros for LSB, shunt resistor, and calibration value
#define INA230_SHUNT_VOLTAGE_LSB 0.0000025f
#define INA230_BUS_VOLTAGE_LSB 0.00125f
#define INA230_CURRENT_LSB 0.001f   // 1 mA, current least significant bit
#define INA230_SHUNT_RESISTOR 0.1f  // 0.1 ohms, shunt resistor value
#define INA230_CALIBRATION_VALUE (uint16_t)(0.00512 / (INA230_CURRENT_LSB * INA230_SHUNT_RESISTOR))
#define INA230_POWER_LSB_MULTIPLIER 25

typedef enum { INA230_DEVICE_ONE = 0x00, INA230_DEVICE_TWO = 0x01, INA230_DEVICE_COUNT = 0x02} ina230_device_t;

// function pointers to switch between mock and real data
extern obc_error_code_t (*i2cReadRegFuncPtr)(uint8_t, uint8_t, uint8_t*, uint16_t, TickType_t);
extern obc_error_code_t (*i2cWriteRegFuncPtr)(uint8_t, uint8_t, uint8_t*, uint16_t);

obc_error_code_t initINA230();
obc_error_code_t readAndDisableIfAlert(ina230_device_t device);
obc_error_code_t getINA230ShuntVoltage(uint8_t i2cAddress, float* shuntVoltage);
obc_error_code_t getINA230ShuntVoltageForDevice(uint8_t deviceIndex, float* shuntVoltage);
void main_usage();
obc_error_code_t disableNoAlert(ina230_device_t device);
obc_error_code_t getINA230BusVoltage(uint8_t i2cAddress, float* busVoltage);
obc_error_code_t getINA230BusVoltageForDevice(uint8_t deviceIndex, float* busVoltage);
obc_error_code_t getINA230Power(uint8_t i2cAddress, float* power);
obc_error_code_t getINA230PowerForDevice(uint8_t deviceIndex, float* power);
obc_error_code_t getINA230Current(uint8_t i2cAddress, float* power);
obc_error_code_t getINA230CurrentForDevice(uint8_t deviceIndex, float* power);

#ifdef __cplusplus
}
#endif
