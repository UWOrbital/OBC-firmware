#include "ina230.h"
#include "obc_i2c_io.h"
#include "tca6424.h"
#include "obc_logging.h"
#include <gio.h>
#include "obc_board_config.h"
#include <stdio.h>
#include <assert.h>

// ------------------  TCA6424 IC Related Pins ------------- //

// enable pins for device one
#define INA230_ONE_ENABLE_PIN TCA6424A_PIN_03
#define INA230_TWO_ENABLE_PIN TCA6424A_PIN_01

// alert pins for device two
#define INA230_ONE_ALERT_PIN TCA6424A_PIN_02
#define INA230_TWO_ALERT_PIN TCA6424A_PIN_00

#define INA230_ENABLE_LOAD TCA6424A_GPIO_HIGH
#define INA230_DISABLE_LOAD TCA6424A_GPIO_LOW
#define INA230_ALERT_HIGH TCA6424A_GPIO_HIGH

#define INA_REG_CONF_BUFF_SIZE 2
#define I2C_TRANSFER_TIMEOUT_TICKS pdMS_TO_TICKS(100)  // 100 ms in RTOS ticks

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

static const ina230_config_t ina230Devices[] = {
    [INA230_DEVICE_ONE] = {.i2cDeviceAddress = INA230_I2C_ADDRESS_ONE,
                           .tcaAlertPort = INA230_ONE_ALERT_PIN,
                           .tcaEnablePort = INA230_ONE_ENABLE_PIN,
                           .configurationMode = INA230_DEFAULT_MODE,
                           .configurationShunt = INA230_DEFAULT_SHUNT_CONV_TIME,
                           .configurationBus = INA230_DEFAULT_BUS_CONV_TIME,
                           .configurationAvg = INA230_DEFAULT_AVERAGING,
                           .calibrationRegister = INA230_CALIBRATION_VALUE,
                           .maskEnableRegister = INA230_MASK_ENABLE_NONE,
                           .alertRegister = INA230_ALERT_LIMIT_NONE},
    [INA230_DEVICE_TWO] = {.i2cDeviceAddress = INA230_I2C_ADDRESS_TWO,
                           .tcaAlertPort = INA230_TWO_ALERT_PIN,
                           .tcaEnablePort = INA230_TWO_ENABLE_PIN,
                           .configurationMode = INA230_DEFAULT_MODE,
                           .configurationShunt = INA230_DEFAULT_SHUNT_CONV_TIME,
                           .configurationBus = INA230_DEFAULT_BUS_CONV_TIME,
                           .configurationAvg = INA230_DEFAULT_AVERAGING,
                           .calibrationRegister = INA230_CALIBRATION_VALUE,
                           .maskEnableRegister = INA230_MASK_ENABLE_NONE,
                           .alertRegister = INA230_ALERT_LIMIT_NONE},
    // initialized only 2 devices for now; initialize the rest if used
    [INA230_DEVICE_THREE] = {0},
    [INA230_DEVICE_FOUR] = {0},
    [INA230_DEVICE_FIVE] = {0},
    [INA230_DEVICE_SIX] = {0},
    [INA230_DEVICE_SEVEN] = {0},
    [INA230_DEVICE_EIGHT] = {0},
    [INA230_DEVICE_NINE] = {0},
    [INA230_DEVICE_TEN] = {0},
    [INA230_DEVICE_ELEVEN] = {0}};

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, ina230_device_t device);
static obc_error_code_t initTca6424PinState();

/**
 * @brief Initializes the INA230 devices
 *
 * This function initializes all INA230 devices by configuring their registers
 * and setting up the corresponding TCA6424 pins for alerts and enable control via initTca6424PinState().
 *
 * @return OBC_ERR_CODE_SUCCESS if initialization is successful,
 *         otherwise returns an appropriate error code
 */
