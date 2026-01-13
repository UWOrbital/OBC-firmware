#ifdef __cplusplus  // to ensure that mock C file works with test C++ file
extern "C" {
#endif

// for putting a mock value on the i2c register for testing
void setMockBusVoltageValue(float expectedVoltage);
void setMockCurrentValue(float expectedCurrent);
void setMockPowerValue(float expectedPower);
void setMockShuntVoltageValue(float expectedPower);

#ifdef __cplusplus
}
#endif
