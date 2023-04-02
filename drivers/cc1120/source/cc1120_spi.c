#include "cc1120_spi.h"
#include "cc1120_defs.h"
#include "cc1120_mcu.h"
#include "obc_logging.h"

#define READ_BIT 1 << 7
#define BURST_BIT 1 << 6

#define CHIP_READY_MASK 1 << 7
#define CHIP_READY 0
#define CHIP_STATE 0b1110000

/**
 * @brief - Reads from consecutive registers from the CC1120.
 * 
 * @param addr - The address of the first register to read.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120ReadSpi(uint8_t addr, uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;
    
    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (addr >= CC1120_REGS_EXT_ADDR)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t header = (len > 1) ? (READ_BIT | BURST_BIT | addr) : (READ_BIT | addr);

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));
    

    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(0x00, &data[i]));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());
    
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Reads from consecutive extended address space registers on the CC1120
 * 
 * @param addr - The address of the first register to read.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120ReadExtAddrSpi(uint8_t addr, uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if ((addr >= CC1120_REGS_EXT_RESERVE_SPACE_1_START && addr <= CC1120_REGS_EXT_RESERVE_SPACE_1_END))
        return OBC_ERR_CODE_INVALID_ARG;
    
    if ((addr >= CC1120_REGS_EXT_RESERVE_SPACE_2_START && addr <= CC1120_REGS_EXT_RESERVE_SPACE_2_END))
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (addr > CC1120_REGS_EXT_FIFO_NUM_RXBYTES)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t header = (len > 1) ? (READ_BIT | BURST_BIT | CC1120_REGS_EXT_ADDR) :
                                    (READ_BIT | CC1120_REGS_EXT_ADDR);

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    uint8_t recvData;
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(addr, &recvData));
    if (recvData != 0x00) { // When sending the extended address, SO will return all zeros. See section 3.2.
        errCode = OBC_ERR_CODE_CC1120_READ_EXT_ADDR_SPI_FAILED;
        LOG_ERROR_CODE(errCode);
        return errCode;
    }

    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(0x00, &data[i]));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());
    
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Writes to consecutive registers on the CC1120.
 * 
 * @param addr - The address of the first register to write to.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120WriteSpi(uint8_t addr, uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if(addr >= CC1120_REGS_EXT_ADDR)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t header = (len > 1) ? (BURST_BIT | addr) : addr;

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(data[i]));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Writes to consecutive extended address space registers on the CC1120.
 * 
 * @param addr - The address of the first register to write to.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120WriteExtAddrSpi(uint8_t addr, uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if ((addr >= CC1120_REGS_EXT_RESERVE_SPACE_1_START && addr <= CC1120_REGS_EXT_RESERVE_SPACE_1_END))
        return OBC_ERR_CODE_INVALID_ARG;
    
    if ((addr >= CC1120_REGS_EXT_RESERVE_SPACE_2_START && addr <= CC1120_REGS_EXT_RESERVE_SPACE_2_END))
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (addr > CC1120_REGS_EXT_FIFO_NUM_RXBYTES)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1)
        return OBC_ERR_CODE_INVALID_ARG;
    
    uint8_t header = (len > 1) ? (BURST_BIT | CC1120_REGS_EXT_ADDR) : CC1120_REGS_EXT_ADDR;

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    uint8_t recvData;
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(addr, &recvData));
    if (recvData != 0x00) { // When sending the extended address, SO will return all zeros. See section 3.2.
        errCode = OBC_ERR_CODE_CC1120_WRITE_EXT_ADDR_SPI_FAILED;
        LOG_ERROR_CODE(errCode);
        return errCode;
    }
    
    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(data[i]));
    
    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief Calls a strobe command on the CC1120.
 * 
 * @param addr - The address of the strobe command.
 * @return OBC_ERR_CODE_SUCCESS - If the strobe command was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120StrobeSpi(uint8_t addr) {
    obc_error_code_t errCode;

    if (addr < CC1120_STROBE_SRES || addr > CC1120_STROBE_SNOP)
        return OBC_ERR_CODE_INVALID_ARG;

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());    
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(addr));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Reads consecutive registers from the RX FIFO memory.
 * 
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the errCode byte is invalid.
 */
