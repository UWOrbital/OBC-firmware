#include "mcp4562.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

obc_error_code_t writeWiperPosition(uint16_t pos, int wiper) {
  if ((pos == NULL || pos > MAX_WIPER_POS || pos < MIN_WIPER_POS) && !(wiper == 0 || wiper == 1)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  uint8_t writeValues[2] = {0, 0};
  writeValues[0] = wiper ? V_WIPER_1_ADDR : V_WIPER_0_ADDR;
  writeValues[0] = writeValues[0] << 4 | (I2C_WRITE << 2);
  if (pos == MAX_WIPER_POS) {
    writeValues[0] |= 0x01;
  } else
    writeValues[1] = pos;

  RETURN_IF_ERROR_CODE(
      i2cSendTo(MCP4562_OBC_I2C_ADDRESS, sizeof(writeValues), &writeValues, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readWiperPosition(uint16_t* pos, int wiper) {
  if (!(wiper == 0 || wiper == 1)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  uint8_t readCommand = wiper ? V_WIPER_1_ADDR : V_WIPER_0_ADDR;
  readCommand = readCommand << 4 | (I2C_READ << 2);

  RETURN_IF_ERROR_CODE(
      i2cSendTo(MCP4562_OBC_I2C_ADDRESS, sizeof(readCommand), &readCommand, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));
  RETURN_IF_ERROR_CODE(
      i2cReceiveFrom(MCP4562_OBC_I2C_ADDRESS, sizeof(pos), pos, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

  return OBC_ERR_CODE_SUCCESS;
};

obc_error_code_t incWiper(int wiper) {
  if (!(wiper == 0 || wiper == 1)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  uint8_t incCommand = wiper ? V_WIPER_1_ADDR : V_WIPER_0_ADDR;
  incCommand = incCommand << 4 | (I2C_INCREMENT_WIPER << 2);

  RETURN_IF_ERROR_CODE(
      i2cSendTo(MCP4562_OBC_I2C_ADDRESS, sizeof(incCommand), &incCommand, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t decWiper(int wiper) {
  if (!(wiper == 0 || wiper == 1)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;

  uint8_t decCommand = wiper ? V_WIPER_1_ADDR : V_WIPER_0_ADDR;
  decCommand = decCommand << 4 | (I2C_DECREMENT_WIPER << 2);

  RETURN_IF_ERROR_CODE(
      i2cSendTo(MCP4562_OBC_I2C_ADDRESS, sizeof(decCommand), &decCommand, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT));

  return OBC_ERR_CODE_SUCCESS;
}
