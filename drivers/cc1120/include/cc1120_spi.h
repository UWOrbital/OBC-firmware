#ifndef DRIVERS_CC1120_INCLUDE_CC1120_SPI_H_
#define DRIVERS_CC1120_INCLUDE_CC1120_SPI_H_

#include <stdint.h>
#include "obc_errors.h"

/**
 * @brief - Reads from consecutive registers from the CC1120.
 * 
 * @param addr - The address of the first register to read.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120ReadSpi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Reads from consecutive extended address space registers on the CC1120
 * 
 * @param addr - The address of the first register to read.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120ReadExtAddrSpi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Writes to consecutive registers on the CC1120.
 * 
 * @param addr - The address of the first register to write to.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120WriteSpi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Writes to consecutive extended address space registers on the CC1120.
 * 
 * @param addr - The address of the first register to write to.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120WriteExtAddrSpi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief Calls a strobe command on the CC1120.
 * 
 * @param addr - The address of the strobe command.
 * @return OBC_ERR_CODE_SUCCESS - If the strobe command was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120StrobeSpi(uint8_t addr);

/**
 * @brief - Reads consecutive registers from the FIFO memory.
 * 
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the status byte is invalid.
 */
obc_error_code_t cc1120ReadFifo(uint8_t data[], uint8_t len);

/**
 * @brief - Writes consecutive registers to the FIFO memory.
 * 
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the status byte is invalid.
 */
obc_error_code_t cc1120WriteFifo(uint8_t data[], uint8_t len);

/**
 * @brief - Reads consecutive registers directly from the FIFO on the CC1120.
 * 
 * @param addr - The address of the first register to read. Range 0x00 - 0xFF.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120ReadFifoDirect(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Writes consecutive registers directly to the FIFO on the CC1120.
 * 
 * @param addr - The address of the first register to write to. Range 0x00 - 0xFF.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120WriteFifoDirect(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Reads the status register on the CC1120 and consecutively sends a byte over SPI.
 * 
 * @param data - The data to send to the status register.
 * @return OBC_ERR_CODE_SUCCESS - If the status byte is valid.
 * @return OBC_ERR_CODE_CC1120_INVALID_STATUS_BYTE - If the status byte is invalid.
 */
obc_error_code_t cc1120SendByteReceiveStatus(uint8_t data);

#endif /* DRIVERS_CC1120_INCLUDE_CC1120_SPI_H_ */
