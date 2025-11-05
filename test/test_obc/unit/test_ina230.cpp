// When testing:
// Comment out obc_i2c_io.c from test/test_obc/unit/CMakeLists.txt
// Put it back when done testing

#include "obc_errors.h"
#include "ina230.h"
#include "mock_i2c_hal.h" // included to manually choose values mock i2c for different test cases
#include <assert.h>
#include <gtest/gtest.h>

#define INA230_I2C_ADDRESS_ONE 0b1000000U
#define INA230_I2C_ADDRESS_TWO 0b1000001U

//   OBC_ERR_CODE_SUCCESS = 0,
//   OBC_ERR_CODE_INVALID_ARG = 2
//   OBC_ERR_CODE_MUTEX_TIMEOUT = 4,
//   OBC_ERR_CODE_FAILED_FILE_READ = 704,

// --------- INIT TESTS ---------

TEST(TestINA230, InitSuccess) {
  obc_error_code_t err = initINA230();
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

TEST(TestINA230, ReadAndDisableIfAlert) {
  ina230_device_t device;
  obc_error_code_t err = readAndDisableIfAlert(device);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

// --------- BUS VOLTAGE TESTS ---------

TEST(TestINA230, GetBusVoltageSuccess) {
  float voltage = 0;
  obc_error_code_t err = getINA230BusVoltageForDevice(0, &voltage);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

TEST(TestINA230, GetBusVoltage_NullBufferPointer) {
  float voltage = 0;
  obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, NULL);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetBusVoltage_InvalidI2CAddress) {
  float voltage = 0;
  obc_error_code_t err = getINA230BusVoltage(0b1010110, &voltage);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetBusVoltage_LastI2CAddress) {
  float voltage = 0;
  obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_TWO, &voltage);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

TEST(TestINA230, GetBusVoltage_I2CAddressOutOfRange) {
  float voltage = 0;
  obc_error_code_t err = getINA230BusVoltage(INA230_DEVICE_COUNT, &voltage);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetBusVoltage_CorrectVoltage) {
  float voltage = 0;
  setMockBusVoltageValue(5.12f);
  obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, &voltage);
  EXPECT_FLOAT_EQ(voltage, 5.12f);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);  // 0x1000 = 4096, voltage = 4096 * 0.00125 = 5.12
}

TEST(TestINA230, GetBusVoltage_MultipleFunctionCalls) {
  for (int i = 0; i < 5; ++i) {
    float voltage = 0;
    setMockBusVoltageValue(5.12f);
    obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
    EXPECT_FLOAT_EQ(voltage, 5.12f);
  }
}

TEST(TestINA230, MaxRegisterVal) {
  // bus voltage register is 16-bits, so max value is 0xFFFF
  // since each bit is 1.25 mV, the max voltage is 65535 * 0.00125 = 81.91875 V
  // in mock_i2c_hal.c, i2cReadReg() returns 0xFFFF
  float voltage = 0;
  setMockBusVoltageValue(81.91875f);
  obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, &voltage);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
  EXPECT_FLOAT_EQ(voltage, 81.91875f);
}

// --------- POWER TESTS ---------

TEST(TestINA230, PowerSuccess) {
  float power = 0;
  obc_error_code_t err = getINA230PowerForDevice(0, &power);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

TEST(TestINA230, GetPower_NullBufferPointer) {
  float power = 0;
  obc_error_code_t err = getINA230Power(INA230_I2C_ADDRESS_ONE, NULL);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetPower_InvalidI2CAddress) {
  float power = 0;
  obc_error_code_t err = getINA230Power(0b1010110, &power);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

// --------- CURRENT TESTS ---------

TEST(TestINA230, CurrentSuccess) {
  float power = 0;
  obc_error_code_t err = getINA230CurrentForDevice(0, &power);
  EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

TEST(TestINA230, GetCurrent_NullBufferPointer) {
  float power = 0;
  obc_error_code_t err = getINA230Current(INA230_I2C_ADDRESS_ONE, NULL);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetCurrent_InvalidI2CAddress) {
  float power = 0;
  obc_error_code_t err = getINA230Current(0b1010110, &power);
  EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, CurrentMultipleCalls) {
  float power = 0;
  obc_error_code_t err;
  for (int i = 0; i < 4; ++i) {
    err = getINA230CurrentForDevice(0, &power);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
  }
}