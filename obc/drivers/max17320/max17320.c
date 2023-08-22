#include "max17320.h"
#include "max17320_defs.h"

#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define BMS_VOLATILE_LOWER 0x0000
#define BMS_VOLATILE_UPPER 0x00FF
#define BMS_NONVOLATILE_LOWER 0x0180
#define BMS_NONVOLATILE_UPPER 0x01FF

#define BMS_MEM_MAP_MASK 0x00FF
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

#define BMS_IC_RESET_WAIT_MS 10
#define BMS_RECALL_WAIT_MS 5
#define BMS_BLOCK_COPY_WAIT_MS 7360
#define BMS_UPDATE_PAGE_WAIT_MS 1280

typedef enum { COPY_NV, NV_RECALL, WRITE_RESET, FIRMWARE_RESET, AVAILABLE_UPDATES_CHECK } nonvolatile_cmd_t;

/*---------------------- Read/Write protection helper functions ------------------ */
static obc_error_code_t disableWriteProtection();
static obc_error_code_t enableWriteProtection();
static obc_error_code_t availableUpdatesStatusCheck();

/*----------------------Register/Bitfield status check helper function ------------------ */
static obc_error_code_t transmitCommand(nonvolatile_cmd_t cmd);
static obc_error_code_t verifyDefaultConfiguration(configuration_value_map_t* config, uint16_t size,
                                                   configuration_value_map_t* buffer, uint8_t* returnSize);
static obc_error_code_t writeDefaultConfiguration(configuration_value_map_t* indices, uint8_t Size);
static obc_error_code_t statusCheckBitfield(uint16_t address, uint16_t bitMask, bool* bit);

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

obc_error_code_t initBmsInterface() {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(writeDefaultConfiguration(volatileConfiguration, BMS_VOL_CONFIGURATION_REGISTER_COUNT));
  RETURN_IF_ERROR_CODE(availableUpdatesStatusCheck());

  configuration_value_map_t* addressIndices[BMS_NV_CONFIGURATION_REGISTER_COUNT] = {0};
  uint8_t size = 0;
  verifyDefaultConfiguration(nonVolatileConfiguration, BMS_NV_CONFIGURATION_REGISTER_COUNT, addressIndices, &size);

  bool statusBit = {1};
  for (uint8_t i = 0; i < BMS_MAXIMUM_WRITE_ATTEMPT_COUNT; ++i) {
    RETURN_IF_ERROR_CODE(disableWriteProtection());
    RETURN_IF_ERROR_CODE(writeDefaultConfiguration(addressIndices, size));
    RETURN_IF_ERROR_CODE(disableWriteProtection());
    RETURN_IF_ERROR_CODE(transmitCommand(COPY_NV));
    vTaskDelay(pdMS_TO_TICKS(BMS_BLOCK_COPY_WAIT_MS));
    RETURN_IF_ERROR_CODE(statusCheckBitfield(COMM_STAT_REGISTER_ADDRESS, COMM_STAT_NV_ERR_BIT_MASK, &statusBit));
    if (statusBit == 0) break;
  }
  if (statusBit == 1) {
    return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT;
  }

  RETURN_IF_ERROR_CODE(transmitCommand(WRITE_RESET_COMMAND_VAL));
  vTaskDelay(pdMS_TO_TICKS(BMS_IC_RESET_WAIT_MS));
  RETURN_IF_ERROR_CODE(disableWriteProtection());
  RETURN_IF_ERROR_CODE(transmitCommand(FIRMWARE_RESET));

  statusBit = 1;
  for (uint16_t i = 0; i < BMS_MAXIMUM_WRITE_ATTEMPT_COUNT; ++i) {
    RETURN_IF_ERROR_CODE(statusCheckBitfield(BMS_CONFIG2_REGISTER_ADDRESS, POR_CMD_MASK, &statusBit));
    if (statusBit == 0) break;
  }
  if (statusBit == 1) {
    return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_COUNT;
  }
  RETURN_IF_ERROR_CODE(enableWriteProtection());
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t writeDefaultConfiguration(configuration_value_map_t* config, uint8_t size) {
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

static obc_error_code_t verifyDefaultConfiguration(configuration_value_map_t* config, uint16_t size,
                                                   configuration_value_map_t* buffer, uint8_t* returnSize) {
  if ((buffer == NULL) || (returnSize == NULL)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t size = 0;
  for (uint16_t i = 0; i < size; ++i) {
    configuration_value_map_t map = config[i];

    uint16_t returnData;
    readConfigurationRegister(map.address, &returnData);
    if (returnData != map.value) {
      buffer[size] = map;
      ++size;
    }
  }
  *returnSize = size;
  return OBC_ERR_CODE_SUCCESS;
}

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

static obc_error_code_t initiateWrite(uint16_t addr, uint16_t value) {
  uint8_t buffer[2] = {0};
  memcpy(buffer, &value, 2);

  obc_error_code_t errCode;
  uint8_t slaveAddr = {0};
  RETURN_IF_ERROR_CODE(mapMemoryAddressToSlave(addr, &slaveAddr));
  RETURN_IF_ERROR_CODE(i2cWriteReg(slaveAddr, addr, buffer, 2));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t availableUpdatesStatusCheck() {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(disableWriteProtection());
  RETURN_IF_ERROR_CODE(transmitCommand(AVAILABLE_UPDATES_CHECK));
  vTaskDelay(pdMS_TO_TICKS(BMS_RECALL_WAIT_MS));

  uint16_t value = 0;
  RETURN_IF_ERROR_CODE(initiateRead(AVAILABLE_UPDATE_REGISTER_ADDRESS, &value));
  RETURN_IF_ERROR_CODE(enableWriteProtection());

  uint8_t countValue = ((uint8_t)(value >> 8) ^ (uint8_t)(value & 0x00FF));
  uint8_t count = 0;
  for (uint8_t mask = 1; mask > 0; mask = mask << 1) {
    if (mask & countValue) {
      ++count;
    }
  }

  if ((count == 8) && (BMS_NV_CONFIGURATION_REGISTER_COUNT != 0))
    return OBC_ERR_CODE_BMS_REACHED_MAXIMUM_CONFIG_UPDATES;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t transmitCommand(nonvolatile_cmd_t command) {
  uint16_t address = 0;
  uint16_t value = 0;
  switch (command) {
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

static obc_error_code_t disableWriteProtection() {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(initiateWrite(COMM_STAT_REGISTER_ADDRESS, DISABLE_WRITE_PROTECTION_VAL));
  RETURN_IF_ERROR_CODE(initiateWrite(COMM_STAT_REGISTER_ADDRESS, DISABLE_WRITE_PROTECTION_VAL));
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t enableWriteProtection() {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE(initiateWrite(COMM_STAT_REGISTER_ADDRESS, ENABLE_WRITE_PROTECTION_VAL));
  RETURN_IF_ERROR_CODE(initiateWrite(COMM_STAT_REGISTER_ADDRESS, ENABLE_WRITE_PROTECTION_VAL));
  return OBC_ERR_CODE_SUCCESS;
}

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
