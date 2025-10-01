#include "ina230.h"
#include "obc_i2c_io.h"
#include "tca6424.h"
#include "obc_logging.h"
#include <stdio.h>
#include <assert.h>

#define INA230_I2C_ADDRESS_ONE 0b1000000U
#define INA230_I2C_ADDRESS_TWO 0b1000001U

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

// ------------------  INA230 IC Related General Configuration Addresses/Bitfields ------------- //
#define INA230_CONFIG_REGISTER_ADDR 0x00U
#define INA230_MASK_ENABLE_REGISTER_ADDR 0x06U
#define INA230_ALERT_LIMIT_REGISTER_ADDR 0x07U
#define INA230_CALIBRATION_REGISTER_ADDR 0x05U
#define INA230_SHUNT_VOLTAGE_REGISTER_ADDR 0x01U
#define INA230_BUS_VOLTAGE_REGISTER_ADDR 0x02U
#define INA230_POWER_REGISTER_ADDR 0x03U
#define INA230_CURERNT_REGISTER_ADDR 0x04U

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

// macros for lsb, shunt resistor, and calibration value
#define INA230_SHUNT_VOLTAGE_LSB 0.0000025f
#define INA230_CURRENT_LSB 0.001f   // 1 mA, current least significant bit
#define INA230_SHUNT_RESISTOR 0.1f  // 0.1 ohms, shunt resistor value
#define INA230_CALIBRATION_VALUE (uint16_t)(0.00512 / (INA230_CURRENT_LSB * INA230_SHUNT_RESISTOR))

// buffer sizes
#define INA_REG_CONF_BUFF_SIZE 2

#define I2C_TRANSFER_TIMEOUT_TICKS pdMS_TO_TICKS(100) // 100 ms in RTOS ticks

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
    // uint16_t calibrationValue = INA230_CALIBRATION_VALUE;
    // uint8_t calibrationRegisterUnpacked[] = {calibrationValue & 0xFF, calibrationValue >> 8};

    RETURN_IF_ERROR_CODE(writeINA230Register(
        INA230_CALIBRATION_REGISTER_ADDR, calibrationRegisterUnpacked,
        sizeof(calibrationRegisterUnpacked) / sizeof(calibrationRegisterUnpacked[0]), deviceAddress));
  }

  RETURN_IF_ERROR_CODE(initTca6424PinState());
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Reads the TCA6424 input and disables INA230 devices if an alert is detected
 *
 * This function reads the complete input from the TCA6424 and checks for alerts
 * on the INA230 devices. If an alert is detected, it disables the corresponding device.
 *
 * @param device The INA230 device to check (currently unused in the function)
 * @return OBC_ERR_CODE_SUCCESS if operation is successful,
 *         otherwise returns an appropriate error code
 */
inline obc_error_code_t readAndDisableIfAlert(ina230_device_t device) {
  uint32_t IOPortValue = 0;
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(readTCA642CompleteInput(&IOPortValue));  // reads 24 bit input of TCA GPIO Expander

  for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
    uint8_t pinLocation =
        ina230Devices[i].tcaEnablePort;  // specific pin on TCA that this ina230 controls, should this be alertPort?
    uint8_t index = ((pinLocation & 0x0F) +
                     ((pinLocation >> 1) & 0x18));  // converts the pinLocation to an index in the 24 bit IOPortValue
    if (IOPortValue & (0b1 << index)) {             // if it's high
      uint8_t drivePort = INA230_DISABLE_LOAD;
      RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(pinLocation, drivePort));
    }
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
 * @param i2cAddress The I2C address of the INA230 device
 * @return OBC_ERR_CODE_SUCCESS if write is successful,
 *         otherwise returns an appropriate error code
 */

