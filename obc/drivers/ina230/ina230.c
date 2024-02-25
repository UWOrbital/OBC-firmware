#include "ina230.h"
#include "obc_i2c_io.h"
#include "tca6424.h"

#define INA230_I2C_ADDRESS_ONE 0b1000000U
#define INA230_I2C_ADDRESS_TWO 0b1000001U

// ------------------  TCA6424 IC Related Pins ------------- //
#define INA230_ONE_ENABLE_PIN TCA6424A_PIN_03
#define INA230_TWO_ENABLE_PIN TCA6424A_PIN_01

#define INA230_ONE_ALERT_PIN TCA6424A_PIN_02
#define INA230_TWO_ALERT_PIN TCA6424A_PIN_00

#define INA230_ENABLE_LOAD TCA6424A_GPIO_HIGH
#define INA230_DISABLE_LOAD TCA6424A_GPIO_LOW
#define INA230_ALERT_HIGH TCA6424A_GPIO_HIGH

// ------------------  INA230 IC Related Configuration Values ------------- //
#define MAX_VOLTAGE_LIMIT_INA1 0x7E000
#define MAX_VOLTAGE_LIMIT_INA2 0x7E000

#define INA230_CONFIGURATION_REGISTER 0x00U
#define INA230_MASK_ENABLE_REGISTER 0x06U
#define INA230_ALERT_LIMIT_REGISTER 0x07U

#define INA230_EM_SOL_EVENT_EN_MASK (0x0001 << 15)
#define INA230_EM_ALERT_LATCH_DISABLE_MASK (0x0000 << 0)

#define INA230_CONFIG_MODE_MASK (0b111 << 0)
#define INA2340_CONFIG_SHU_MASK (0b100 << 3)
#define INA230_CONFIG_AVG_MASK (0b001 << 9)

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, uint8_t i2cAddress);
static obc_error_code_t initTca6424PinState();

obc_error_code_t initINA230Interface() {
  uint16_t maskEnableRegister = INA230_EM_ALERT_LATCH_DISABLE_MASK | INA230_EM_SOL_EVENT_EN_MASK;
  uint8_t maskEnRegisterUnpacked[] = {maskEnableRegister & 0xFF, (maskEnableRegister >> 8) & 0xFF};

  uint16_t configRegister = INA230_CONFIG_MODE_MASK | INA2340_CONFIG_SHU_MASK | INA230_CONFIG_AVG_MASK;
  uint8_t configRegisterUnpacked[] = {configRegister & 0xFF, (configRegister >> 8) & 0xFF};

  uint8_t shuntAlertINA1Unpacked[] = {MAX_VOLTAGE_LIMIT_INA1 & 0xFF, (MAX_VOLTAGE_LIMIT_INA1 >> 8) & 0xFF};
  uint8_t shuntAlertINA2Unpacked[] = {MAX_VOLTAGE_LIMIT_INA2 & 0xFF, (MAX_VOLTAGE_LIMIT_INA2 >> 8) & 0xFF};

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_MASK_ENABLE_REGISTER, &maskEnRegisterUnpacked[0], 2, INA230_I2C_ADDRESS_ONE));
  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_MASK_ENABLE_REGISTER, &maskEnRegisterUnpacked[0], 2, INA230_I2C_ADDRESS_TWO));

  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_CONFIGURATION_REGISTER, &configRegisterUnpacked[0], 2, INA230_I2C_ADDRESS_ONE));
  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_CONFIGURATION_REGISTER, &configRegisterUnpacked[0], 2, INA230_I2C_ADDRESS_TWO));

  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_ALERT_LIMIT_REGISTER, &shuntAlertINA1Unpacked[0], 2, INA230_I2C_ADDRESS_ONE));
  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_ALERT_LIMIT_REGISTER, &shuntAlertINA2Unpacked[0], 2, INA230_I2C_ADDRESS_TWO));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readAndDisableIfAlert(ina230_device_t device) {
  uint8_t pinLocation = (device == INA230_ONE) ? INA230_ONE_ALERT_PIN : INA230_TWO_ALERT_PIN;
  TCA6424A_pin_state_t inaAlertState = {.pinLocation = pinLocation, .IOPortValue = 0};
  RETURN_IF_ERROR_CODE(readTCA6424APinInput(&inaAlertState));

  if (inaAlertState.IOPortValue == INA230_ALERT_HIGH) {
    inaAlertState.pinLocation = (device == INA230_ONE) ? INA230_ONE_ENABLE_PIN : INA230_TWO_ENABLE_PIN,
    inaAlertState.IOPortValue = INA230_DISABLE_LOAD;
    RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(inaAlertState));
  }
}

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, uint8_t i2cAddress) {
  if (data == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (i2cAddress != INA230_I2C_ADDRESS_ONE && i2cAddress != INA230_I2C_ADDRESS_TWO) return OBC_ERR_CODE_INVALID_ARG;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(i2cWriteReg(i2cAddress, regAddress, data, size));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t initTca6424PinState() {}
