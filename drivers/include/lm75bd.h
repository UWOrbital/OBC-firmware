#ifndef DRIVERS_INCLUDE_LM75BD_H_
#define DRIVERS_INCLUDE_LM75BD_H_

#include <stdint.h>

/* LM75BD I2C Device Addresses */
#define LM75BD_BASE_I2C_ADDR 0x9
#define LM75BD_OBC_I2C_ADDR 0x4F /* (LM75BD_BASE_I2C_ADDR << 3) | 0x7 */

/* LM75BD Registers (p.8) */
#define LM75BD_REG_TEMP 0x00  /* Temperature Register (R) */
#define LM75BD_REG_CONF 0x01  /* Configuration Register (R/W) */
#define LM75BD_REG_THYST 0x02 /* Hysteresis Register (R/W) */
#define LM75BD_REG_TOS 0x03   /* Overtemperature Shutdown Register (R/W) */

/* LM75BD Configuration Values */
#define LM75BD_DEV_OP_MODE_NORMAL 0x00
#define LM75BD_DEV_OP_MODE_SHUTDOWN 0x01
#define LM75BD_OS_POL_ACTIVE_LOW 0x00
#define LM75BD_OS_POL_ACTIVE_HIGH 0x01
#define LM75BD_OS_OP_MODE_COMP 0x00
#define LM75BD_OS_OP_MODE_INT 0x01

/* LM75BD Default State (p.12) */
#define LM75BD_DEFAULT_DEV_OP_MODE LM75BD_DEV_OP_MODE_NORMAL
#define LM75BD_DEFAULT_OS_OP_MODE LM75BD_OS_OP_MODE_COMP
#define LM75BD_DEFAULT_TEMP_TOS 80.0 /* Degrees Celsius */
#define LM75BD_DEFAULT_TEMP_HYS 75.0 /* Degrees Celsius */
#define LM75BD_DEFAULT_OS_POL LM75BD_OS_POL_ACTIVE_LOW
#define LM75BD_DEFAULT_OS_FAULT_QUEUE_SIZE 1 /* OS_F_QUE = 0 */

/* LM75BD Temperature Resolutions */
#define LM75BD_THYST_RES 0.5 /* Degrees Celsius */
#define LM75BD_TOS_RES 0.5 /* Degrees Celsius */
#define LM75BD_TEMP_RES 0.125 /* Degrees Celsius */

/* LM75BD Number of Bits in Right Padding */
#define LM75BD_THYST_LSB_PADDING 7 /* Number of bits */
#define LM75BD_TOS_LSB_PADDING 7 /* Number of bits */
#define LM75BD_TEMP_LSB_PADDING 5 /* Number of bits */

/* Buffer Size of Registers (i.e. Number of Bytes to R/W) */
#define LM75BD_TEMP_BUFF_SIZE 2
#define LM75BD_CONF_BUFF_SIZE 1
#define LM75BD_THYST_BUFF_SIZE 2
#define LM75BD_TOS_BUFF_SIZE 2

/**
 * @struct Configuration struct for LM75BD,118 temperature sensor
 *
 * @param devAddr I2C address of the LM75BD
 * @param osFaltQueueSize Number of consecutive OS faults until OS output is activated (1, 2, 4, or 6)
 * @param osPolarity OS output polarity, 0 = active low, 1 = active high
 * @param osOperationMode OS output operation mode, 0 = comparator, 1 = interrupt
 * @param devOperationMode Device operation mode, 0 = normal, 1 = shutdown
 * @param overTempThreshold Overtemperature shutdown threshold, in degrees Celsius
 * @param hysteresisThreshold Hysteresis threshold, in degrees Celsius
 */
typedef struct {
    uint8_t devAddr;
    uint8_t osFaltQueueSize;
    uint8_t osPolarity;
    uint8_t osOperationMode;
    uint8_t devOperationMode;
    float overTempThresholdCelsius;
    float hysteresisThresholdCelsius;
} lm75bd_config_t;

/**
 * @brief Initialize the LM75BD
 *
 * @param config Configuration struct for LM75BD
 * @return 1 if successful, 0 otherwise
 */
uint8_t lm75bdInit(lm75bd_config_t *config);

/**
 * @brief Read the temperature from the LM75BD
 *
 * @param temp Pointer to float to store the temperature in degrees Celsius
 * @return 1 if successful, 0 otherwise
 */
uint8_t readTempLM75BD(uint8_t devAddr, float *temp);

/**
 * @brief Read the configuration register from the LM75BD
 * 
 * @param config Configuration struct for LM75BD
 * @return 1 if successful, 0 otherwise
 */
uint8_t readConfigLM75BD(lm75bd_config_t *config);

/**
 * @brief Write to the configuration register from the LM75BD
 * 
 * @param devAddr I2C address of the LM75BD
 * @param osFaltQueueSize Number of consecutive OS faults until OS output is activated (1, 2, 4, or 6)
 * @param osPolarity OS output polarity, 0 = active low, 1 = active high
 * @param osOperationMode OS output operation mode, 0 = comparator, 1 = interrupt
 * @param devOperationMode Device operation mode, 0 = normal, 1 = shutdown
 * @return 1 if successful, 0 otherwise
 */
uint8_t writeConfigLM75BD(uint8_t devAddr, uint8_t osFaltQueueSize, uint8_t osPolarity, uint8_t osOperationMode, 
                          uint8_t devOperationMode);

/**
 * @brief Get the hysteresis threshold from the LM75BD
 * 
 * @param devAddr I2C address of the LM75BD
 * @param hysteresisThresholdCelsius Hysteresis threshold, in degrees Celsius
 * @return 1 if successful, 0 otherwise
 */
uint8_t readThystLM75BD(uint8_t devAddr, float *hysteresisThresholdCelsius);

/**
 * @brief Set the hysteresis threshold for the LM75BD
 * 
 * @param devAddr I2C address of the LM75BD
 * @param hysteresisThresholdCelsius Hysteresis threshold, in degrees Celsius
 * @return 1 if successful, 0 otherwise
 */
uint8_t writeThystLM75BD(uint8_t devAddr, float hysteresisThresholdCelsius);

/**
 * @brief Get the overtemperature shutdown threshold from the LM75BD
 * 
 * @param devAddr I2C address of the LM75BD
 * @param overTempThresholdCelsius Overtemperature shutdown threshold, in degrees Celsius
 * @return 1 if successful, 0 otherwise 
 */
uint8_t readTosLM75BD(uint8_t devAddr, float *overTempThresholdCelsius);

/**
 * @brief Set the overtemperature shutdown threshold for the LM75BD
 * 
 * @param devAddr I2C address of the LM75BD
 * @param overTempThresholdCelsius Overtemperature shutdown threshold, in degrees Celsius
 * @return 1 if successful, 0 otherwise
 */
uint8_t writeTosLM75BD(uint8_t devAddr, float overTempThresholdCelsius);

/**
 * @brief Handle an OS interrupt from the LM75BD
 * 
 * @param devAddr I2C address of the LM75BD
 */
void osHandlerLM75BD(uint8_t devAddr);

#endif /* DRIVERS_INCLUDE_LM75BD_H_ */ 