obc_error_code_t cc1120ReadFifo(uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1 || len > CC1120_TX_FIFO_SIZE)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t header = (len > 1) ? (READ_BIT | BURST_BIT | CC1120_REGS_FIFO_ACCESS_STD) :
                                (READ_BIT | CC1120_REGS_FIFO_ACCESS_STD);

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(0x00, &data[i]));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Writes consecutive registers to the TX FIFO memory.
 * 
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the errCode byte is invalid.
 */
obc_error_code_t cc1120WriteFifo(uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1 || len > CC1120_TX_FIFO_SIZE)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t header = (len > 1) ? (BURST_BIT | CC1120_REGS_FIFO_ACCESS_STD) :
                                CC1120_REGS_FIFO_ACCESS_STD;

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(0x00, &data[i]));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Reads consecutive registers directly from the FIFO on the CC1120.
 * 
 * @param addr - The address of the first register to read. Range 0x00 - 0xFF.
 * @param data - The array to store the read data, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to read.
 * @return OBC_ERR_CODE_SUCCESS - If the read was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120ReadFifoDirect(uint8_t addr, uint8_t data[], uint8_t len) {
    obc_error_code_t errCode;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1)
        return OBC_ERR_CODE_INVALID_ARG;

    uint8_t header = (len > 1) ? (READ_BIT | BURST_BIT | CC1120_REGS_FIFO_ACCESS_DIR) :
                                (READ_BIT | CC1120_REGS_FIFO_ACCESS_DIR);

    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    uint8_t ignore;
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(addr, &ignore));
    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(0x00, &data[i]));

    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());

    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Writes consecutive registers directly to the FIFO on the CC1120.
 * 
 * @param addr - The address of the first register to write to. Range 0x00 - 0xFF.
 * @param data - The array of data to write to the registers, or a pointer to a single uint8_t if len=1.
 * @param len - The number of registers to write.
 * @return OBC_ERR_CODE_SUCCESS - If the write was successful.
 * @return An error code - If the register is not valid, or the errCode byte is invalid.
 */
obc_error_code_t cc1120WriteFifoDirect(uint8_t addr, uint8_t data[], uint8_t len) {
    obc_error_code_t errCode = OBC_ERR_CODE_SUCCESS;

    if (data == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    if (len < 1)
        return OBC_ERR_CODE_INVALID_ARG;
    
    uint8_t header = (len > 1) ? (BURST_BIT | CC1120_REGS_FIFO_ACCESS_DIR) :
                                CC1120_REGS_FIFO_ACCESS_DIR;


    RETURN_IF_ERROR_CODE(mcuCC1120CSAssert());
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(cc1120SendByteReceiveStatus(header));

    uint8_t ignore;
    CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(addr, &ignore));
    for(uint8_t i = 0; i < len; i++)
        CC1120_DEASSERT_RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(0x00, &data[i]));
    
    RETURN_IF_ERROR_CODE(mcuCC1120CSDeassert());
    return OBC_ERR_CODE_SUCCESS;
}

/**
 * @brief - Reads the status register on the CC1120 and consecutively sends a byte over SPI.
 * 
 * @param data - The data to send to the status register.
 * @return OBC_ERR_CODE_SUCCESS - If the status byte is valid.
 * @return OBC_ERR_CODE_CC1120_INVALID_STATUS_BYTE - If the status byte is invalid.
 */
obc_error_code_t cc1120SendByteReceiveStatus(uint8_t data) {
    obc_error_code_t errCode;
    uint8_t ccStatus;
    
    // TODO: This is a hacky way to do this. We should implement a mutex + timeout.
    for (uint8_t i = 1; i <= 5; i++) {
        RETURN_IF_ERROR_CODE(mcuCC1120SpiTransfer(data, &ccStatus));
        if ((ccStatus & CHIP_READY_MASK) == CHIP_READY) {
            return OBC_ERR_CODE_SUCCESS;
        }
    }

    return OBC_ERR_CODE_CC1120_INVALID_STATUS_BYTE;
}
