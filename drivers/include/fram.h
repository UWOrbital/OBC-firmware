#ifndef DRIVERS_INCLUDE_FRAM_H_
#define DRIVERS_INCLUDE_FRAM_H_

//FRAM LIMITS
#define FRAM_MAX_ADDRESS        0x3FFFFU
#define FRAM_ID_LEN             9

#include <stdlib.h>
#include <stdint.h>
#include <obc_errors.h>

obc_error_code_t framReadStatusReg(uint8_t *status);
obc_error_code_t framWriteStatusReg(uint8_t status);

obc_error_code_t framFastRead(uint32_t addr, uint8_t *buffer, size_t nBytes);
obc_error_code_t framRead(uint32_t addr, uint8_t *buffer, size_t nBytes);
obc_error_code_t framWrite(uint32_t addr, uint8_t *data, size_t nBytes);

obc_error_code_t framSleep(void);
obc_error_code_t framWakeUp(void);

obc_error_code_t framReadID(uint8_t *ID, size_t nBytes);
#endif