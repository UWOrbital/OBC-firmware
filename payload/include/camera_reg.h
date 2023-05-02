#ifndef PAYLOAD_INCLUDE_CAMERA_REG_H_
#define PAYLOAD_INCLUDE_CAMERA_REG_H_

#include "stdint.h"
#include "obc_spi_io.h"
#include "ov5642_regs.h"
#include "obc_board_config.h"

/**
 * @brief Write to a camera register over SPI
 * @param addr Register address to write to
 * @param data Data to send
 * @return Error code indicating if the write was successful
 */
obc_error_code_t writeReg(uint16_t addr, uint16_t data);

/**
 * @brief Read a camera register over SPI
 * @param addr Register address to read from
 * @param rx_data Buffer to store received data
 * @return Error code indicating if the read was successful
 */
obc_error_code_t readReg(uint16_t addr, uint16_t *rx_data);

/**
 * @brief Read 8 bits from a 16 bit register over I2C
 * @param regID Register address to write to
 * @param regDat Data to send
 * @return Error code indicating if the write was successful
 */
obc_error_code_t wrSensorReg16_8(int regID, int regDat);

/**
 * @brief Write 8 bits to a 16 bit register over I2C
 * @param regID Register address to read from
 * @param regDat Buffer to store received data
 * @return Error code indicating if the read was successful
 */
obc_error_code_t rdSensorReg16_8(int regID, int* regDat);

/**
 * @brief Write to a list of registers over I2C
 * @param reglist List of registers and data to write
 * @return Error code indicating if the writes were successful
 */
obc_error_code_t wrSensorRegs16_8(const struct sensor_reg reglist[]);

/**
 * @brief Select an I2C port on the TCA9458a multiplexer
 * @param tca Port number to select
 * @return Error code indicating if an ACK was received
 */
obc_error_code_t tcaSelect(uint8_t tca);

/**
 * @brief Read one bit from a register over SPI
 * @param addr Address to read from
 * @param bit Bit to read
 */
uint8_t getBit(uint8_t addr, uint8_t bit);

#endif /* PAYLOAD_INCLUDE_CAMERA_REG_H_ */
