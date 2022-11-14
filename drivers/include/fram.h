#ifndef DRIVERS_INCLUDE_FRAM_H_
#define DRIVERS_INCLUDE_FRAM_H_

#include <stdlib.h>
#include <stdint.h>

//FRAM OPCODES
#define OP_WRITE_ENABLE        0x06
#define OP_WRITE_RESET         0x04
#define OP_READ_STAT_REG       0x05
#define OP_WRITE_STAT_REG      0x01

#define OP_READ                0x03
#define OP_FREAD               0x0B
#define OP_WRITE               0x02

#define OP_SLEEP               0xB9
#define OP_GET_ID              0x9F

typedef enum cmdType{
    rCmd,
    wCmd,
    sCmd
}cmdType;

typedef enum readCmd{
    RDSR,           //Read Status Register
    READ,           //Normal read
    FSTRD,          //Fast read
    RDID            //Get Device ID
}readCmd;

typedef enum writeCmd{
    WREN,           //Set Write EN
    WRDI,           //Reset Write EN
    WRSR,           //Write to Status Register
    WRITE           //Write memory data
}writeCmd;

uint8_t framTransmitOpCode(int cmd, cmdType cmdType);
uint8_t framTransmitAddress(void* addr);
uint8_t framRead(void* addr, uint8_t *buffer, size_t nBytes, readCmd cmd);
uint8_t framWrite(uint8_t *addr, uint8_t data, writeCmd cmd);
uint8_t framSleep();
#endif