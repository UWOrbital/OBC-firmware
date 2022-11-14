#include <stdlib.h>
#include "fram.h"
#include <FreeRTOS.h>

#include "spi.h"
#include "obc_spi_io.h"
#include <sys_common.h>

#define FRAM_spiREG     spiREG3
#define FRAM_spiPORT    spiPORT3
#define FRAM_CS         1

uint8_t framTransmitOpCode(int cmd, cmdType cmdType){
    if(cmdType == rCmd) {
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
            default:
                return 0;
        }
    }

    if(cmdType == wCmd) {
        switch (cmd) {
            case WREN:
                return spiTransmitByte(FRAM_spiREG, OP_WRITE_ENABLE);
                break;
            case WRDI:
                return spiTransmitByte(FRAM_spiREG, OP_WRITE_RESET);
                break;
            case WRSR:
                return spiTransmitByte(FRAM_spiREG, OP_READ_STAT_REG);
                break;
            case WRITE:
                return spiTransmitByte(FRAM_spiREG, OP_WRITE);
                break;
            default:
                return 0;
        }
    }

    if(cmdType == sCmd) {
        return spiTransmitByte(FRAM_spiREG, OP_SLEEP);
    }

    return 0;
}

uint8_t framTransmitAddress(void* addr) {
    uint8_t byte;
    for(int i=3; i >=0; i--){
        byte = ((uint32_t)addr >> (i*8)) & (0xFF);
        spiTransmitByte(FRAM_spiREG, (unsigned char) byte);
    }
    return 1;
}

uint8_t framRead(void* addr, uint8_t *buffer, size_t nBytes, readCmd cmd){
    if(cmd == RDID){
        if(nBytes < 9){
            return 0;
        }
    }
    assertChipSelect(FRAM_spiPORT, FRAM_CS);

    framTransmitOpCode(cmd, rCmd);
    
    if(cmd == READ || cmd == FSTRD){
        framTransmitAddress(addr);
    }

    if(cmd == FSTRD){
        //Transmit dummy byte
        spiTransmitByte(FRAM_spiREG, 0xFF);
    }
    
    if(cmd == RDID){
        nBytes = 9;
    }

    unsigned char receiveBuffer;
    for(int i=0; i<nBytes; i++){
        spiReceiveByte(FRAM_spiREG, &receiveBuffer);
        buffer[i] = receiveBuffer;
    }   
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return 1;
}

uint8_t framWrite(uint8_t *addr, uint8_t data, writeCmd cmd){
    return 1;
}

uint8_t framSleep(){
    assertChipSelect(FRAM_spiPORT, FRAM_CS);
    framTransmitOpCode(0, sCmd);
    deassertChipSelect(FRAM_spiPORT, FRAM_CS);
    return 1;
}