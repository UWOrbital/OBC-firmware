#include "max17320.h"
#include "max17320_defs.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define BMS_VOLATILE_LOWER 0x000
#define BMS_VOLATILE_UPPER 0x0FF
#define BMS_NONVOLATILE_LOWER 0x180
#define BMS_NONVOLATILE_UPPER 0x1FF

#define BMS_INTERNAL_MEM_MASK 0x00FF
#define BMS_SLAVE_BIT_MASK (1 << 8)

#define BMS_I2C_SLAVE_ADDR_0 15
#define BMS_I2C_SLAVE_ADDR_1 20

#define BMS_MAXIMUM_WRITE_ATTEMPT_COUNT 10

/* ---------------- Nonvolatile memory configuration registers ----------------*/
#define DISABLE_WRITE_PROTECTION_VAL 0x0000
#define ENABLE_WRITE_PROTECTION_VAL 0x00F9
#define COMM_STAT_REGISTER_ADDRESS 0x0061
#define COMM_STAT_REGISTER_WRITE_MASK 0x01FF
#define COMM_STAT_NV_ERR_BIT_MASK (0x0001 << 2)

#define BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS 0x060
#define COPY_NV_BLOCK_COMMAND_VAL 0xE904
#define WRITE_RESET_COMMAND_VAL 0x000F
#define NV_RECALL_COMMAND_VAL 0xE001
#define AVAILABLE_UPDATES_VAL 0xE29B

/* ---------------- BMS Config2 Regsiters & Macros -----------------------*/
#define BMS_CONFIG2_REGISTER_ADDRESS 0x00AB
#define BMS_FIRMWARE_RESET_COMMAND 0x8000
#define POR_CMD_MASK (0x0001 << 15)

#define AVAILABLE_UPDATE_REGISTER_ADDRESS 0x01FD

typedef enum { COPY_NV, NV_RECALL, WRITE_RESET, FIRMWARE_RESET, AVAILABLE_UPDATES_CHECK } nonvolatile_cmd_t;

static obc_error_code_t memoryAddressMatchesSlaveAddr(uint16_t addr, uint16_t slaveAddr);
static obc_error_code_t isValidTransaction(uint16_t* startAddr, uint16_t* slaveAddr, uint16_t size);
static obc_error_code_t isValidMemoryAddress(uint16_t* addr, uint16_t* slaveAddr);

static obc_error_code_t transmitCommand(nonvolatile_cmd_t cmd);
static obc_error_code_t disableWriteProtection();
static obc_error_code_t enableWriteProtection();

static obc_error_code_t availableUpdatesStatusCheck();
static obc_error_code_t readDefaultNVConfiguration(uint16_t* addresses, uint8_t* returnSize);
static obc_error_code_t writeDefaultNVConfiguration(uint16_t* indices, uint8_t Size);
static obc_error_code_t checkStatusBitfield(uint16_t address, uint16_t bitMask, bool* bit);

static obc_error_code_t writeToBmsBlockRegister(uint16_t startAddr, uint8_t* data, uint16_t size);
static obc_error_code_t readFromBmsBlockRegister(uint16_t addr, uint8_t* buf, uint16_t size);
static obc_error_code_t readValueFromBmsBlockRegister(uint16_t addr, uint16_t* value);
static obc_error_code_t writeValueBmsBlockRegister(uint16_t startAddr, uint16_t value);