obc_error_code_t initINA230() {
  obc_error_code_t errCode;
  // for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
  //   const ina230_config_t device = ina230Devices[i];
  //   const uint16_t configurationRegister = (device.configurationMode << INA230_CONFIG_MODE_SHIFT) |
  //                                          (device.configurationShunt << INA230_CONFIG_SHU_SHIFT) |
  //                                          (device.configurationAvg << INA230_CONFIG_AVG_SHIFT) |
  //                                          (device.configurationBus << INA230_CONFIG_BUS_SHIFT);
  //
  //   uint8_t configRegisterUnpacked[] = {configurationRegister >> 8, configurationRegister & 0xFF};
  //   uint8_t maskEnRegisterUnpacked[] = {device.maskEnableRegister >> 8, device.maskEnableRegister & 0xFF};
  //   uint8_t alertRegisterUnpacked[] = {device.alertRegister >> 8, device.alertRegister & 0xFF};
  //   uint8_t calibrationRegisterUnpacked[] = {device.calibrationRegister >> 8, device.calibrationRegister & 0xFF};
  //
  //   RETURN_IF_ERROR_CODE(writeINA230Register(INA230_CONFIG_REGISTER_ADDR, configRegisterUnpacked,
  //                                            sizeof(configRegisterUnpacked) / sizeof(configRegisterUnpacked[0]), i));
  //   RETURN_IF_ERROR_CODE(writeINA230Register(INA230_MASK_ENABLE_REGISTER_ADDR, maskEnRegisterUnpacked,
  //                                            sizeof(maskEnRegisterUnpacked) / sizeof(maskEnRegisterUnpacked[0]), i));
  //   RETURN_IF_ERROR_CODE(writeINA230Register(INA230_ALERT_LIMIT_REGISTER_ADDR, alertRegisterUnpacked,
  //                                            sizeof(alertRegisterUnpacked) / sizeof(alertRegisterUnpacked[0]), i));
  //   RETURN_IF_ERROR_CODE(
  //       writeINA230Register(INA230_CALIBRATION_REGISTER_ADDR, calibrationRegisterUnpacked,
  //                           sizeof(calibrationRegisterUnpacked) / sizeof(calibrationRegisterUnpacked[0]), i));
  // }
  const ina230_config_t device = ina230Devices[0];
  const uint16_t configurationRegister = (device.configurationMode << INA230_CONFIG_MODE_SHIFT) |
                                         (device.configurationShunt << INA230_CONFIG_SHU_SHIFT) |
                                         (device.configurationAvg << INA230_CONFIG_AVG_SHIFT) |
                                         (device.configurationBus << INA230_CONFIG_BUS_SHIFT);

  uint8_t configRegisterUnpacked[] = {configurationRegister >> 8, configurationRegister & 0xFF};
  uint8_t maskEnRegisterUnpacked[] = {device.maskEnableRegister >> 8, device.maskEnableRegister & 0xFF};
  uint8_t alertRegisterUnpacked[] = {device.alertRegister >> 8, device.alertRegister & 0xFF};
  uint8_t calibrationRegisterUnpacked[] = {device.calibrationRegister >> 8, device.calibrationRegister & 0xFF};

  RETURN_IF_ERROR_CODE(writeINA230Register(INA230_CONFIG_REGISTER_ADDR, configRegisterUnpacked,
                                           sizeof(configRegisterUnpacked) / sizeof(configRegisterUnpacked[0]), 0));
  RETURN_IF_ERROR_CODE(writeINA230Register(INA230_MASK_ENABLE_REGISTER_ADDR, maskEnRegisterUnpacked,
                                           sizeof(maskEnRegisterUnpacked) / sizeof(maskEnRegisterUnpacked[0]), 0));
  RETURN_IF_ERROR_CODE(writeINA230Register(INA230_ALERT_LIMIT_REGISTER_ADDR, alertRegisterUnpacked,
                                           sizeof(alertRegisterUnpacked) / sizeof(alertRegisterUnpacked[0]), 0));
  RETURN_IF_ERROR_CODE(
      writeINA230Register(INA230_CALIBRATION_REGISTER_ADDR, calibrationRegisterUnpacked,
                          sizeof(calibrationRegisterUnpacked) / sizeof(calibrationRegisterUnpacked[0]), 0));

  RETURN_IF_ERROR_CODE(initTca6424PinState());
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Reads the TCA6424 input and disables INA230 devices if an alert is detected
 *
 * This function reads the complete input from the TCA6424 and checks for alerts
 * on the INA230 devices. If an alert is detected, it disables the corresponding device.
 *
 * @param device INA230 device enum
 * @return OBC_ERR_CODE_SUCCESS if operation is successful,
 *         otherwise returns an appropriate error code
 */
inline obc_error_code_t readAndDisableIfAlert(ina230_device_t device) {
  uint32_t IOPortValue = 0;
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(readTCA642CompleteInput(&IOPortValue));  // reads 24 bit input of TCA GPIO Expander
  uint8_t pinLocation = ina230Devices[device].tcaAlertPort;
  uint8_t index = ((pinLocation & 0x0F) + ((pinLocation >> 1) & 0x18));
  if (IOPortValue & (0b1 << index)) {
    uint8_t drivePort = INA230_DISABLE_LOAD;
    RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(pinLocation, drivePort));
  }
  return OBC_ERR_CODE_SUCCESS;
}
/**
 * @brief Writes data to an INA230 register
 *
 * This function writes data to a specified register of an INA230 device.
 *
 * @param regAddress The address of the register to write to
 * @param data Pointer to the data to be written
 * @param size The size of the data to be written
 * @param device INA230 device index
 * @return OBC_ERR_CODE_SUCCESS if write is successful,
 *         otherwise returns an appropriate error code
 */

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, ina230_device_t device) {
  if (data == NULL || device >= INA230_DEVICE_COUNT) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;
  uint8_t dataSize[1] = {0};
  RETURN_IF_ERROR_CODE(i2cWriteReg(INA230_I2C_ADDRESS_ONE, 0x00, dataSize, 1));
  gioSetBit(STATE_MGR_DEBUG_LED_GIO_PORT, STATE_MGR_DEBUG_LED_GIO_BIT, 1);
  return OBC_ERR_CODE_SUCCESS;
}
/**
 * @brief Initializes the TCA6424 pin states for INA230 devices
 *
 * This function configures the TCA6424 pins used for alerts and enable control
 * of the INA230 devices. It sets up the alert pins as inputs and the enable pins as outputs.
 *
 * @return OBC_ERR_CODE_SUCCESS if initialization is successful,
 *         otherwise returns an appropriate error code
 */
