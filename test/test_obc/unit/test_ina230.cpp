

#include "obc_errors.h"
#include "ina230.h"
#include "mock_i2c_hal.h"
#include <assert.h>
#include <gtest/gtest.h>

// include C functions in C++ file
extern "C" {
obc_error_code_t i2cReadReg(uint8_t, uint8_t, uint8_t*, uint16_t, TickType_t);
obc_error_code_t i2cWriteReg(uint8_t, uint8_t, uint8_t*, uint16_t);
}

// Google Test test environment class configuration
class INA230TestEnvironment : public ::testing::Environment {
 public:
  void SetUp() override {
    i2cReadRegFuncPtr = i2cReadReg;
    i2cWriteRegFuncPtr = i2cWriteReg;
  }
};

static ::testing::Environment* const ina230_env = ::testing::AddGlobalTestEnvironment(new INA230TestEnvironment());

#define INA230_TEST_FLOAT_TOLERANCE 0.01f

// for reference:
//   OBC_ERR_CODE_SUCCESS = 0,
//   OBC_ERR_CODE_INVALID_ARG = 2
//   OBC_ERR_CODE_MUTEX_TIMEOUT = 4,
//   OBC_ERR_CODE_FAILED_FILE_READ = 704,

// --------- INIT TESTS ---------

TEST(TestINA230, InitSuccess) { EXPECT_EQ(initINA230(), OBC_ERR_CODE_SUCCESS); }

TEST(TestINA230, ReadAndDisableIfAlert) {
  ina230_device_t device = INA230_DEVICE_ONE;
  EXPECT_EQ(readAndDisableIfAlert(device), OBC_ERR_CODE_SUCCESS);
}

// --------- SHUNT VOLTAGE TESTS ---------

TEST(TestINA230, ShuntVoltageValues) {
  float voltage = 0;
  // Max register value: 0xFFFF
  // since each bit is 2.5 μV, the voltage is -1 * 0.0000025 = -0.0000025 V
  setMockShuntVoltageValue(-0.0000025);
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, -0.0000025);
  // Min register value: 0x0000
  setMockShuntVoltageValue(0);
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, 0);
  // Lowest val: 0x8000
  // use 2's complement on 0x8000 = 1000 0000 0000 0000 --> -32768
  // since each bit is 2.5 μV, the  voltage is -32768 * 0.0000025 = -0.08192 V
  setMockShuntVoltageValue(-0.08192);
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, -0.08192);
  // Test positive value
  setMockShuntVoltageValue(0.08f);
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, 0.08f);
  // Multiple function calls
  for (int i = 0; i < 5; ++i) {
    float voltage = 0;
    setMockShuntVoltageValue(0.05f);
    obc_error_code_t err = getINA230ShuntVoltage(INA230_DEVICE_ONE, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
    EXPECT_NEAR(voltage, 0.05f, INA230_TEST_FLOAT_TOLERANCE);
  }
}

TEST(TestINA230, ShuntVoltageInvalidArguments) {
  float voltage = 0;
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_ONE, NULL), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(getINA230ShuntVoltage((ina230_device_t)0b1010110, &voltage), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetShuntVoltage_I2CAddress) {
  float voltage = 0;
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_TWO, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_EQ(getINA230ShuntVoltage(INA230_DEVICE_COUNT, &voltage), OBC_ERR_CODE_INVALID_ARG);
}

