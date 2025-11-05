#ifdef __cplusplus  // ensure that mock C file works with test C++ file
extern "C" {
#endif

// for choosing the value read from the i2c register for test cases
void setMockBusVoltageValue(float expectedVoltage);

#ifdef __cplusplus
}
#endif