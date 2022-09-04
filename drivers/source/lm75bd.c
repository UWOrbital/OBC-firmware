#include "lm75bd.h"
#include "obc_i2c_io.h"

#include <math.h>

uint8_t lm75bdInit(lm75bd_config_t *config)
{
    if (config == NULL) {
        return 0;
    }
    /* TOS must be greater than THYST */
    if (config->hysteresisThresholdCelsius >= config->overTempThresholdCelsius) {
        return 0;
    }
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

    if (temp == NULL) {
        return 0;
    }

    if (i2cReadReg(devAddr, LM75BD_REG_TEMP, tempBuff, 2) == 0) {
        return 0;
    }

    /* Combine the 11 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 3 ) | ( tempBuff[1]  >> 5 );

    /* Convert to degrees Celsius */
    *temp = (float)value * LM75BD_TEMP_RES;

    return 1;
}

uint8_t readConfigLM75BD(lm75bd_config_t *config)
{
    uint8_t configBuff[1];

    if (config == NULL) {
        return 0;
    }

    if (i2cReadReg(config->devAddr, LM75BD_REG_CONF, configBuff, 1) == 0) {
        return 0;
    }

    /* Bit 4 and 5 are OS Fault Queue Data */
    uint8_t osFaltQueueRegData = (configBuff[0] & 0b11000) >> 3;
    switch (osFaltQueueRegData) {
        case 0:
            config->osFaltQueueSize = 1;
            break;
        case 1:
            config->osFaltQueueSize = 2;
            break;
        case 2:
            config->osFaltQueueSize = 4;
            break;
        case 3:
            config->osFaltQueueSize = 6;
            break;
        default:
            return 0;
    }

    config->osPolarity = (configBuff[0] & 0b100) >> 2;
    config->osOperationMode = (configBuff[0] & 0b010) >> 1;
    config->devOperationMode = configBuff[0] & 0b001;

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

    if (hysteresisThresholdCelsius == NULL) {
        return 0;
    }

    if (i2cReadReg(devAddr, LM75BD_REG_THYST, tempBuff, 2) == 0) {
        return 0;
    }

    /* Combine the 9 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 1 ) | ( tempBuff[1]  >> 7 );

    /* Convert to degrees Celsius */
    *hysteresisThresholdCelsius = (float)value * LM75BD_THYST_RES;

    return 1;
}

uint8_t writeThystLM75BD(uint8_t devAddr, float hysteresisThresholdCelsius) {
    uint8_t tempBuff[2];

    /* Threshold must be a multiple of the resolution and less than 127.5 degrees Celsius */
    if (fmod(hysteresisThresholdCelsius, LM75BD_THYST_RES) != 0 || fabs(hysteresisThresholdCelsius) > 127.5) {
        return 0;
    }

    // Convert celsius to 2's complement reg data
    int16_t converted = (int16_t)(hysteresisThresholdCelsius / LM75BD_THYST_RES) << 7;
    tempBuff[0] = converted >> 8;
    tempBuff[1] = converted & 0xFF;


    if (i2cWriteReg(devAddr, LM75BD_REG_THYST, tempBuff, 2) == 0) {
        return 0;
    }

    return 1;
}

uint8_t readTosLM75BD(uint8_t devAddr, float *overTempThresholdCelsius) {
    uint8_t tempBuff[2];

    if (overTempThresholdCelsius == NULL) {
        return 0;
    }

    if (i2cReadReg(devAddr, LM75BD_REG_TOS, tempBuff, 2) == 0) {
        return 0;
    }

    /* Combine the 9 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 1 ) | ( tempBuff[1]  >> 7 );

    /* Convert to degrees Celsius */
    *overTempThresholdCelsius = (float)value * LM75BD_TOS_RES;

    return 1;
}

uint8_t writeTosLM75BD(uint8_t devAddr, float overTempThresholdCelsius) {
    uint8_t tempBuff[2];

    /* Threshold must be a multiple of the resolution and less than 127.5 degrees Celsius */
    if (fmod(overTempThresholdCelsius, LM75BD_TOS_RES) != 0 || fabs(overTempThresholdCelsius) > 127.5) {
        return 0;
    }

    // Convert celsius to 2's complement reg data
    int16_t converted = (int16_t)(overTempThresholdCelsius / LM75BD_TOS_RES) << 7;
    tempBuff[0] = converted >> 8;
    tempBuff[1] = converted & 0xFF;


    if (i2cWriteReg(devAddr, LM75BD_REG_TOS, tempBuff, 2) == 0) {
        return 0;
    }

    return 1;
} 

void osHandlerLM75BD(uint8_t devAddr) {
    if (devAddr == LM75BD_OBC_I2C_ADDR) {
        /* 
            Deal with OS interrupt.
            In OS comparator mode, the OS output will be deactivated automatically if T < Thys.
            In OS interrupt mode, the OS output will be deactivated only if we read from a register.
            Note that we only have to deal with the OS interrupt in OS interrupt mode. 
         */
        // TODO: Implement OS interrupt handler 

        return;
    }
}