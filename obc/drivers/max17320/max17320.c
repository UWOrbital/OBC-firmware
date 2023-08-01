#include "max17320.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define BMS_INTERNAL_MEM_LOWER_0 0x000
#define BMS_INTERNAL_MEM_UPPER_0 0x0FF
#define BMS_INTERNAL_MEM_LOWER_1 0x180
#define BMS_INTERNAL_MEM_UPPER_1 0x1FF

#define BMS_INTERNAL_MEM_MASK 0x00FF
#define BMS_SLAVE_BIT_MASK (1 << 8)

#define BMS_I2C_SLAVE_ADDR_0 15
#define BMS_I2C_SLAVE_ADDR_1 20

static obc_error_code_t isValidMemoryAddress(uint16_t* addr, uint16_t* slaveAddr);
static inline obc_error_code_t memoryAddressMatchesSlaveAddr(uint16_t addr, uint16_t slaveAddr);
static obc_error_code_t isValidTransaction(uint16_t* startAddr, uint16_t* slaveAddrPtr, uint16_t size);
static obc_error_code_t writeToBmsBlockRegister(uint16_t startAddr, uint8_t* data, uint16_t size);
static obc_error_code_t readFromBmsBlockRegister(uint32_t addr, uint16_t size, uint8_t* buf);

static obc_error_code_t isValidMemoryAddress(uint16_t* addr, uint16_t* slaveAddr) {
  bool isValid = (((addr >= BMS_INTERNAL_MEM_LOWER_0) && (addr <= BMS_INTERNAL_MEM_UPPER_0)) ||
                  ((addr >= BMS_INTERNAL_MEM_LOWER_1) && (addr <= BMS_INTERNAL_MEM_UPPER_1)));

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

  RETURN_IF_ERROR_CODE(isValidTransaction(&startCommAddr, &slaveAddr, size));
  return (i2cWriteReg(slaveAddr, startCommAddr, data, size));
}

static obc_error_code_t readFromBmsBlockRegister(uint32_t startAddr, uint16_t size, uint8_t* buf) {
  uint16_t slaveAddr = 0;
  uint16_t startCommAddr = startAddr;

  RETURN_IF_ERROR_CODE(isValidTransaction(&startCommAddr, &slaveAddr, size));
  return (i2cReadReg(slaveAddr, startCommAddr, buf, size));
}