// --------- BUS VOLTAGE TESTS ---------
// bus voltage is unsigned
TEST(TestINA230, BusVoltageValues) {
  float voltage = 0;
  // bus voltage register is 16-bits, so max value is 0xFFFF
  // since each bit is 1.25 mV, the max voltage is 65535 * 0.00125 = 81.91875 V
  setMockBusVoltageValue(81.91875f);
  EXPECT_EQ(getINA230BusVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, 81.91875f);
  // Min register value: 0x0000
  setMockBusVoltageValue(0);
  EXPECT_EQ(getINA230BusVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, 0);
  // Test positive value 0x1000 = 4096, voltage = 4096 * 0.00125 = 5.12
  setMockBusVoltageValue(5.12f);
  EXPECT_EQ(getINA230BusVoltage(INA230_DEVICE_ONE, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, 5.12f);
  // Multiple function calls
  for (int i = 0; i < 5; ++i) {
    float voltage = 0;
    setMockBusVoltageValue(5.12f);
    obc_error_code_t err = getINA230BusVoltage(INA230_DEVICE_ONE, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
    EXPECT_FLOAT_EQ(voltage, 5.12f);
  }
}

TEST(TestINA230, BusVoltageInvalidArguments) {
  float voltage = 0;
  EXPECT_EQ(getINA230BusVoltage(INA230_DEVICE_ONE, NULL), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(getINA230BusVoltage((ina230_device_t)0b1010110, &voltage), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, BusVoltage_I2CAddress) {
  float voltage = 0;
  EXPECT_EQ(getINA230BusVoltage(INA230_DEVICE_TWO, &voltage), OBC_ERR_CODE_SUCCESS);
  EXPECT_EQ(getINA230BusVoltage(INA230_DEVICE_COUNT, &voltage), OBC_ERR_CODE_INVALID_ARG);
}

// --------- POWER TESTS ---------
// power is unsigned
TEST(TestINA230, PowerVoltageValues) {
  float power = 0;

  // Max register value: 0xFFFF
  // power LSB = 25 * current LSB = 25 * 0.001f = 0.025f
  // since each bit is 2.5 mW, the max power is 65535 * 0.025 = 1638.375 W
  setMockPowerValue(1638.375f);
  EXPECT_EQ(getINA230Power(INA230_DEVICE_ONE, &power), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(power, 1638.375f);
  // Min register value: 0x0000
  setMockPowerValue(0);
  EXPECT_EQ(getINA230Power(INA230_DEVICE_ONE, &power), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(power, 0);
  // Test positive value
  setMockPowerValue(5.12f);
  EXPECT_EQ(getINA230Power(INA230_DEVICE_ONE, &power), OBC_ERR_CODE_SUCCESS);
  EXPECT_NEAR(power, 5.12f, INA230_TEST_FLOAT_TOLERANCE);
  // Multiple function calls
  for (int i = 0; i < 5; ++i) {
    float power = 0;
    setMockPowerValue(0.05f);
    obc_error_code_t err = getINA230Power(INA230_DEVICE_ONE, &power);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
    EXPECT_NEAR(power, 0.05f, INA230_TEST_FLOAT_TOLERANCE);
  }
}

TEST(TestINA230, PowerInvalidArguments) {
  float power = 0;
  EXPECT_EQ(getINA230Power(INA230_DEVICE_ONE, NULL), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(getINA230Power((ina230_device_t)0b1010110, &power), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, Power_I2CAddress) {
  float power = 0;
  EXPECT_EQ(getINA230Power(INA230_DEVICE_TWO, &power), OBC_ERR_CODE_SUCCESS);
  EXPECT_EQ(getINA230Power(INA230_DEVICE_COUNT, &power), OBC_ERR_CODE_INVALID_ARG);
}

// --------- CURRENT TESTS ---------

TEST(TestINA230, CurrentValues) {
  float current = 0;
  // Max register value: 0xFFFF
  // since each bit is 1 mA, -1 * 0.001 = -0.001
  setMockCurrentValue(-0.001);
  EXPECT_EQ(getINA230Current(INA230_DEVICE_ONE, &current), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(current, -0.001);
  // Min register value: 0x0000
  setMockCurrentValue(0);
  EXPECT_EQ(getINA230Current(INA230_DEVICE_ONE, &current), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(current, 0);
  // Lowest register value: 0x8000
  // use 2's complement on 0x8000 = 1000 0000 0000 0000 --> -32768
  // since each bit is 1 mA, -32768 * 0.001 = -32.768
  setMockCurrentValue(-32.768);
  EXPECT_EQ(getINA230Current(INA230_DEVICE_ONE, &current), OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(current, -32.768);
  // Test positive value
  setMockCurrentValue(5.12f);
  EXPECT_EQ(getINA230Current(INA230_DEVICE_ONE, &current), OBC_ERR_CODE_SUCCESS);
  EXPECT_NEAR(current, 5.12f, INA230_TEST_FLOAT_TOLERANCE);
  // Multiple function calls
  for (int i = 0; i < 5; ++i) {
    float current = 0;
    setMockCurrentValue(0.05f);
    obc_error_code_t err = getINA230Current(INA230_DEVICE_ONE, &current);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
    EXPECT_NEAR(current, 0.05f, INA230_TEST_FLOAT_TOLERANCE);
  }
}

TEST(TestINA230, CurrentInvalidArguments) {
  float current = 0;
  EXPECT_EQ(getINA230Current(INA230_DEVICE_ONE, NULL), OBC_ERR_CODE_INVALID_ARG);
  EXPECT_EQ(getINA230Current((ina230_device_t)0b1010110, &current), OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, Current_I2CAddress) {
  float current = 0;
  EXPECT_EQ(getINA230Current(INA230_DEVICE_TWO, &current), OBC_ERR_CODE_SUCCESS);
  EXPECT_EQ(getINA230Current(INA230_DEVICE_COUNT, &current), OBC_ERR_CODE_INVALID_ARG);
}
