#include "ina230.h"
#include "obc_i2c_io.h"
#include "tca6424.h"
#include "obc_logging.h"

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

// ------------------  INA230 IC Related General Configuration Addresses/Bitfields ------------- //
#define INA230_CONFIG_REGISTER_ADDR 0x00U
#define INA230_MASK_ENABLE_REGISTER_ADDR 0x06U
#define INA230_ALERT_LIMIT_REGISTER_ADDR 0x07U
#define INA230_CALIBRATION_REGISTER_ADDR 0x08U

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

// ------------------  INA230 Device Configuration ------------- //
typedef struct {
  uint8_t i2cDeviceAddress;

  // TCA GPIO Expander ports
  uint8_t tcaAlertPort;
  uint8_t tcaEnablePort;

  // Values for the configuration register
  uint8_t configurationMode;
  uint8_t configurationShunt;
  uint8_t configurationAvg;
  uint8_t configurationBus;

  // Values for other registers
  uint16_t maskEnableRegister;
  uint16_t calibrationRegister;
  uint16_t alertRegister;
} ina230_config_t;

static const ina230_config_t ina230Devices[] = {[INA230_DEVICE_ONE] = {.i2cDeviceAddress = INA230_I2C_ADDRESS_ONE,
                                                                       .tcaAlertPort = INA230_ONE_ALERT_PIN,
                                                                       .tcaEnablePort = INA230_ONE_ENABLE_PIN},

                                                [INA230_DEVICE_TWO] = {.i2cDeviceAddress = INA230_I2C_ADDRESS_TWO,
                                                                       .tcaAlertPort = INA230_TWO_ALERT_PIN,
                                                                       .tcaEnablePort = INA230_TWO_ENABLE_PIN}};

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, uint8_t i2cAddress);
static obc_error_code_t initTca6424PinState();

obc_error_code_t initINA230() {
  obc_error_code_t errCode;
  for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
    const ina230_config_t device = ina230Devices[i];
    const uint16_t configurationRegister = (device.configurationMode << INA230_CONFIG_MODE_SHIFT) |
                                           (device.configurationShunt << INA230_CONFIG_SHU_SHIFT) |
                                           (device.configurationAvg << INA230_CONFIG_AVG_SHIFT) |
                                           (device.configurationBus << INA230_CONFIG_BUS_SHIFT);

    uint8_t configRegisterUnpacked[] = {configurationRegister & 0xFF, configurationRegister >> 8};
    uint8_t maskEnRegisterUnpacked[] = {device.maskEnableRegister & 0xFF, device.maskEnableRegister >> 8};
    uint8_t alertRegisterUnpacked[] = {device.alertRegister & 0xFF, device.alertRegister >> 8};
    uint8_t calibrationRegisterUnpacked[] = {device.calibrationRegister & 0xFF, device.calibrationRegister >> 8};

    uint8_t deviceAddress = device.i2cDeviceAddress;
    RETURN_IF_ERROR_CODE(writeINA230Register(INA230_CONFIG_REGISTER_ADDR, configRegisterUnpacked,
                                             sizeof(configRegisterUnpacked) / sizeof(configRegisterUnpacked[0]),
                                             deviceAddress));
    RETURN_IF_ERROR_CODE(writeINA230Register(INA230_MASK_ENABLE_REGISTER_ADDR, maskEnRegisterUnpacked,
                                             sizeof(maskEnRegisterUnpacked) / sizeof(maskEnRegisterUnpacked[0]),
                                             deviceAddress));
    RETURN_IF_ERROR_CODE(writeINA230Register(INA230_ALERT_LIMIT_REGISTER_ADDR, alertRegisterUnpacked,
                                             sizeof(alertRegisterUnpacked) / sizeof(alertRegisterUnpacked[0]),
                                             deviceAddress));
    RETURN_IF_ERROR_CODE(writeINA230Register(
        INA230_CALIBRATION_REGISTER_ADDR, calibrationRegisterUnpacked,
        sizeof(calibrationRegisterUnpacked) / sizeof(calibrationRegisterUnpacked[0]), deviceAddress));
  }

  RETURN_IF_ERROR_CODE(initTca6424PinState());
  return OBC_ERR_CODE_SUCCESS;
}

inline obc_error_code_t readAndDisableIfAlert(ina230_device_t device) {
  uint32_t IOPortValue = 0;
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(readTCA642CompleteInput(&IOPortValue));

  for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
    uint8_t pinLocation = ina230Devices[i].tcaEnablePort;
    uint8_t index = ((pinLocation & 0x0F) + ((pinLocation >> 1) & 0x18));
    if (IOPortValue & (0b1 << index)) {
      uint8_t drivePort = INA230_DISABLE_LOAD;
      RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(pinLocation, drivePort));
    }
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, uint8_t i2cAddress) {
  if (data == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (i2cAddress != INA230_I2C_ADDRESS_ONE && i2cAddress != INA230_I2C_ADDRESS_TWO) return OBC_ERR_CODE_INVALID_ARG;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(i2cWriteReg(i2cAddress, regAddress, data, size));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t initTca6424PinState() {
  obc_error_code_t errCode;
  for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
    ina230_config_t device = ina230Devices[i];
    RETURN_IF_ERROR_CODE(configureTCA6424APin(device.tcaAlertPort, TCA6424A_GPIO_CONFIG_INPUT));
    RETURN_IF_ERROR_CODE(configureTCA6424APin(device.tcaEnablePort, TCA6424A_GPIO_CONFIG_OUTPUT));
    RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(device.tcaEnablePort, INA230_ENABLE_LOAD));
  }
  return OBC_ERR_CODE_SUCCESS;
}
