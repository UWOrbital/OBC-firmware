#include "max17320.h"
#include "max17320_defs.h"

#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define BMS_VOLATILE_LOWER 0x0000u
#define BMS_VOLATILE_UPPER 0x00FFu
#define BMS_NONVOLATILE_LOWER 0x0180u
#define BMS_NONVOLATILE_UPPER 0x01FFu

#define BMS_MEM_MAP_MASK 0x00FFu
#define BMS_MAXIMUM_WRITE_ATTEMPT_COUNT 10u

/* ---------------- Nonvolatile memory configuration registers ----------------*/
#define BMS_DISABLE_WRITE_PROTECTION_VAL 0x0000u
#define BMS_ENABLE_WRITE_PROTECTION_VAL 0x00F9u
#define BMS_COMM_STAT_REGISTER_ADDRESS 0x0061u
#define BMS_COMM_STAT_REGISTER_WRITE_MASK 0x01FFu
#define BMS_COMM_STAT_NV_ERR_BIT_MASK (0x0001u << 2)

#define BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS 0x060u
#define BMS_COPY_NV_BLOCK_COMMAND_VAL 0xE904u
#define BMS_WRITE_RESET_COMMAND_VAL 0x000Fu
#define BMS_NV_RECALL_COMMAND_VAL 0xE001u
#define BMS_AVAILABLE_UPDATES_VAL 0xE29Bu

/* ---------------- BMS Config2 Regsiters & Macros -----------------------*/
#define BMS_CONFIG2_REGISTER_ADDRESS 0x00ABu
#define BMS_FIRMWARE_RESET_COMMAND 0x8000u
#define BMS_POR_CMD_MASK (0x0001u << 15)

#define BMS_AVAILABLE_UPDATE_REGISTER_ADDRESS 0x01FDu

#define BMS_IC_RESET_WAIT_MS 10u
#define BMS_RECALL_WAIT_MS 5u
#define BMS_BLOCK_COPY_WAIT_MS 7360u
#define BMS_UPDATE_PAGE_WAIT_MS 1280u
#define BMS_FIRMWARE_RESET_WAIT_MS 5u

/*---------------------- Read/Write protection helper functions ------------------ */
static obc_error_code_t disableWriteProtection();
static obc_error_code_t enableWriteProtection();
static obc_error_code_t availableUpdatesStatusCheck();
static obc_error_code_t resetFirmware();

/*----------------------Register/Bitfield status check helper function ------------------ */
static obc_error_code_t transmitCommand(nonvolatile_cmd_t cmd);
static obc_error_code_t verifyConfiguration(configuration_value_map_t* config, uint8_t size,
                                            configuration_value_map_t* buffer, uint8_t* returnSize);
static obc_error_code_t writeConfiguration(configuration_value_map_t* indices, uint8_t size);
static obc_error_code_t unlockAndWriteConfig(configuration_value_map_t* config, uint8_t size,
                                             threshold_config_t* thresholdConfig, uint8_t thresholdSize);
static obc_error_code_t statusCheckBitfield(uint16_t address, uint16_t bitMask, bool* bit);
static obc_error_code_t writeThresholdRegisters(threshold_config_t* config, uint8_t size);

/* ---------------------- Read/write functions --------------- */
static obc_error_code_t initiateRead(uint16_t addr, uint16_t* value);
static obc_error_code_t initiateWrite(uint16_t addr, uint16_t value);
static obc_error_code_t mapMemoryAddressToSlave(uint16_t addr, uint8_t* slaveAddr);
static obc_error_code_t validateBmsRegister(bms_register_t address);

