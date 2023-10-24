#include "test_temp.h"
#include "OBC-firmware/obc/drivers/lm75bd/lm75bd.h"
#include "obc_sci_io.h"

#define EXPECTED_TEMP_RANGE1 10.0f
#define EXPECTED_TEMP_RANGE2 30.0f
#define EXPECTED_THYST 75.0f
#define EXPECTED_TOS 80.0f
#define EXPECTED_CONF 0x00

#define LM75BD_CONF_BUFF_SIZE 1U

void testLm75bd(void) {
  sciPrintf("Testing temp sensor...\r\n");

  obc_error_code_t errCode;

  lm75bd_config_t config;
  uint8_t configBuff[LM75BD_CONF_BUFF_SIZE] = {1};

  float temp = 0.0f;
  float tHyst = 0.0f;
  float tOS = 0.0f;

  errCode = readConfigLM75BD(&config);

  if (errCode != OBC_ERR_CODE_SUCCESS) sciPrintf("Config Read Testing error: %d\r\n", errCode);

  if (configBuff[0] != EXPECTED_CONF)
    sciPrintf("Testing error: read register value %d does not match expected value of 0\r\n", configBuff[0]);

  else
    sciPrintf("Testing success: read config value equals default value of 0\r\n");

  errCode = readTempLM75BD(LM75BD_OBC_I2C_ADDR, &temp);
  if (errCode != OBC_ERR_CODE_SUCCESS) sciPrintf("Temp Read Testing error: %d\r\n", errCode);

  if (temp < EXPECTED_TEMP_RANGE1)
    sciPrintf("Testing error: temperature falls below expected range of 10.0 - 30.0 degrees celsius\r\n");

  else if (temp > EXPECTED_TEMP_RANGE2)
    sciPrintf("Testing error: temperature is above expected range of 10.0 - 30.0 degrees celsius\r\n");

  else
    sciPrintf("Testing success: temperature is within the expected range of 10.0 - 30.0 degrees celsius\r\n");

  errCode = readThystLM75BD(LM75BD_OBC_I2C_ADDR, &tHyst);
  if (errCode != OBC_ERR_CODE_SUCCESS) sciPrintf("Thyst Read Testing error: %d\r\n", errCode);
  if (tHyst != EXPECTED_THYST)
    sciPrintf("Testing error: read tHyst value %d does not equal expected value of 75.0 degrees celsius\r\n", tHyst);

  else
    sciPrintf("Testing success: read tHyst value equals the default tHyst value of 75.0 degrees\r\n");
  errCode = readTosLM75BD(LM75BD_OBC_I2C_ADDR, &tOS);
  if (errCode != OBC_ERR_CODE_SUCCESS) sciPrintf("tOS Read Testing error: %d\r\n", errCode);

  if (tOS != EXPECTED_TOS)
    sciPrintf("Testing error: read tOS value %d does not equal expected value of 80.0 degrees celsius\r\n", tOS);

  else
    sciPrintf("Testing success: read tOS value equals the default tOS value of 80.0 degrees\r\n");
}
