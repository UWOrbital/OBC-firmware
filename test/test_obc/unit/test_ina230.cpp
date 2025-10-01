#include "obc_errors.h"
#include "ina230.h"
#include <assert.h>
#include <gtest/gtest.h>


TEST(TestINA230, GetBusVoltageValid) {
   float voltage = 0;
    obc_error_code_t err = getINA230BusVoltageForDevice(0, &voltage);
    EXPECT_EQ(err, OBC_ERR_CODE_SUCCESS);
}
