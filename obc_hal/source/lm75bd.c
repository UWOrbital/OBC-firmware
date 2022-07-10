/**
 * @author  Daniel Gobalakrishnan
 * @date    2022-07-02
 * @file    lm75bd.c
*/
#include "lm75bd.h"
#include "obc_i2c_io.h"

uint8_t readTempLM75BD(uint8_t devAddr, float *temp) {
    uint8_t tempBuff[2];

    if (i2c_read_register(devAddr, LM75BD_REG_TEMP, tempBuff, 2) == 0) {
        return 0;
    }

    int16_t value = ( (tempBuff[0] << 8) | tempBuff[1] ) >> 5;

    /* Sign extension */
    if (value & (1 << 10)) {
        value |= ~((1 << 10) - 1);
    }
    /*
    value = 1111 1111 1110 0000b -> 111 1111 1111b
    (1 << 10) - 1 = 100 0000 0000b - 1 = 011 1111 1111b
    ~(011 1111 1111) = 1111 1100 0000 0000b -> -1024 in signed 2's complement
    111 1111 1111b | 1111 1100 0000 0000b = 1111 1111 1111 1111 -> -1 in signed 2's complement
    */

    *temp = (float)value * 0.125;

    return 1;
}

uint8_t writeConfigLM75BD(uint8_t devAddr, lm75bd_config_t *config) {
    
}
