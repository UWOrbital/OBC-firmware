#ifndef DRIVERS_INCLUDE_OBC_I2C_IO_H_
#define DRIVERS_INCLUDE_OBC_I2C_IO_H_

#include <stdint.h>

/**
 * @brief Initialize the I2C bus mutex
 */
void initI2CMutex(void);

/**
 * @brief Send a buffer of bytes to a device on the I2C bus
 * @param sAddr 
 * @param size 
 * @param buf 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cSendTo(uint8_t sAddr, uint16_t size, void *buf);

/**
 * @brief Receive a buffer of bytes from a device on the I2C bus
 * @param sAddr 
 * @param size 
 * @param buf 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cReceiveFrom(uint8_t sAddr, uint16_t size, void *buf);

/**
 * @brief Read byte(s) from a device's register(s).
 * @param sAddr 
 * @param reg 
 * @param data 
 * @param numBytes 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cReadReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes);

/**
 * @brief Write byte(s) to a device's register(s).
 * @param sAddr 
 * @param reg 
 * @param data 
 * @param numBytes 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2cWriteReg(uint8_t sAddr, uint8_t reg, uint8_t *data, uint8_t numBytes);

#endif /* DRIVERS_INCLUDE_OBC_I2C_IO_H_ */ 