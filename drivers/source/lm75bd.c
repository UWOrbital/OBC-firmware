#include "lm75bd.h"
#include "obc_i2c_io.h"

#include <math.h>

uint8_t lm75bdInit(lm75bd_config_t *config)
{
    if (writeConfigLM75BD(config->devAddr, config->osFaltQueueSize, config->osPolarity, config->osOperationMode, config->devOperationMode) == 0) {
        return 0;
    }
    if (writeThystLM75BD(config->devAddr, config->hysteresisThresholdCelsius) == 0) {
        return 0;
    }
    if (writeTosLM75BD(config->devAddr, config->overTempThresholdCelsius) == 0) {
        return 0;
    }
    return 1;
}

uint8_t readTempLM75BD(uint8_t devAddr, float *temp) {
    uint8_t tempBuff[2];

    /* Get 2 bytes from the LM75BD temperature registers */
    if (i2cReadReg(devAddr, LM75BD_REG_TEMP, tempBuff, 2) == 0) {
        return 0;
    }

    /* Combine the 11 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 3 ) | ( tempBuff[1]  >> 5 );

    /* Convert to to degrees Celsius */
    *temp = (float)value * 0.125;

    return 1;
}

uint8_t readConfigLM75BD(uint8_t devAddr, uint8_t *osFaltQueueSize, uint8_t *osPolarity, uint8_t *osOperationMode, 
                         uint8_t *devOperationMode)
{
    uint8_t configBuff[1];
    if (i2cReadReg(devAddr, LM75BD_REG_CONF, configBuff, 1) == 0) {
        return 0;
    }

    uint8_t osFaltQueueRegData = (configBuff[0] & 0b11000) >> 3;
    switch (osFaltQueueRegData) {
        case 0:
            *osFaltQueueSize = 1;
            break;
        case 1:
            *osFaltQueueSize = 2;
            break;
        case 2:
            *osFaltQueueSize = 4;
            break;
        case 3:
            *osFaltQueueSize = 6;
            break;
        default:
            return 0;
    }

    *osPolarity = (configBuff[0] & 0b100) >> 2;
    *osOperationMode = (configBuff[0] & 0b010) >> 1;
    *devOperationMode = configBuff[0] & 0b001;

    return 1;
}

uint8_t writeConfigLM75BD(uint8_t devAddr, uint8_t osFaltQueueSize, uint8_t osPolarity, uint8_t osOperationMode, 
                          uint8_t devOperationMode)
{
    uint8_t configBuff[1] = {0};

    uint8_t osFaltQueueRegData;
    switch (osFaltQueueSize) {
        case 1:
            osFaltQueueRegData = 0;
            break;
        case 2:
            osFaltQueueRegData = 1;
            break;
        case 4:
            osFaltQueueRegData = 2;
            break;
        case 6:
            osFaltQueueRegData = 3;
            break;
        default:
            return 0;
    }

    configBuff[0] |= (osFaltQueueRegData << 3);
    configBuff[0] |= (osPolarity << 2);
    configBuff[0] |= (osOperationMode << 1);
    configBuff[0] |= devOperationMode;

    if (i2cWriteReg(devAddr, LM75BD_REG_CONF, configBuff, 1) == 0) {
        return 0;
    }

    return 1;
}

uint8_t readThystLM75BD(uint8_t devAddr, float *hysteresisThresholdCelsius) {
    uint8_t tempBuff[2];

    if (i2cReadReg(devAddr, LM75BD_REG_THYST, tempBuff, 2) == 0) {
        return 0;
    }

    /* Combine the 9 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 1 ) | ( tempBuff[1]  >> 7 );

    *hysteresisThresholdCelsius = (float)value * 0.5;

    return 1;
}

uint8_t writeThystLM75BD(uint8_t devAddr, float hysteresisThresholdCelsius) {
    uint8_t tempBuff[2];

    /* Threshold must be a multiple of 0.5 and less than 127.5 degrees Celsius */
    if (fmod(hysteresisThresholdCelsius, 0.5) != 0 || fabs(hysteresisThresholdCelsius) > 127.5) {
        return 0;
    }

    // TODO: Convert celsius to 2's complement reg data

    if (i2cWriteReg(devAddr, LM75BD_REG_THYST, tempBuff, 2) == 0) {
        return 0;
    }

    return 1;
}

uint8_t readTosLM75BD(uint8_t devAddr, float *overTempThresholdCelsius) {
    uint8_t tempBuff[2];

    if (i2cReadReg(devAddr, LM75BD_REG_TOS, tempBuff, 2) == 0) {
        return 0;
    }

    /* Combine the 9 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 1 ) | ( tempBuff[1]  >> 7 );

    *overTempThresholdCelsius = (float)value * 0.5;

    return 1;
}

uint8_t writeTosLM75BD(uint8_t devAddr, float overTempThresholdCelsius) {
    uint8_t tempBuff[2];

    /* Threshold must be a multiple of 0.5 and less than 127.5 degrees Celsius */
    if (fmod(overTempThresholdCelsius, 0.5) != 0 || fabs(overTempThresholdCelsius) > 127.5) {
        return 0;
    }

    // TODO: Convert celsius to 2's complement reg data

    if (i2cWriteReg(devAddr, LM75BD_REG_TOS, tempBuff, 2) == 0) {
        return 0;
    }

    return 1;
} 

void osHandlerLM75BD(uint8_t devAddr) {
    if (devAddr == LM75BD_OBC_I2C_ADDR) {
        /* Deal with OS interrupt */
        return;
    }
}