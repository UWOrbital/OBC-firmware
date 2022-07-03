/**
 * @author  Daniel Gobalakrishnan
 * @date    2022-07-02
 * @file    lm75bd.h
*/
#ifndef OBC_HAL_LM75BD_H_
#define OBC_HAL_LM75BD_H_

#include "stdint.h"

/* LM75BD Device Address Options */
#define LM75BD_I2C_ADDR    0x4F

/* LM75BD Registers */
#define LM75BD_REG_TEMP    0x00
#define LM75BD_REG_CONF    0x01
#define LM75BD_REG_THYST   0x02
#define LM75BD_REG_TOS     0x03

typedef struct {
    float overTempThreshold;
    float hysteresisThreshold;
} lm75bd_config_t;

uint8_t lm75bd_read_temp(float *temp);

#endif /* OBC_HAL_LM75BD_H_ */