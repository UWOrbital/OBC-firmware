#ifndef CC1120_SPI_H
#define CC1120_SPI_H

#include <stdint.h>
#include <stdbool.h>
#include "obc_errors.h"

#define R_BIT 1 << 7
#define BURST_BIT 1 << 6

struct cc_status {
  uint8_t res : 4;
  uint8_t state : 3;
  uint8_t chip_ready : 1;
};

union cc_st {
  struct cc_status ccst;
  uint8_t data;
};

/**
 * @brief - Reads from consecutive registers from the CC1120.
 * 
 * @param addr - The address of the first register to read.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_read_spi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Reads from consecutive extended address space registers on the CC1120
 * 
 * @param addr - The address of the first register to read.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_read_ext_addr_spi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Writes to consecutive registers on the CC1120.
 * 
 * @param addr - The address of the first register to write to.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_write_spi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Writes to consecutive extended address space registers on the CC1120.
 * 
 * @param addr - The address of the first register to write to.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_write_ext_addr_spi(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief Calls a strobe command on the CC1120.
 * 
 * @param addr - The address of the strobe command.
 * @return OBC_ERR_CODE_SUCCESS - If the strobe command was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_strobe_spi(uint8_t addr);

/**
 * @brief - Reads consecutive registers from the FIFO memory.
 * 
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the status byte is invalid.
 */
obc_error_code_t cc1120_read_fifo(uint8_t data[], uint8_t len);

/**
 * @brief - Writes consecutive registers to the FIFO memory.
 * 
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the status byte is invalid.
 */
obc_error_code_t cc1120_write_fifo(uint8_t data[], uint8_t len);

/**
 * @brief - Reads consecutive registers directly from the FIFO on the CC1120.
 * 
 * @param addr - The address of the first register to read. Range 0x00 - 0xFF.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_read_fifo_direct(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Writes consecutive registers directly to the FIFO on the CC1120.
 * 
 * @param addr - The address of the first register to write to. Range 0x00 - 0xFF.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the status byte is invalid.
 */
obc_error_code_t cc1120_write_fifo_direct(uint8_t addr, uint8_t data[], uint8_t len);

/**
 * @brief - Reads the status register on the CC1120 and consecutively sends a byte over SPI.
 * 
 * @param data - The data to send to the status register.
 * @return OBC_ERR_CODE_SUCCESS - If the status byte is valid.
 * @return CC1120_ERROR_CODE_SEND_BYTE_RECEIVE_STATUS_INVALID_STATUS_BYTE - If the status byte is invalid.
 */
obc_error_code_t cc1120_send_byte_receive_status(uint8_t data);

#endif /* CC1120_SPI_H */
