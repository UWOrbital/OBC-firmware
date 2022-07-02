/**
 * @file obc_i2c_io.h
 * @author Daniel Gobalakrishnan
 * @date 2022-07-01
 */
#ifndef OBC_I2C_IO_H
#define OBC_I2C_IO_H

#include "stdint.h"

/**
 * @brief Initialize the I2C bus mutex
 */
void i2c_mutex_init(void);

/**
 * @brief Send a buffer of bytes to a device on the I2C bus
 * @param sAddr 
 * @param size 
 * @param buf 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2c_send(uint8_t sAddr, uint16_t size, void *buf);

/**
 * @brief Receive a buffer of bytes from a device on the I2C bus
 * @param sAddr 
 * @param size 
 * @param buf 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2c_receive(uint8_t sAddr, uint16_t size, void *buf);

/**
 * @brief Read byte(s) from a device's register(s).
 * @param sAddr 
 * @param reg 
 * @param data 
 * @param numBytes 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2c_read_register(uint8_t sAddr, uint8_t reg, uint8_t *data, uint16_t numBytes);

/**
 * @brief Write byte(s) to a device's register(s).
 * @param sAddr 
 * @param reg 
 * @param data 
 * @param numBytes 
 * @return 1 if successful, 0 if not 
 */
uint8_t i2c_write_register(uint8_t sAddr, uint8_t reg, uint8_t *data, uint8_t numBytes);

#endif /* OBC_I2C_IO_H */