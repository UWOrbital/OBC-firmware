// When testing:
    // Comment out obc_i2c_io.c from test/test_obc/unit/CMakeLists.txt
    // Put it back when done testing


#include "obc_errors.h"
#include "ina230.h"
#include <assert.h>
#include <gtest/gtest.h>

#define INA230_I2C_ADDRESS_ONE 0b1000000U
#define INA230_I2C_ADDRESS_TWO 0b1000001U

//   OBC_ERR_CODE_SUCCESS = 0,
//   OBC_ERR_CODE_INVALID_ARG = 2
//   OBC_ERR_CODE_MUTEX_TIMEOUT = 4,
//   OBC_ERR_CODE_FAILED_FILE_READ = 704,


// --------- SUCCESS CODE ---------

TEST(TestINA230, GetBusVoltageSuccess) {
   float voltage = 0;
    obc_error_code_t err = getINA230BusVoltageForDevice(0, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}

// --------- HARDWARE/DEPENDENCY FAILURES ---------

// need to set up mock failure  
// TEST(TestINA230, GetBusVoltage_I2CReadFailure) {
//     float voltage = 0;
//     obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, &voltage);
//     EXPECT_EQ(err, OBC_ERR_CODE_FAILED_FILE_READ);
// }

// --------- INPUT VALIDATION ---------

TEST(TestINA230, GetBusVoltage_NullBufferPointer) {
    float voltage = 0;
    obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, NULL);
    EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetBusVoltage_InvalidI2CAddress) {
    float voltage = 0;
    obc_error_code_t err = getINA230BusVoltage(0b1010110, NULL);
    EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetBusVoltage_LastI2CAddress) {
    float voltage = 0;
    obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_TWO, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

TEST(TestINA230, GetBusVoltage_I2CAddressOutOfRange) {
    float voltage = 0;
    obc_error_code_t err = getINA230BusVoltage(INA230_DEVICE_COUNT, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_INVALID_ARG);
}

// --------- NORMAL AND EDGE CASES ---------


// --------- CALCULATIONS AND DATA ---------

TEST(TestINA230, GetBusVoltage_CorrectVoltage) {
    float voltage = 0;
    obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, &voltage);
    EXPECT_FLOAT_EQ(voltage, 5.12f);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS); // 0x1000 = 4096, voltage = 4096 * 0.00125 = 5.12
}

TEST(TestINA230, GetBusVoltage_MultipleFunctionCalls) {
    for (int i = 0; i < 5; ++i) {
        float voltage = 0;
        obc_error_code_t err = getINA230BusVoltage(INA230_I2C_ADDRESS_ONE, &voltage);
        EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
        EXPECT_FLOAT_EQ(voltage, 5.12f); 
    }
}