static obc_error_code_t writeINA230Register(uint8_t regAddress, uint8_t* data, uint8_t size, uint8_t i2cAddress) {
  if (data == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (i2cAddress != INA230_I2C_ADDRESS_ONE && i2cAddress != INA230_I2C_ADDRESS_TWO) return OBC_ERR_CODE_INVALID_ARG;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(i2cWriteReg(i2cAddress, regAddress, data, size));
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

// function to get shunt voltage
// i2cAddress = ina230 device address
// shuntVoltage = pointer to store shuntVoltage

obc_error_code_t getINA230ShuntVoltage(uint8_t i2cAddress, float* shuntVoltage) {
  if (shuntVoltage == NULL) return OBC_ERR_CODE_INVALID_ARG;

  uint8_t shuntVoltageRaw[INA_REG_CONF_BUFF_SIZE] = {0};  // store 2 bytes of shunt voltage
  obc_error_code_t errCode;

  if (i2cAddress != INA230_I2C_ADDRESS_ONE && i2cAddress != INA230_I2C_ADDRESS_TWO) return OBC_ERR_CODE_INVALID_ARG;

  // Read the 16-bit shunt voltage register
  errCode = i2cReadReg(i2cAddress, INA230_SHUNT_VOLTAGE_REGISTER_ADDR, shuntVoltageRaw, 2, I2C_TRANSFER_TIMEOUT_TICKS);  // last param not sure
  if (errCode != OBC_ERR_CODE_SUCCESS) return errCode;

  // Combine the two bytes into a 16-bit value
  int16_t shuntVoltageValue = (shuntVoltageRaw[0] << 8) | shuntVoltageRaw[1];

  // Check if the value is negative (MSB is set)
  if (shuntVoltageValue & 0x8000) {                        // if MSB is 1
    shuntVoltageValue = (shuntVoltageValue ^ 0xFFFF) + 1;  // Convert to two's complement
  }

  // Convert to actual voltage
  *shuntVoltage = shuntVoltageValue * INA230_SHUNT_VOLTAGE_LSB;

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getINA230ShuntVoltageForDevice(uint8_t deviceIndex, float* shuntVoltage) {
  if (shuntVoltage == NULL) return OBC_ERR_CODE_INVALID_ARG;

  if (deviceIndex >= INA230_DEVICE_COUNT) return OBC_ERR_CODE_INVALID_ARG;  // Check valid device index

  // Retrieve the I2C address for the specified INA230 device
  uint8_t i2cAddress = ina230Devices[deviceIndex].i2cDeviceAddress;

  // Get the shunt voltage
  return getINA230ShuntVoltage(i2cAddress, shuntVoltage);
}

void main_usage() {
  float shuntVoltage = 0;

  // Call the function for INA230 device 1 (index 0)
  obc_error_code_t errCode = getINA230ShuntVoltageForDevice(0, &shuntVoltage);
  // print shunt volatge
  printf("Shunt Voltage for INA230 Device 1: %f V\n", shuntVoltage);

  if (errCode == OBC_ERR_CODE_SUCCESS) {
    printf("Shunt Voltage for INA230 Device 1: %f V\n", shuntVoltage);
  } else {
    printf("Error reading shunt voltage for INA230 Device 1\n");
  }
}

// general disable function for ina230 device

obc_error_code_t disableNoAlert(ina230_device_t device) {
  uint32_t IOPortValue = 0;
  obc_error_code_t errCode;

  for (uint8_t i = 0; i < INA230_DEVICE_COUNT; ++i) {
    uint8_t pinLocation =
        ina230Devices[i].tcaEnablePort;  // specific pin on TCA that this ina230 controls, should this be alertPort?
    uint8_t index = ((pinLocation & 0x0F) +
                     ((pinLocation >> 1) & 0x18));  // converts the pinLocation to an index in the 24 bit IOPortValue
    // disbale
    uint8_t drivePort = INA230_DISABLE_LOAD;
    RETURN_IF_ERROR_CODE(driveTCA6424APinOutput(pinLocation, drivePort));
  }
  return OBC_ERR_CODE_SUCCESS;
}

// Notes (delete later)
// Bus voltage register: 
  // Address: 0x02
  // Size: 16 bits (2 bytes)
  // Each bit on the register represents 1.25 mV

/**
 * @brief Gets the bus voltage
 * 
 * @param i2cAddress The I2C address of the INA230 device
 * @return OBC_ERR_CODE_SUCCESS if write is successful,
 *         otherwise return an appropriate error code
 */
// function to get bus voltage
obc_error_code_t getINA230BusVoltage(uint8_t i2cAddress, float* busVoltage) {
  if (busVoltage == NULL) return OBC_ERR_CODE_INVALID_ARG;
  if (i2cAddress != INA230_I2C_ADDRESS_ONE && i2cAddress != INA230_I2C_ADDRESS_TWO) return OBC_ERR_CODE_INVALID_ARG;

  obc_error_code_t errCode;
  uint8_t busVoltageRaw[2] = {};
  RETURN_IF_ERROR_CODE(i2cReadReg(i2cAddress, INA230_BUS_VOLTAGE_REGISTER_ADDR, busVoltageRaw, 2, I2C_TRANSFER_TIMEOUT_TICKS));
  int16_t busVoltageValue = (busVoltageRaw[0] << 8) | busVoltageRaw[1];
  *busVoltage = busVoltageValue * 0.00125f;

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getINA230BusVoltageForDevice(uint8_t deviceIndex, float* busVoltage) {
  if (busVoltage == NULL || deviceIndex >= INA230_DEVICE_COUNT) return OBC_ERR_CODE_INVALID_ARG;
  uint8_t i2cAddress = ina230Devices[deviceIndex].i2cDeviceAddress;
  return getINA230BusVoltage(i2cAddress, busVoltage);
}

// function to get power

// function to get current

// allow loop through
