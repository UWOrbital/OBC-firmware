#ifndef PAYLOAD_INCLUDE_CAMERA_REG_H_
#define PAYLOAD_INCLUDE_CAMERA_REG_H_

#include "stdint.h"
#include "obc_spi_io.h"
#include "ov5642_regs.h"
#include "obc_board_config.h"

/**
 * @enum	camera_t
 * @brief	Primary or secondary camera.
 *
 * Enum containing camera identifiers.
*/
typedef enum {
  PRIMARY,
  SECONDARY
} camera_t;

/**
 * @struct	camera_settings_t
 * @brief	Camera settings struct
 *
 * Holds the settings for each camera.
*/
typedef struct {
  spiDAT1_t spi_config;
  uint8_t cs_num;
} cam_settings_t;

/**
 * @brief Write to a camera register over SPI
 * @param addr Register address to write to
 * @param data Data to send
 * @param cam  Camera identifier
 * @return Error code indicating if the write was successful
 */
obc_error_code_t camWriteReg(uint8_t addr, uint8_t data, camera_t cam);

/**
 * @brief Read a camera register over SPI
 * @param addr Register address to read from
 * @param rx_data Buffer to store received data
 * @param cam  Camera identifier
 * @return Error code indicating if the read was successful
 */
obc_error_code_t camReadReg(uint8_t regID, uint8_t* regDat, camera_t cam);

/**
 * @brief Write one byte to a camera over SPI
 * @param byte  Camera settings struct
 * @param cam  Camera identifier
 * @return Error code
 */
obc_error_code_t camWriteByte(uint8_t byte, camera_t cam);

/**
 * @brief Read one byte to a camera over SPI, does not handle CS assertion
 * @param byte  Camera settings struct
 * @param cam  Camera identifier
 * @return Error code
 */
obc_error_code_t camReadByte(uint8_t *byte, camera_t cam);

/**
 * @brief Read 8 bits from a 16 bit register over I2C
 * @param regID Register address to write to
 * @param regDat Data to send
 * @return Error code indicating if the write was successful
 */
obc_error_code_t camWriteSensorReg16_8(uint32_t regID, uint8_t regDat);

/**
 * @brief Write 8 bits to a 16 bit register over I2C
 * @param regID Register address to read from
 * @param regDat Buffer to store received data
 * @return Error code indicating if the read was successful
 */
obc_error_code_t camReadSensorReg16_8(uint8_t regID, uint8_t* regDat);

/**
 * @brief Write to a list of registers over I2C
 * @param reglist List of registers and data to write
 * @return Error code indicating if the writes were successful
 */
obc_error_code_t camWriteSensorRegs16_8(const struct sensor_reg reglist[]);

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
uint8_t getBit(uint8_t addr, uint8_t bit, camera_t cam);

#endif /* PAYLOAD_INCLUDE_CAMERA_REG_H_ */
