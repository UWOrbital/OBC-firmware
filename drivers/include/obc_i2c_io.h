#ifndef DRIVERS_INCLUDE_OBC_I2C_IO_H_
#define DRIVERS_INCLUDE_OBC_I2C_IO_H_

#include <stdint.h>

/**
 * @brief Initialize the I2C bus mutex
 */
void initI2CMutex(void);

/**
 * @brief Send a buffer of bytes to a device on the I2C bus
 * @param sAddr The slave address of the device to send to
 * @param size The number of bytes to send
 * @param buf The buffer to send
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cSendTo(uint8_t sAddr, uint16_t size, void *buf);

/**
 * @brief Receive a buffer of bytes from a device on the I2C bus
 * @param sAddr The slave address of the device to receive from
 * @param size The number of bytes to receive
 * @param buf The buffer to receive into
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cReceiveFrom(uint8_t sAddr, uint16_t size, void *buf);

/**
 * @brief Read byte(s) from a device's register(s).
 * @param sAddr The slave address of the device to read from
 * @param reg The register to read from
 * @param data The buffer to read into
 * @param numBytes The number of bytes to read
 * @note  You can read from consecutive registers by using numBytes > 1.
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cReadReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes);

/**
 * @brief Write byte(s) to a device's register(s).
 * @param sAddr The slave address of the device to write to
 * @param reg The register to write to
 * @param data The buffer to write from
 * @param numBytes The number of bytes to write
 * @note  You can write to consecutive registers by using numBytes > 1.
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cWriteReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint8_t numBytes);

#endif /* DRIVERS_INCLUDE_OBC_I2C_IO_H_ */ 