#include "max17320.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_i2c_io.h"

#include <stdint.h>

#define BMS_RANGE_ONE_LOWER 0x000u
#define BMS_RANGE_ONE_UPPER 0x0FFu
#define BMS_RANGE_TWO_LOWER 0x180u
#define BMS_RANGE_TWO_UPPER 0x1FFu

#define BMS_MEM_MAP_MASK 0x00FFu
#define BMS_MAXIMUM_WRITE_ATTEMPT_COUNT 10u

#define BMS_SLAVE_ADDR_RANGE_ONE 0x6C
#define BMS_SLAVE_ADDR_RANGE_TWO 0x16

#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

static obc_error_code_t initiateRead(uint16_t addr, uint16_t* value);
static obc_error_code_t initiateWrite(uint16_t addr, uint16_t value);
static obc_error_code_t mapMemoryAddressToSlave(uint16_t addr, uint8_t* internalAddr, uint8_t* slaveAddr);

obc_error_code_t readBmsRegister(bms_register_t* data) {
  if (data == NULL) return OBC_ERR_CODE_INVALID_ARG;

  uint16_t dataValue;
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(initiateRead((uint16_t)data->address, &dataValue));

  bms_threshold_value_t threshold = {0};
  if (data->isThreshold) {
    switch (data->address) {
      default:
        threshold =
            (bms_threshold_value_t){.lowerThreshold = dataValue & 0xFF, .upperThreshold = (dataValue >> 8) & 0xFF};
    }
    data->threshold = threshold;
  } else
    data->configurationValue = dataValue;

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeBmsRegister(bms_register_t* data) {
  if (data == NULL) return OBC_ERR_CODE_INVALID_ARG;

  uint16_t dataValue = 0;
  switch (data->address) {
    default:
      dataValue = (data->isThreshold) ? ((data->threshold.upperThreshold << 8) & data->threshold.lowerThreshold)
                                      : data->configurationValue;
  }

  obc_error_code_t errCode = 0;
  RETURN_IF_ERROR_CODE(initiateWrite(data->address, dataValue));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t initBmsInterface(max17320_config_t* config) {
  obc_error_code_t errCode = 0;
  for (uint8_t i = 0; i < BMS_MAX_CONFIGURATION_REGISTERS; ++i) {
    RETURN_IF_ERROR_CODE(writeBmsRegister(&config->configuration[i]));
  }
  return OBC_ERR_CODE_SUCCESS;
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
  if (value == NULL) return OBC_ERR_CODE_INVALID_ARG;

  uint8_t buffer[2] = {0};
  uint8_t slaveAddr = 0, internalAddr = 0;
  obc_error_code_t errCode = 0;
  RETURN_IF_ERROR_CODE(mapMemoryAddressToSlave(addr, &internalAddr, &slaveAddr));
  RETURN_IF_ERROR_CODE(i2cReadReg(slaveAddr, internalAddr, buffer, 2, I2C_TRANSFER_TIMEOUT));

  *value = (buffer[1] << 8) | buffer[0];
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Initiates a register write using the I2C bus.
 * @param addr 16-bit internal memory addrrss of the register that needs to be written to.
 * @param value The value to be written to the register.
 * @return Error code.
 * OBC_ERR_CODE_SUCCESS if successful.
 * OBC_ERR_CODE_INVALID_ARG if the addr is not of type bms_register_t.
 */
static obc_error_code_t initiateWrite(uint16_t addr, uint16_t value) {
  obc_error_code_t errCode = 0;

  uint8_t buffer[2] = {value & 0xFF, (value >> 8) & 0xFF};
  uint8_t slaveAddr = 0, internalAddr = 0;
  RETURN_IF_ERROR_CODE(mapMemoryAddressToSlave(addr, &internalAddr, &slaveAddr));
  RETURN_IF_ERROR_CODE(i2cWriteReg(slaveAddr, internalAddr, buffer, 2));
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
static obc_error_code_t mapMemoryAddressToSlave(uint16_t addr, uint8_t* internalAddr, uint8_t* slaveAddr) {
  bool isValid;

  isValid = ((addr <= BMS_RANGE_ONE_UPPER) || (addr >= BMS_RANGE_TWO_LOWER && addr <= BMS_RANGE_TWO_UPPER));
  if (!isValid) return OBC_ERR_CODE_INVALID_ARG;

  *internalAddr = addr & BMS_MEM_MAP_MASK;
  *slaveAddr = (addr >= BMS_RANGE_ONE_UPPER) ? BMS_SLAVE_ADDR_RANGE_TWO : BMS_SLAVE_ADDR_RANGE_ONE;
  return OBC_ERR_CODE_SUCCESS;
}
