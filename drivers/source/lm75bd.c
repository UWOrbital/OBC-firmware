#include "lm75bd.h"
#include "obc_i2c_io.h"
#include "logging.h"
#include "obc_errors.h"

#include <stdint.h>
#include <math.h>

obc_error_code_t lm75bdInit(lm75bd_config_t *config) {
    if (config == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    /* TOS must be greater than THYST */
    if (config->hysteresisThresholdCelsius >= config->overTempThresholdCelsius)
        return OBC_ERR_CODE_INVALID_ARG;
    
    obc_error_code_t err;

    err = writeConfigLM75BD(config->devAddr, config->osFaultQueueSize, config->osPolarity, config->osOperationMode, config->devOperationMode);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to write config to LM75BD - Addr 0x%02X", config->devAddr);
        return err;
    }

    err = writeThystLM75BD(config->devAddr, config->hysteresisThresholdCelsius);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to write hysteresis threshold to LM75BD - Addr 0x%02X", config->devAddr);
        return err;
    }
    
    err = writeTosLM75BD(config->devAddr, config->overTempThresholdCelsius);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to write over temperature threshold to LM75BD - Addr 0x%02X", config->devAddr);
        return err;
    }
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readTempLM75BD(uint8_t devAddr, float *temp) {
    uint8_t tempBuff[LM75BD_TEMP_BUFF_SIZE] = {0};

    if (temp == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    obc_error_code_t err = i2cReadReg(devAddr, LM75BD_REG_TEMP, tempBuff, LM75BD_TEMP_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to read temperature from LM75BD - Addr 0x%02X", devAddr);
        return err;
    }

    /* Combine the 11 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tempBuff[0] << 3 ) | ( tempBuff[1]  >> 5 );

    /* Convert to degrees Celsius */
    *temp = (float)value * LM75BD_TEMP_RES;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readConfigLM75BD(lm75bd_config_t *config) {
    const uint8_t faultQueueMapping[] = {1, 2, 4, 6}; // Maps the fault queue bits to the falt queue size

    uint8_t configBuff[LM75BD_CONF_BUFF_SIZE] = {0};

    if (config == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    obc_error_code_t err = i2cReadReg(config->devAddr, LM75BD_REG_CONF, configBuff, LM75BD_CONF_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to read config from LM75BD - Addr 0x%02X", config->devAddr);
        return err;
    }

    uint8_t osFaltQueueRegData = (configBuff[0] & LM75BD_OS_FAULT_QUEUE_MASK) >> 3;
    if (osFaltQueueRegData > 3)
        return OBC_ERR_CODE_I2C_FAILURE;
    
    config->osFaultQueueSize = faultQueueMapping[osFaltQueueRegData];
    config->osPolarity = (configBuff[0] & LM75BD_OS_POL_MASK) >> 2;
    config->osOperationMode = (configBuff[0] & LM75BD_OS_OP_MODE_MASK) >> 1;
    config->devOperationMode = configBuff[0] & LM75BD_DEV_OP_MODE_MASK;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeConfigLM75BD(uint8_t devAddr, uint8_t osFaultQueueSize, uint8_t osPolarity, uint8_t osOperationMode, 
                                  uint8_t devOperationMode)
{
    uint8_t configBuff[LM75BD_CONF_BUFF_SIZE] = {0};

    uint8_t osFaltQueueRegData;
    switch (osFaultQueueSize) {
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

    obc_error_code_t err = i2cWriteReg(devAddr, LM75BD_REG_CONF, configBuff, LM75BD_CONF_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to write config to LM75BD - Addr 0x%02X", devAddr);
        return err;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readThystLM75BD(uint8_t devAddr, float *hysteresisThresholdCelsius) {
    uint8_t thystBuff[LM75BD_THYST_BUFF_SIZE] = {0};

    if (hysteresisThresholdCelsius == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    obc_error_code_t err = i2cReadReg(devAddr, LM75BD_REG_THYST, thystBuff, LM75BD_THYST_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to read hysteresis threshold from LM75BD - Addr 0x%02X", devAddr);
        return err;
    }

    /* Combine the 9 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)thystBuff[0] << 1 ) | ( thystBuff[1]  >> 7 );

    /* Convert to degrees Celsius */
    *hysteresisThresholdCelsius = (float)value * LM75BD_THYST_RES;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeThystLM75BD(uint8_t devAddr, float hysteresisThresholdCelsius) {
    uint8_t thystBuff[LM75BD_THYST_BUFF_SIZE] = {0};

    /* Threshold must be a multiple of the resolution and less than 127.5 degrees Celsius */
    if (fmod(hysteresisThresholdCelsius, LM75BD_THYST_RES) != 0 || fabs(hysteresisThresholdCelsius) > LM75BD_TEMP_THRESH_MAX)
        return OBC_ERR_CODE_INVALID_ARG;


    // Convert celsius to 2's complement reg data
    int16_t converted = (int16_t)(hysteresisThresholdCelsius / LM75BD_THYST_RES) << 7;
    thystBuff[0] = converted >> 8;
    thystBuff[1] = converted & 0xFF;

    obc_error_code_t err = i2cWriteReg(devAddr, LM75BD_REG_THYST, thystBuff, LM75BD_THYST_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to write hysteresis threshold to LM75BD - Addr 0x%02X", devAddr);
        return err;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readTosLM75BD(uint8_t devAddr, float *overTempThresholdCelsius) {
    uint8_t tosBuff[LM75BD_TOS_BUFF_SIZE] = {0};

    if (overTempThresholdCelsius == NULL)
        return OBC_ERR_CODE_INVALID_ARG;

    obc_error_code_t err = i2cReadReg(devAddr, LM75BD_REG_TOS, tosBuff, LM75BD_TOS_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to read over temp threshold from LM75BD - Addr 0x%02X", devAddr);
        return err;
    }

    /* Combine the 9 MSB into a 16-bit signed integer */
    int16_t value = ( (int8_t)tosBuff[0] << 1 ) | ( tosBuff[1]  >> 7 );

    /* Convert to degrees Celsius */
    *overTempThresholdCelsius = (float)value * LM75BD_TOS_RES;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeTosLM75BD(uint8_t devAddr, float overTempThresholdCelsius) {
    uint8_t tosBuff[LM75BD_TOS_BUFF_SIZE] = {0};

    /* Threshold must be a multiple of the resolution and less than 127.5 degrees Celsius */
    if (fmod(overTempThresholdCelsius, LM75BD_TOS_RES) != 0 || fabs(overTempThresholdCelsius) > LM75BD_TEMP_THRESH_MAX)
        return OBC_ERR_CODE_INVALID_ARG;

    // Convert celsius to 2's complement reg data
    int16_t converted = (int16_t)(overTempThresholdCelsius / LM75BD_TOS_RES) << 7;
    tosBuff[0] = converted >> 8;
    tosBuff[1] = converted & 0xFF;

    obc_error_code_t err = i2cWriteReg(devAddr, LM75BD_REG_TOS, tosBuff, LM75BD_TOS_BUFF_SIZE);
    if (err != OBC_ERR_CODE_SUCCESS) {
        LOG_ERROR("Failed to write over temp threshold to LM75BD - Addr 0x%02X", devAddr);
        return err;
    }

    return OBC_ERR_CODE_SUCCESS;
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

    }
}