obc_error_code_t readConfigurationRegister(uint16_t address, uint16_t* returnDataPtr) {
  if (returnDataPtr == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(readValueFromBmsBlockRegister(address, returnDataPtr));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t initalizeConfigurationRegisters() {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(availableUpdatesStatusCheck());

  uint16_t addressIndices[BMS_CONFIGURATION_REGISTER_COUNT] = {0};
  uint8_t size = 0;
  readDefaultNVConfiguration(addressIndices, &size);

  bool statusBit = 1;
  uint8_t count = 0;
  do {
    RETURN_IF_ERROR_CODE(disableWriteProtection());
    RETURN_IF_ERROR_CODE(writeDefaultNVConfiguration(addressIndices, size));
    RETURN_IF_ERROR_CODE(disableWriteProtection());
    RETURN_IF_ERROR_CODE(transmitCommand(COPY_NV));
    // Delay
    RETURN_IF_ERROR_CODE(checkStatusBitfield(COMM_STAT_REGISTER_ADDRESS, COMM_STAT_NV_ERR_BIT_MASK, &statusBit));
    ++count;
  } while ((statusBit != 0) || (count > BMS_MAXIMUM_WRITE_ATTEMPT_COUNT));
  if (statusBit == 1) {
    return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT;
  }

  RETURN_IF_ERROR_CODE(transmitCommand(WRITE_RESET_COMMAND_VAL));
  // 10 ms delay
  RETURN_IF_ERROR_CODE(disableWriteProtection());
  RETURN_IF_ERROR_CODE(transmitCommand(FIRMWARE_RESET));

  count = 0;
  statusBit = 0;
  for (volatile uint8_t i = 0; i < BMS_MAXIMUM_WRITE_ATTEMPT_COUNT; ++i) {
    RETURN_IF_ERROR_CODE(checkStatusBitfield(BMS_CONFIG2_REGISTER_ADDRESS, POR_CMD_MASK, &statusBit));
    if (statusBit == 0) break;
    if (i == BMS_MAXIMUM_WRITE_ATTEMPT_COUNT) {
      return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT;
    }
  }
  RETURN_IF_ERROR_CODE(enableWriteProtection());
}

static obc_error_code_t writeDefaultNVConfiguration(uint16_t* indices, uint8_t size) {
  if (indices == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  for (uint8_t i = 0; i < size; ++i) {
    configuration_value_map_t addressMap = configurationAddresses[indices[i]];
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(writeValueBmsBlockRegister(addressMap.address, addressMap.value));
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t readDefaultNVConfiguration(uint16_t* buffer, uint8_t* returnSize) {
  if ((buffer == NULL) || (returnSize == NULL)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t size = 0;
  uint16_t returnData;
  for (uint16_t i = 0; i < BMS_CONFIGURATION_REGISTER_COUNT; ++i) {
    configuration_value_map_t map = configurationAddresses[i];
    readConfigurationRegister(map.address, &returnData);
    if (returnData != map.value) {
      buffer[size] = i;
      ++size;
    }
  }
  *returnSize = size;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t readValueFromBmsBlockRegister(uint16_t addr, uint16_t* value) {
  if (value == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t buffer[2] = {0};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(readFromBmsBlockRegister(addr, buffer, 2));
  *value = (((uint16_t)(buffer[1]) << 8) | ((uint16_t)buffer[0]));
}

static obc_error_code_t writeValueBmsBlockRegister(uint16_t addr, uint16_t value) {
  uint8_t buffer[2] = {0};
  memcpy(buffer, &value, 2);
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(addr, buffer, 2));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t availableUpdatesStatusCheck() {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(disableWriteProtection());
  RETURN_IF_ERROR_CODE(transmitCommand(AVAILABLE_UPDATES_CHECK));
  // Delay
  uint16_t value = 0;
  RETURN_IF_ERROR_CODE(readValueFromBmsBlockRegister(AVAILABLE_UPDATE_REGISTER_ADDRESS, &value));
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

static obc_error_code_t transmitCommand(nonvolatile_cmd_t cmd) {
  uint8_t buffer[2] = {0};
  uint16_t address = {0};
  uint16_t value = 0;
  switch (cmd) {
    case COPY_NV:
      value = COPY_NV_BLOCK_COMMAND_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case NV_RECALL:
      value = NV_RECALL_COMMAND_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case WRITE_RESET:
      value = WRITE_RESET_COMMAND_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
      break;
    case AVAILABLE_UPDATES_CHECK:
      value = AVAILABLE_UPDATES_VAL;
      address = BMS_NONVOLATILE_COMMAND_REGISTER_ADDRESS;
    case FIRMWARE_RESET:
      value = BMS_FIRMWARE_RESET_COMMAND;
      address = BMS_CONFIG2_REGISTER_ADDRESS;
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;
  memcpy(buffer, &value, 2);
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(address, buffer, 2));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t checkStatusBitfield(uint16_t address, uint16_t bitMask, bool* bit) {
  if (bit == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  obc_error_code_t errCode;
  uint16_t bitfield = {0};
  RETURN_IF_ERROR_CODE(readValueFromBmsBlockRegister(address, &bitfield));
  *bit = bitfield & bitMask;

  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t isValidMemoryAddress(uint16_t* addr, uint16_t* slaveAddr) {
  uint16_t addrTmp = addr;
  bool isValid = (((addrTmp >= BMS_VOLATILE_LOWER) && (addrTmp <= BMS_VOLATILE_UPPER)) ||
                  ((addrTmp >= BMS_NONVOLATILE_LOWER) && (addrTmp <= BMS_NONVOLATILE_UPPER)));

  if (!isValid) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  *addr &= BMS_SLAVE_BIT_MASK;
  *slaveAddr = (addrTmp & BMS_SLAVE_BIT_MASK) ? BMS_I2C_SLAVE_ADDR_1 : BMS_I2C_SLAVE_ADDR_0;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t memoryAddressMatchesSlaveAddr(uint16_t addr, uint16_t slaveAddr) {
  uint32_t slaveAddrTmp = (addr & BMS_SLAVE_BIT_MASK) ? BMS_I2C_SLAVE_ADDR_1 : BMS_I2C_SLAVE_ADDR_0;
  if (slaveAddr != slaveAddrTmp) {
    return OBC_ERR_CODE_BMS_MEM_SLAVE_ADDR_MISMATCH;
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t isValidTransaction(uint16_t* startAddr, uint16_t* slaveAddr, uint16_t size) {
  bool isValid = 0;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(isValidMemoryAddress(startAddr, slaveAddr));

  for (uint16_t off = 0; off < size; off++) {
    uint32_t addrTmp = *startAddr + off;
    if (memoryAddressMatchesSlaveAddr(addrTmp, *slaveAddr) == OBC_ERR_CODE_BMS_MEM_SLAVE_ADDR_MISMATCH) {
      return OBC_ERR_CODE_BMS_NEW_TRANSACTION;
    }
  }
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t writeToBmsBlockRegister(uint16_t startAddr, uint8_t* data, uint16_t size) {
  uint16_t slaveAddr = 0;
  uint16_t startCommAddr = startAddr;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(isValidTransaction(&startCommAddr, &slaveAddr, size));
  return (i2cWriteReg(slaveAddr, startCommAddr, data, size));
}

static obc_error_code_t readFromBmsBlockRegister(uint16_t startAddr, uint8_t* buf, uint16_t size) {
  uint16_t slaveAddr = 0;
  uint16_t startCommAddr = startAddr;

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(isValidTransaction(&startCommAddr, &slaveAddr, size));
  return (i2cReadReg(slaveAddr, startCommAddr, buf, size));
}

static obc_error_code_t disableWriteProtection() {
  obc_error_code_t errCode;

  uint8_t buffer[] = {(uint8_t)(DISABLE_WRITE_PROTECTION_VAL >> 8), (uint8_t)(DISABLE_WRITE_PROTECTION_VAL & 0x00FF)};
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(COMM_STAT_REGISTER_ADDRESS, buffer, 2));
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(COMM_STAT_REGISTER_ADDRESS, buffer, 2));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t enableWriteProtection() {
  obc_error_code_t errCode;

  uint8_t buffer[] = {(uint8_t)(ENABLE_WRITE_PROTECTION_VAL >> 8), (uint8_t)(ENABLE_WRITE_PROTECTION_VAL & 0x00FF)};
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(COMM_STAT_REGISTER_ADDRESS, buffer, 2));
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(COMM_STAT_REGISTER_ADDRESS, buffer, 2));
  return OBC_ERR_CODE_SUCCESS;
}
