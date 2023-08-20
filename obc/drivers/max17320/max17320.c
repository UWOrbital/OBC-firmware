#include "max17320.h"
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

/* ----------------Nonvolatile memory configuration registers ----------------*/
#define DISABLE_WRITE_PROTECTION_VAL 0x0000
#define ENABLE_WRITE_PROTECTION_VAL 0x00F9
#define COMM_STAT_REGISTER_ADDRESS 0x0061
#define COMM_STAT_REGISTER_WRITE_MASK 0x01FF

#define BMS_NONVOLATILE_COMMAND_REGISTER 0x060
#define COPY_NV_BLOCK_COMMAND_VAL 0xE904
#define NV_RECALL_COMMAND_VAL 0xE001

typedef enum { COPY_NV, NV_RECALL } nonvolatile_cmd_t;

static obc_error_code_t isValidMemoryAddress(uint16_t* addr, uint16_t* slaveAddr);
static inline obc_error_code_t memoryAddressMatchesSlaveAddr(uint16_t addr, uint16_t slaveAddr);
static obc_error_code_t isValidTransaction(uint16_t* startAddr, uint16_t* slaveAddrPtr, uint16_t size);

static obc_error_code_t transmitCommand(nonvolatile_cmd_t cmd);

static obc_error_code_t disableWriteProtection();
static obc_error_code_t enableWriteProtection();

static obc_error_code_t writeToBmsBlockRegister(uint16_t startAddr, uint8_t* data, uint16_t size);
static obc_error_code_t readFromBmsBlockRegister(uint16_t startAddr, uint8_t* buf, uint16_t size);

static obc_error_code_t transmitCommand(nonvolatile_cmd_t cmd) {
  uint8_t buffer[2] = {0};

  switch (cmd) {
    case COPY_NV:
      memcpy(buffer, COPY_NV_BLOCK_COMMAND_VAL, 2);
      break;
    case NV_RECALL:
      memcpy(buffer, NV_RECALL_COMMAND_VAL, 2);
      break;
  }

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(BMS_NONVOLATILE_COMMAND_REGISTER, buffer, 2));
}

obc_error_code_t initalizeConfigurationRegisters() {}

static obc_error_code_t isValidMemoryAddress(uint16_t* addr, uint16_t* slaveAddr) {
  bool isValid = (((addr >= BMS_VOLATILE_LOWER) && (addr <= BMS_VOLATILE_UPPER)) ||
                  ((addr >= BMS_NONVOLATILE_LOWER) && (addr <= BMS_NONVOLATILE_UPPER)));

  if (!isValid) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint16_t addrTmp = *addr;
  *addr = (addrTmp & BMS_INTERNAL_MEM_MASK);
  *slaveAddr = (addrTmp & BMS_SLAVE_BIT_MASK) ? BMS_I2C_SLAVE_ADDR_1 : BMS_I2C_SLAVE_ADDR_0;
  return OBC_ERR_CODE_SUCCESS;
}

static inline obc_error_code_t memoryAddressMatchesSlaveAddr(uint16_t addr, uint16_t slaveAddr) {
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
    uint32_t addrTmp = startAddr + off;
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
}

static obc_error_code_t enableWriteProtection() {
  obc_error_code_t errCode;

  uint8_t buffer[] = {(uint8_t)(ENABLE_WRITE_PROTECTION_VAL >> 8), (uint8_t)(ENABLE_WRITE_PROTECTION_VAL & 0x00FF)};
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(COMM_STAT_REGISTER_ADDRESS, buffer, 2));
  RETURN_IF_ERROR_CODE(writeToBmsBlockRegister(COMM_STAT_REGISTER_ADDRESS, buffer, 2));
}