static obc_error_code_t initTca6424PinState() {
  obc_error_code_t errCode;
  for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
    ina230_config_t device = ina230Devices[i];
    RETURN_IF_ERROR_CODE(configureTCA6424APin(
        device.tcaAlertPort, TCA6424A_GPIO_CONFIG_INPUT));  // alert pin is output of ina230 and input to tca
    RETURN_IF_ERROR_CODE(configureTCA6424APin(
        device.tcaEnablePort, TCA6424A_GPIO_CONFIG_OUTPUT));  // alert pin is input of ina230 and output of tca
    RETURN_IF_ERROR_CODE(
        driveTCA6424APinOutput(device.tcaEnablePort, INA230_ENABLE_LOAD));  // set the pin enable pin to high
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Gets INA230 shunt voltage
 *
 * Reads the 16-bit shunt voltage register (MSB first) from the specified INA230 device.
 * Converts the raw register value to a signed voltage in volts (LSB = 2.5μV).
 *
 * @param device INA230 device enum
 * @param shuntVoltage Pointer to store the shunt voltage in volts
 * @return OBC_ERR_CODE_SUCCESS if operation is successful,
 *         otherwise returns an appropriate error code
 */
obc_error_code_t getINA230ShuntVoltage(ina230_device_t device, float* shuntVoltage) {
  if (shuntVoltage == NULL || device >= INA230_DEVICE_COUNT) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t shuntVoltageRaw[INA_REG_CONF_BUFF_SIZE] = {0};  // store 2 bytes of shunt voltage
  obc_error_code_t errCode;

  // Read the 16-bit shunt voltage register
  errCode = i2cReadReg(ina230Devices[device].i2cDeviceAddress, INA230_SHUNT_VOLTAGE_REGISTER_ADDR, shuntVoltageRaw, 2,
                       I2C_TRANSFER_TIMEOUT_TICKS);
  if (errCode != OBC_ERR_CODE_SUCCESS) return errCode;

  // Combine the two bytes into a 16-bit value
  int16_t shuntVoltageValue = (shuntVoltageRaw[0] << 8) | shuntVoltageRaw[1];

  // Convert to actual voltage (signed value)
  *shuntVoltage = shuntVoltageValue * INA230_SHUNT_VOLTAGE_LSB;

  return OBC_ERR_CODE_SUCCESS;
}

// general disable function for ina230 device

obc_error_code_t disableNoAlert(ina230_device_t device) {
  uint32_t IOPortValue = 0;
  obc_error_code_t errCode;
  uint8_t pinLocation =
      ina230Devices[device].tcaEnablePort;  // specific pin on TCA that this ina230 controls, should this be alertPort?
  uint8_t index = ((pinLocation & 0x0F) +
                   ((pinLocation >> 1) & 0x18));  // converts the pinLocation to an index in the 24 bit IOPortValue
  // disbale
  uint8_t drivePort = INA230_DISABLE_LOAD;
  RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(pinLocation, drivePort));
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Gets INA230 bus voltage
 *
 * Reads the 16-bit bus voltage register (MSB first) from the specified INA230 device.
 * Converts the raw register value to a signed voltage in volts (LSB = 1.25mV).
 *
 * @param device INA230 device enum
 * @param busVoltage Pointer to store the bus voltage in volts
 * @return OBC_ERR_CODE_SUCCESS if write is successful,
 *         otherwise return an appropriate error code
 */
obc_error_code_t getINA230BusVoltage(ina230_device_t device, float* busVoltage) {
  if (busVoltage == NULL || device >= INA230_DEVICE_COUNT) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;
  uint8_t busVoltageRaw[2] = {};
  RETURN_IF_ERROR_CODE(i2cReadReg(ina230Devices[device].i2cDeviceAddress, INA230_BUS_VOLTAGE_REGISTER_ADDR,
                                  busVoltageRaw, 2, I2C_TRANSFER_TIMEOUT_TICKS));
  uint16_t busVoltageValue = (busVoltageRaw[0] << 8) | busVoltageRaw[1];
  *busVoltage = busVoltageValue * INA230_BUS_VOLTAGE_LSB;

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Gets INA230 power
 *
 * Reads the 16-bit power register (MSB first) from the specified INA230 device.
 * Converts the raw register value to an unsigned power in watts (LSB = 25mW).
 *
 * @param ina230_device_t device enum
 * @param busVoltage Pointer to store the power in watts
 * @return OBC_ERR_CODE_SUCCESS if write is successful,
 *         otherwise return an appropriate error code
 */
obc_error_code_t getINA230Power(ina230_device_t device, float* power) {
  obc_error_code_t errCode;
  if (power == NULL || device >= INA230_DEVICE_COUNT) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  uint8_t powerRaw[INA_REG_CONF_BUFF_SIZE] = {};
  RETURN_IF_ERROR_CODE(i2cReadReg(ina230Devices[device].i2cDeviceAddress, INA230_POWER_REGISTER_ADDR, powerRaw, 2,
                                  I2C_TRANSFER_TIMEOUT_TICKS));
  uint16_t powerValue = (powerRaw[0] << 8) | powerRaw[1];
  *power = powerValue * (INA230_CURRENT_LSB * INA230_POWER_LSB_MULTIPLIER);
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Gets INA230 current
 *
 * Reads the 16-bit current register (MSB first) from the specified INA230 device.
 * Converts the raw register value to a signed current in watts (LSB = 1mA).
 *
 * @param device INA230 device enum
 * @param busVoltage Pointer to store the current in amperes
 * @return OBC_ERR_CODE_SUCCESS if write is successful,
 *         otherwise return an appropriate error code
 */
obc_error_code_t getINA230Current(ina230_device_t device, float* current) {
  obc_error_code_t errCode;
  if (current == NULL || device >= INA230_DEVICE_COUNT) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t currentRaw[INA_REG_CONF_BUFF_SIZE] = {};
  RETURN_IF_ERROR_CODE(i2cReadReg(ina230Devices[device].i2cDeviceAddress, INA230_CURRENT_REGISTER_ADDR, currentRaw, 2,
                                  I2C_TRANSFER_TIMEOUT_TICKS));
  int16_t currentValue = (currentRaw[0] << 8) | currentRaw[1];
  *current = currentValue * INA230_CURRENT_LSB;
  return OBC_ERR_CODE_SUCCESS;
}
