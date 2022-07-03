/**
 * @author  Daniel Gobalakrishnan
 * @date    2022-07-02
 * @file    lm75bd.c
*/
#include "lm75bd.h"
#include "obc_i2c_io.h"

uint8_t lm75bd_read_temp(float *temp) {
    uint8_t tempBuff[2];

    if (i2c_read_register(LM75BD_I2C_ADDR, LM75BD_REG_TEMP, tempBuff, 2) == 0) {
        return 0;
    }

    /* Convert and store the temperature in degrees Celsius */
    uint8_t tempMSB = (tempBuff[0] >> 7) & 1u;

    *temp = (int16_t)( ( ( (tempBuff[0] & 0x7Fu) << 8 ) | tempBuff[1] ) >> 5 ) * 0.125f;

    return 1;
}
