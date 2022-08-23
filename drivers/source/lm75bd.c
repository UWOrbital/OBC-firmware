#include "lm75bd.h"
#include "obc_i2c_io.h"

#include <math.h>

uint8_t lm75bdInit(uint8_t devAddr, uint8_t osFaltQueueSize, uint8_t osPolarity, uint8_t osOperationMode, 
                   uint8_t devOperationMode, float overTempThresholdCelsius, float hysteresisThresholdCelsius)
{
    if (writeConfigLM75BD(devAddr, osFaltQueueSize, osPolarity, osOperationMode, devOperationMode) == 0) {
        return 0;
    }
    if (writeThystLM75BD(devAddr, hysteresisThresholdCelsius) == 0) {
        return 0;
    }
    if (writeTosLM75BD(devAddr, overTempThresholdCelsius) == 0) {
        return 0;
    }
    return 1;
}

uint8_t readTempLM75BD(uint8_t devAddr, float *temp) {
    uint8_t tempBuff[2];

    if (i2cReadReg(devAddr, LM75BD_REG_TEMP, tempBuff, 2) == 0) {
        return 0;
    }

    int16_t value = ( (tempBuff[0] << 8) | tempBuff[1] ) >> 5;

    if (value & (1 << 10)) {
        value |= ~((1 << 10) - 1);
    }

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

    uint8_t osFaltQueueRegData = (configBuff[0] & 0b00011000) >> 3;
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

    *osPolarity = (configBuff[0] & 0b00000100) >> 2;
    *osOperationMode = (configBuff[0] & 0b00000010) >> 1;
    *devOperationMode = configBuff[0] & 0b00000001;

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

    int16_t value = ( (tempBuff[0] << 8) | tempBuff[1] ) >> 7;

    if (value & (1 << 8)) {
        value |= ~((1 << 8) - 1);
    }

    *hysteresisThresholdCelsius = (float)value * 0.5;

    return 1;
}

uint8_t writeThystLM75BD(uint8_t devAddr, float hysteresisThresholdCelsius) {
    uint8_t tempBuff[2];

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

    int16_t value = ( (tempBuff[0] << 8) | tempBuff[1] ) >> 7;

    if (value & (1 << 8)) {
        value |= ~((1 << 8) - 1);
    }

    *overTempThresholdCelsius = (float)value * 0.5;

    return 1;
}

uint8_t writeTosLM75BD(uint8_t devAddr, float overTempThresholdCelsius) {
    uint8_t tempBuff[2];

    if (fmod(overTempThresholdCelsius, 0.5) != 0 || fabs(overTempThresholdCelsius) > 127.5) {
        return 0;
    }

    // TODO: Convert celsius to 2's complement reg data

    if (i2cWriteReg(devAddr, LM75BD_REG_TOS, tempBuff, 2) == 0) {
        return 0;
    }

    return 1;
} 