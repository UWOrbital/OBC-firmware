#include <stdlib.h>
#include "fram.h"
#include <FreeRTOS.h>

#include "spi.h"
#include "obc_spi_io.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_board_config.h"
#include <sys_common.h>

//SPI values
static spiDAT1_t framSPIDataFmt = {.CS_HOLD = 0,
                                    .CSNR = SPI_CS_NONE, 
                                    .DFSEL = FRAM_spiFMT, 
                                    .WDEL = 0};

//FRAM OPCODES
#define OP_WRITE_ENABLE         0x06U
#define OP_WRITE_RESET          0x04U
#define OP_READ_STAT_REG        0x05U
#define OP_WRITE_STAT_REG       0x01U

#define OP_READ                 0x03U
#define OP_FREAD                0x0BU
#define OP_WRITE                0x02U

#define OP_SLEEP                0xB9U
#define OP_GET_ID               0x9FU


#define FRAM_WAKE_BUSY_WAIT     99000U      //Assume RM46 clk is 220 MHz, value for wait loop should give ~450us delay

typedef enum cmd{
    FRAM_READ_STATUS_REG,               //Read Status Register
    FRAM_READ,                          //Normal read
    FRAM_FAST_READ,                     //Fast read, Note this is used for serial flash compatibility not to read data fast!
    FRAM_READ_ID,                       //Get Device ID
    FRAM_WRITE_EN,                      //Set Write EN
    FRAM_WRITE_EN_RESET,                //Reset Write EN
    FRAM_WRITE_STATUS_REG,              //Write to Status Register
    FRAM_WRITE,                         //Write memory data
    FRAM_SLEEP                          //Put FRAM to sleep
}cmd_t;


//Function Declarations
static obc_error_code_t framTransmitOpCode(cmd_t cmd);
static obc_error_code_t framTransmitAddress(uint32_t addr);

//CS assumed to be asserted
static obc_error_code_t framTransmitOpCode(cmd_t cmd){
    switch (cmd) {
        case FRAM_READ:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_READ);
        case FRAM_FAST_READ:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_FREAD);
        case FRAM_READ_STATUS_REG:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_READ_STAT_REG);
        case FRAM_READ_ID:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_GET_ID);
        case FRAM_WRITE_EN:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_WRITE_ENABLE);
        case FRAM_WRITE_EN_RESET:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_WRITE_RESET);
        case FRAM_WRITE_STATUS_REG:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_WRITE_STAT_REG);
        case FRAM_WRITE:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_WRITE);
        case FRAM_SLEEP:
            return spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, OP_SLEEP);
    }

    return OBC_ERR_CODE_INVALID_ARG;
}

//CS assumed to be asserted
static obc_error_code_t framTransmitAddress(uint32_t addr) {
    if(addr > FRAM_MAX_ADDRESS){
        return OBC_ERR_CODE_FRAM_ADDRESS_OUT_OF_RANGE;
    }
    
    //Send last 3 bytes MSB first
    for(int i=2; i >=0; i--){
        uint8_t byte = (addr >> (i*8)) & (0xFF);
        obc_error_code_t ret = spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, (unsigned char) byte);
        if(ret != OBC_ERR_CODE_SUCCESS){
            return ret;
        }
    }
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framReadStatusReg(uint8_t *status){
    obc_error_code_t errCode;
    if(status == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));

    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_READ_STATUS_REG));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiReceiveByte(FRAM_spiREG, &framSPIDataFmt, status));

    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framWriteStatusReg(uint8_t status){
    obc_error_code_t errCode;
    //Send WREN
    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_WRITE_EN));
    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    
    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_WRITE_STATUS_REG));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, status));
    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT,  FRAM_CS));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framFastRead(uint32_t addr, uint8_t *buffer, size_t nBytes){
    obc_error_code_t errCode;
    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));

    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_FAST_READ));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitAddress(addr));

    //Send dummy byte
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, 0xFF));

    for(uint32_t i=0; i<nBytes; i++){
        uint8_t receiveByte;
        DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiReceiveByte(FRAM_spiREG, &framSPIDataFmt, &receiveByte));
        buffer[i] = receiveByte;
    }

    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framRead(uint32_t addr, uint8_t *buffer, size_t nBytes){
    obc_error_code_t errCode;
    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));

    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_READ));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitAddress(addr));

    for(uint32_t i=0; i<nBytes; i++){
        uint8_t receiveByte;
        DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiReceiveByte(FRAM_spiREG, &framSPIDataFmt, &receiveByte));
        buffer[i] = receiveByte;
    }

    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framWrite(uint32_t addr, uint8_t *data, size_t nBytes){
    obc_error_code_t errCode;
    if(data == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    //Send WREN
    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_WRITE_EN));
    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));

    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));

    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_WRITE));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitAddress(addr));

    for(uint32_t i=0; i<nBytes; i++){
        DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiTransmitByte(FRAM_spiREG, &framSPIDataFmt, data[i]));
    }

    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framSleep(void){
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_SLEEP));
    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framWakeUp(void){
    obc_error_code_t errCode;
    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));
    for(uint32_t i = 0; i < FRAM_WAKE_BUSY_WAIT; i++) {
        //Do Nothing
    }
    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framReadID(uint8_t *id, size_t nBytes){
    obc_error_code_t errCode;
    if(id == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }
    RETURN_IF_ERROR_CODE(assertChipSelect(FRAM_spiPORT, FRAM_CS));
    DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, framTransmitOpCode(FRAM_READ_ID));

    for(uint32_t i=0; i<nBytes && i < FRAM_ID_LEN; i++){
        uint8_t receiveByte;
        DEASSERT_RETURN_IF_ERROR_CODE(FRAM_spiPORT, FRAM_CS, spiReceiveByte(FRAM_spiREG, &framSPIDataFmt, &receiveByte));
        id[i] = receiveByte;
    }

    RETURN_IF_ERROR_CODE(deassertChipSelect(FRAM_spiPORT, FRAM_CS));
    return OBC_ERR_CODE_SUCCESS;
}