obc_error_code_t readBmsRegister(bms_register_t address, uint16_t* data) {
  if (data == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(validateBmsRegister(address));
  return initiateRead(address, data);
}

obc_error_code_t initBmsInterface(max17320_config_t config) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(writeConfiguration(config.volatileConfiguration, config.volatileConfigSize));

  configuration_value_map_t addressIndices[BMS_NV_CONFIGURATION_REGISTER_COUNT];
  uint8_t size = 0;
  verifyConfiguration(config.nonVolatileConfiguration, config.nonVolatileConfigSize, addressIndices, &size);

  if (size != 0 || (config.thresholdIsNonVolatile == 1)) {
    RETURN_IF_ERROR_CODE(
        unlockAndWriteConfig(addressIndices, size, config.measurementThresholds, config.thresholdConfigSize));
  } else {
    writeThresholdRegisters(config.measurementThresholds, config.thresholdConfigSize);
  }

  RETURN_IF_ERROR_CODE(transmitCommand(BMS_WRITE_RESET_COMMAND_VAL));
  vTaskDelay(pdMS_TO_TICKS(BMS_IC_RESET_WAIT_MS));
  RETURN_IF_ERROR_CODE(disableWriteProtection());
  RETURN_IF_ERROR_CODE(resetFirmware());
  RETURN_IF_ERROR_CODE(enableWriteProtection());
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Unlocks and writes the configuration values to the register BMS IC.
 * @param config An array of configuration_value_map_t that need to be programmed.
 * @param size The size of the config array.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the config is NULL or contains an invalid address not of enum type bms_register_t.
 */
static obc_error_code_t unlockAndWriteConfig(configuration_value_map_t* config, uint8_t size,
                                             threshold_config_t* thresholdConfig, uint8_t thresholdSize) {
  bool statusBit = {1};
  for (uint8_t i = 0; i < BMS_MAXIMUM_WRITE_ATTEMPT_COUNT; ++i) {
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(availableUpdatesStatusCheck());
    RETURN_IF_ERROR_CODE(disableWriteProtection());
    RETURN_IF_ERROR_CODE(writeConfiguration(config, size));
    RETURN_IF_ERROR_CODE(writeThresholdRegisters(thresholdConfig, thresholdSize));
    RETURN_IF_ERROR_CODE(disableWriteProtection());
    RETURN_IF_ERROR_CODE(transmitCommand(COPY_NV));
    vTaskDelay(pdMS_TO_TICKS(BMS_BLOCK_COPY_WAIT_MS));
    RETURN_IF_ERROR_CODE(
        statusCheckBitfield(BMS_COMM_STAT_REGISTER_ADDRESS, BMS_COMM_STAT_NV_ERR_BIT_MASK, &statusBit));
    if (statusBit == 0) break;
  }
  if (statusBit == 1) {
    return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT;
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Writes the configuration key-value pairs to their corresponding registers.
 * @param config An array of configuration_value_map_t that need to be programmed.
 * @param size The size of the config array.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the config is NULL or contains an invalid address not of enum type bms_register_t.
 */
static obc_error_code_t writeConfiguration(configuration_value_map_t* config, uint8_t size) {
  if (config == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  for (uint8_t i = 0; i < size; ++i) {
    configuration_value_map_t addressMap = config[i];
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(initiateWrite(addressMap.address, addressMap.value));
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Iteratively reads through addresses in config and adds them to the buffer if the default value
 * does not match the current value in the register.
 * @param config An array of configuration_value_map_t that need to be checked.
 * @param size The size of the config array.
 * @param buffer An array containing elements from config that do not have their default value programmed.
 * @param returnSize The size of the buffer array.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the config, buffer or returnSize pointer
 * is NULL or if config contains an invalid address not in bms_register_t.
 */
static obc_error_code_t verifyConfiguration(configuration_value_map_t* config, uint8_t size,
                                            configuration_value_map_t* buffer, uint8_t* returnSize) {
  if ((buffer == NULL) || (returnSize == NULL)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  uint8_t bufferSize = 0;
  for (uint16_t i = 0; i < size; ++i) {
    configuration_value_map_t map = config[i];

    uint16_t returnData;
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(initiateRead(map.address, &returnData));
    if (returnData != map.value) {
      buffer[bufferSize] = map;
      ++bufferSize;
    }
  }
  *returnSize = bufferSize;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Helper function to reset firmware and continously check the CONFIG2 register
 * until the reset is confirmed.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT if maximum allowed check attempts have been made.
 */
static obc_error_code_t resetFirmware() {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(transmitCommand(FIRMWARE_RESET));

  bool statusBit = 1;
  for (uint16_t i = 0; i < BMS_MAXIMUM_WRITE_ATTEMPT_COUNT; ++i) {
    RETURN_IF_ERROR_CODE(statusCheckBitfield(BMS_CONFIG2_REGISTER_ADDRESS, BMS_POR_CMD_MASK, &statusBit));
    if (statusBit == 0) break;
    vTaskDelay(pdMS_TO_TICKS(BMS_FIRMWARE_RESET_WAIT_MS));
  }
  if (statusBit == 1) {
    return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT;
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Iteratively converts each member of type threshold_config_t into configuration_value_map_t
 * and programs it into the BMS IC.
 * @param config An array of type threshold_config_t that need to be programmed in the IC.
 * @param size The size of the config array.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the config points to NULL or an address in a threshold_config_t
 * is invalid.
 */
static obc_error_code_t writeThresholdRegisters(threshold_config_t* config, uint8_t size) {
  if (config == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  configuration_value_map_t configRegisters[BMS_THRESHOLD_CONFIGURATION_REGISTER_COUNT] = {0};
  for (uint8_t i = 0; i < size; ++i) {
    configRegisters[i].address = config[i].address;
    configRegisters[i].value = ((uint16_t)(config[i].upperTh << 8)) | ((uint16_t)(config[i].lowerTh));
  }

  return writeConfiguration(configRegisters, size);
}

/**
 *
 * @brief Initiates a register read using the I2C bus.
 * @param addr 16-bit internal memory addrrss of the register that needs to be read.
 * @param value The value read from the register.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if value points to NULL or the addr is invalid.
 */
static obc_error_code_t initiateRead(uint16_t addr, uint16_t* value) {
  if (value == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t buffer[2] = {0};
  uint8_t slaveAddr = {0};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(mapMemoryAddressToSlave(addr, &slaveAddr));
  RETURN_IF_ERROR_CODE(i2cReadReg(slaveAddr, addr, buffer, 2));

  memcpy(value, buffer, 2);
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Initiates a register write using the I2C bus.
 * @param addr 16-bit internal memory addrrss of the register that needs to be written to.
 * @param value The value to be read to the register.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the addr is not of type bms_register_t.
 */
static obc_error_code_t initiateWrite(uint16_t addr, uint16_t value) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(validateBmsRegister(addr));

  uint8_t buffer[2] = {0};
  memcpy(buffer, &value, 2);
  uint8_t slaveAddr = {0};
  RETURN_IF_ERROR_CODE(mapMemoryAddressToSlave(addr, &slaveAddr));
  RETURN_IF_ERROR_CODE(i2cWriteReg(slaveAddr, addr, buffer, 2));
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief A check to ensure that the BMS Nonvaltile memory can still be programmed.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_BMS_REACHED_MAXIMUM_CONFIG_UPDATES if the nonvolatile memory cannot
 * be updated anynore.
 */
static obc_error_code_t availableUpdatesStatusCheck() {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(disableWriteProtection());
  RETURN_IF_ERROR_CODE(transmitCommand(AVAILABLE_UPDATES_CHECK));
  vTaskDelay(pdMS_TO_TICKS(BMS_RECALL_WAIT_MS));

  uint16_t value = 0;
  RETURN_IF_ERROR_CODE(initiateRead(BMS_AVAILABLE_UPDATE_REGISTER_ADDRESS, &value));
  RETURN_IF_ERROR_CODE(enableWriteProtection());

  uint8_t countValue = ((uint8_t)(value >> 8) ^ (uint8_t)(value & 0x00FF));
  uint8_t count = 0;
  for (uint8_t mask = 1; mask > 0; mask = mask << 1) {
    if (mask & countValue) {
      ++count;
    }
  }

  if (count == 8) return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_CONFIG_UPDATES;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Transmits a command to the BMS IC to perform some action such as Copy the NV block or reset.
 * @param command The command to be executed, of type nonvolatile_cmd_t.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the command is not of type nonvolatile_cmd_t.
 */
static obc_error_code_t transmitCommand(nonvolatile_cmd_t command) {
  uint16_t address = 0;
  uint16_t value = 0;
  switch (command) {
    case COPY_NV:
      value = BMS_COPY_NV_BLOCK_COMMAND_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case NV_RECALL:
      value = BMS_NV_RECALL_COMMAND_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case WRITE_RESET:
      value = BMS_WRITE_RESET_COMMAND_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case AVAILABLE_UPDATES_CHECK:
      value = BMS_AVAILABLE_UPDATES_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case FIRMWARE_RESET:
      value = BMS_FIRMWARE_RESET_COMMAND;
      address = BMS_CONFIG2_REGISTER_ADDRESS;
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(initiateWrite(address, value));
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Checks the value of the bit at an address and position.
 * @param address The address where the bits needs to be read.
 * @param bitMask A bit mask reflecting the position of the bit.
 * @param bit Pointer to the bit that needs to be returned.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if bit points to NULL or if the address is invalid in BMS memory space.
 */
static obc_error_code_t statusCheckBitfield(uint16_t address, uint16_t bitMask, bool* bit) {
  if (bit == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;
  uint16_t bitfield = {0};
  RETURN_IF_ERROR_CODE(initiateRead(address, &bitfield));
  *bit = bitfield & bitMask;

  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Disables the write protection for the BMS IC, allowing the driver to write values.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if macro BMS_COMM_STAT_REGISTER_ADDRESS is incorrect.
 */
static obc_error_code_t disableWriteProtection() {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(initiateWrite(BMS_COMM_STAT_REGISTER_ADDRESS, BMS_DISABLE_WRITE_PROTECTION_VAL));
  RETURN_IF_ERROR_CODE(initiateWrite(BMS_COMM_STAT_REGISTER_ADDRESS, BMS_DISABLE_WRITE_PROTECTION_VAL));
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Enables the write protection for the BMS IC, blocking the driver from writing values.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if macro BMS_COMM_STAT_REGISTER_ADDRESS is incorrect.
 */
static obc_error_code_t enableWriteProtection() {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(initiateWrite(BMS_COMM_STAT_REGISTER_ADDRESS, BMS_ENABLE_WRITE_PROTECTION_VAL));
  RETURN_IF_ERROR_CODE(initiateWrite(BMS_COMM_STAT_REGISTER_ADDRESS, BMS_ENABLE_WRITE_PROTECTION_VAL));
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Maps the register address to the correct slave address.
 * @param addr 16-bit internal address within the memory space.
 * @param slaveAddr Pointer to slave address where the value will be returned.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the address is not within programmable memory space of BMS IC.
 */
static obc_error_code_t mapMemoryAddressToSlave(uint16_t addr, uint8_t* slaveAddr) {
  bool isValid;
#if (BMS_VOLATILE_LOWER > 0)
  isValid = ((addr >= BMS_VOLATILE_LOWER) && (addr <= BMS_VOLATILE_UPPER)) ||
            ((addr >= BMS_NONVOLATILE_LOWER) && (addr <= BMS_NONVOLATILE_UPPER));
#else
  isValid = ((addr <= BMS_VOLATILE_UPPER)) || ((addr >= BMS_NONVOLATILE_LOWER) && (addr <= BMS_NONVOLATILE_UPPER));
#endif
  if (!isValid) return OBC_ERR_CODE_INVALID_ARG;
  *slaveAddr = addr & BMS_MEM_MAP_MASK;
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Validates if the address is of type
 * @param address bms_register_t address that needs to be validated.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the address is not of type bms_register_t.
 */
static obc_error_code_t validateBmsRegister(bms_register_t address) {
  switch (address) {
    case Status:
      break;
    case RepCap:
      break;
    case RepSOC:
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  return OBC_ERR_CODE_SUCCESS;
}
