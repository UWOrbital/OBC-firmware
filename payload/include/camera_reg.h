#ifndef PAYLOAD_INCLUDE_CAMERA_REG_H_
#define PAYLOAD_INCLUDE_CAMERA_REG_H_

#include "stdint.h"
#include "obc_spi_io.h"
#include "ov5642_regs.h"

#define SPI_REG spiREG3
#define SPI_PORT spiPORT3
#define CS_NUM 1

#define CAM_I2C_WR_ADDR 0x3C
#define CAM_I2C_RD_ADDR 0x3C

#define TCA_I2C_ADDR 0x70

extern spiDAT1_t spi_config;

/**
 * @brief Write to a camera register over SPI
 * @param addr Register address to write to
 * @param data Data to send
 */
void write_reg(uint16_t addr, uint16_t data);

/**
 * @brief Read a camera register over SPI
 * @param addr Register address to read from
 * @param rx_data Buffer to store received data
 */
void read_reg(uint16_t addr, uint16_t *rx_data);

/**
 * @brief Read 8 bits from a 16 bit register over I2C
 * @param regID Register address to write to
 * @param regDat Data to send
 */
void wrSensorReg16_8(int regID, int regDat);

/**
 * @brief Write 8 bits to a 16 bit register over I2C
 * @param regID Register address to read from
 * @param regDat Buffer to store received data
 */
void rdSensorReg16_8(int regID, int* regDat);

/**
 * @brief Write to a list of registers over I2C
 * @param reglist List of registers and data to write
 */
void wrSensorRegs16_8(const struct sensor_reg reglist[]);

/**
 * @brief Select an I2C port on the TCA9458a multiplexer
 * @param tca Port number to select
 */
void tca_select(uint8_t tca);

/**
 * @brief Read one bit from a register over SPI
 * @param addr Address to read from
 * @param bit Bit to read
 */
uint8_t get_bit(uint8_t addr, uint8_t bit);

#endif /* PAYLOAD_INCLUDE_CAMERA_REG_H_ */