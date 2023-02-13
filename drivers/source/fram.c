#include <stdlib.h>
#include "fram.h"
#include <FreeRTOS.h>

#include "spi.h"
#include "obc_spi_io.h"
#include <sys_common.h>

//SPI Values
#define FRAM_spiREG     spiREG3
#define FRAM_spiPORT    spiPORT3
#define FRAM_CS         1

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


#define FRAM_WAKE_BUSY_WAIT     99000U      //Assume RM46 is 220 MHz, value for wait loop should give ~450us delay

typedef enum cmd{
    RDSR,           //Read Status Register
    READ,           //Normal read
    FSTRD,          //Fast read, Note this is used for serial flash compatibility not to read data fast!
    RDID,           //Get Device ID
    WREN,           //Set Write EN
    WRDI,           //Reset Write EN
    WRSR,           //Write to Status Register
    WRITE,          //Write memory data
    SLEEP           //Put FRAM to sleep
}cmd_t;


//Function Declarations
static obc_error_code_t framTransmitOpCode(cmd_t cmd);
static obc_error_code_t framTransmitAddress(uint32_t addr);

static obc_error_code_t framTransmitOpCode(cmd_t cmd){
    switch (cmd) {
        case READ:
            return spiTransmitByte(FRAM_spiREG, OP_READ);
            break;
        case FSTRD:
            return spiTransmitByte(FRAM_spiREG, OP_FREAD);
            break;
        case RDSR:
            return spiTransmitByte(FRAM_spiREG, OP_READ_STAT_REG);
            break;
        case RDID:
            return spiTransmitByte(FRAM_spiREG, OP_GET_ID);
            break;
        case WREN:
            return spiTransmitByte(FRAM_spiREG, OP_WRITE_ENABLE);
            break;
        case WRDI:
            return spiTransmitByte(FRAM_spiREG, OP_WRITE_RESET);
            break;
        case WRSR:
            return spiTransmitByte(FRAM_spiREG, OP_WRITE_STAT_REG);
            break;
        case WRITE:
            return spiTransmitByte(FRAM_spiREG, OP_WRITE);
            break;
        case SLEEP:
            return spiTransmitByte(FRAM_spiREG, OP_SLEEP);
            break;
        default:
            return OBC_ERR_CODE_INVALID_ARG;
    }

    return OBC_ERR_CODE_INVALID_ARG;
}

static obc_error_code_t framTransmitAddress(uint32_t addr) {
    if(addr > FRAM_MAX_ADDRESS){
        return OBC_ERR_CODE_FRAM_ADDRESS_OUT_OF_RANGE;
    }
    
    for(int i=2; i >=0; i--){
        uint8_t byte = (addr >> (i*8)) & (0xFF);
        obc_error_code_t ret = spiTransmitByte(FRAM_spiREG, (unsigned char) byte);
        if(ret != OBC_ERR_CODE_SUCCESS){
            return ret;
        }
    }
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framReadStatusReg(uint8_t *status){
    obc_error_code_t ret;
    if(status == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    ret = framTransmitOpCode(RDSR);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    ret = spiReceiveByte(FRAM_spiREG, status);
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);

    return ret;
}

obc_error_code_t framWriteStatusReg(uint8_t status){
    obc_error_code_t ret;
    //Send WREN
    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    ret = framTransmitOpCode(WREN);
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    if(ret != OBC_ERR_CODE_SUCCESS){
        return ret;
    }

    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    ret = framTransmitOpCode(WRSR);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    ret = spiTransmitByte(FRAM_spiREG, status);
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);

    return ret;
}

obc_error_code_t framFastRead(uint32_t addr, uint8_t *buffer, size_t nBytes){
    obc_error_code_t ret;
    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    assertChipSelect(FRAM_spiPORT, FRAM_CS);

    ret = framTransmitOpCode(FSTRD);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    ret = framTransmitAddress(addr);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    //Send dummy byte
    ret = spiTransmitByte(FRAM_spiREG, 0xFF);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    for(int i=0; i<nBytes; i++){
        uint8_t receiveByte;
        ret = spiReceiveByte(FRAM_spiREG, &receiveByte);
        if(ret != OBC_ERR_CODE_SUCCESS){
            deassertChipSelect(FRAM_spiPORT, FRAM_CS);
            return ret;
        }
        buffer[i] = receiveByte;
    }

    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framRead(uint32_t addr, uint8_t *buffer, size_t nBytes){
    obc_error_code_t ret;
    if(buffer == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    assertChipSelect(FRAM_spiPORT, FRAM_CS);

    ret = framTransmitOpCode(READ);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    ret = framTransmitAddress(addr);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    for(int i=0; i<nBytes; i++){
        uint8_t receiveByte;
        ret = spiReceiveByte(FRAM_spiREG, &receiveByte);
        if(ret != OBC_ERR_CODE_SUCCESS){
            deassertChipSelect(FRAM_spiPORT, FRAM_CS);
            return ret;
        }
        buffer[i] = receiveByte;
    }

    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framWrite(uint32_t addr, uint8_t *data, size_t nBytes){
    obc_error_code_t ret;
    if(data == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }

    //Send WREN
    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    ret = framTransmitOpCode(WREN);
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    if(ret != OBC_ERR_CODE_SUCCESS){
        return ret;
    }

    assertChipSelect(FRAM_spiPORT, FRAM_CS);

    ret = framTransmitOpCode(WRITE);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    ret = framTransmitAddress(addr);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    for(int i=0; i<nBytes; i++){
        ret = spiTransmitByte(FRAM_spiREG, data[i]);
        if(ret != OBC_ERR_CODE_SUCCESS){
            deassertChipSelect(FRAM_spiPORT, FRAM_CS);
            return ret;
        }
    }

    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t framSleep(){
    obc_error_code_t ret;
    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    ret = framTransmitOpCode(SLEEP);
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return ret;
}

obc_error_code_t framWakeUp(void){
    obc_error_code_t ret;
    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    for(int i=0; i<FRAM_WAKE_BUSY_WAIT; i++){
        //Do Nothing
    }
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return ret;
}

obc_error_code_t framReadID(uint8_t *ID, size_t nBytes){
    obc_error_code_t ret;
    if(ID == NULL){
        return OBC_ERR_CODE_INVALID_ARG;
    }
    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    ret = framTransmitOpCode(RDID);
    if(ret != OBC_ERR_CODE_SUCCESS){
        deassertChipSelect(FRAM_spiPORT, FRAM_CS);
        return ret;
    }

    for(int i=0; i<nBytes && i < FRAM_ID_LEN; i++){
        uint8_t receiveByte;
        ret = spiReceiveByte(FRAM_spiREG, &receiveByte);
        if(ret != OBC_ERR_CODE_SUCCESS){
            deassertChipSelect(FRAM_spiPORT, FRAM_CS);
            return ret;
        }
        ID[i] = receiveByte;
    }

    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return ret;
}