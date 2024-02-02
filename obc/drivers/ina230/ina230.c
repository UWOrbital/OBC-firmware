#include "ina230.h"
#include "obc_i2c_io.h"
#include "obc_logging.h"
#include "obc_errors.h"

#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

/* INA230 Registers */
#define INA230_REG_CONF 0x00U     /* Configuration Register (R/W) */
#define INA230_REG_SH_VOLT 0x01U  /* Shunt Voltage Register (R) */
#define INA230_REG_BUS_VOLT 0x02U /* Bus Voltage Register (R) */
#define INA230_REG_POWER 0x03U    /* Power Register (R) */
#define INA230_REG_CURRENT 0x04U  /* Current Register (R) */
#define INA230_REG_CALIB 0x05U    /* Calibration Register (R/W) */

/* Buffer Size of Registers (Number of bytes to R/W) */
#define INA230_REG_BUFF_SIZE 2U

// TODO: init func
obc_error_code_t ina230Init(ina230_config_t *config) {
  obc_error_code_t errCode;

  if (!config) return OBC_ERR_CODE_INVALID_ARG;

  // write config and calibration
  // return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t writeConfigINA230(uint16_t sampleSize, uint16_t busVoltCT, uint16_t shuntVoltCT,
                                   ina230_opmode_t opMode, ina230_pin_t a0, ina230_pin_t a1) {
  obc_error_code_t errCode;

  uint8_t sampleSizeRegData;
  switch (sampleSize) {
    case 1:
      sampleSizeRegData = 0x00;
      break;
    case 4:
      sampleSizeRegData = 0x01;
      break;
    case 16:
      sampleSizeRegData = 0x02;
      break;
    case 64:
      sampleSizeRegData = 0x03;
      break;
    case 128:
      sampleSizeRegData = 0x04;
      break;
    case 256:
      sampleSizeRegData = 0x05;
      break;
    case 512:
      sampleSizeRegData = 0x06;
      break;
    case 1024:
      sampleSizeRegData = 0x07;
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t busVoltCTRegData;
  switch (busVoltCT) {
    case 140:
      busVoltCTRegData = 0x00;
      break;
    case 204:
      busVoltCTRegData = 0x01;
      break;
    case 332:
      busVoltCTRegData = 0x02;
      break;
    case 588:
      busVoltCTRegData = 0x03;
      break;
    case 1100:
      busVoltCTRegData = 0x04;
      break;
    case 2116:
      busVoltCTRegData = 0x05;
      break;
    case 4156:
      busVoltCTRegData = 0x06;
      break;
    case 8244:
      busVoltCTRegData = 0x07;
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t shuntVoltCTRegData;
  switch (shuntVoltCT) {
    case 140:
      shuntVoltCTRegData = 0x00;
      break;
    case 204:
      shuntVoltCTRegData = 0x01;
      break;
    case 332:
      shuntVoltCTRegData = 0x02;
      break;
    case 588:
      shuntVoltCTRegData = 0x03;
      break;
    case 1100:
      shuntVoltCTRegData = 0x04;
      break;
    case 2116:
      shuntVoltCTRegData = 0x05;
      break;
    case 4156:
      shuntVoltCTRegData = 0x06;
      break;
    case 8244:
      shuntVoltCTRegData = 0x07;
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t devAddr;
  if (a1 == GND) {
    if (a0 == GND) {
      devAddr = 0x40;
    } else if (a0 == VS) {
      devAddr = 0x42;
    } else if (a0 == SDA) {
      devAddr = 0x44;
    } else if (a0 == SCL) {
      devAddr = 0x46;
    } else {
      return OBC_ERR_CODE_INVALID_ARG;
    }
  } else if (a1 == VS) {
    if (a0 == GND) {
      devAddr = 0x48;
    } else if (a0 == VS) {
      devAddr = 0x4A;
    } else if (a0 == SDA) {
      devAddr = 0x4C;
    } else if (a0 == SCL) {
      devAddr = 0x4E;
    } else {
      return OBC_ERR_CODE_INVALID_ARG;
    }
  } else if (a1 == SDA) {
    if (a0 == GND) {
      devAddr = 0x50;
    } else if (a0 == VS) {
      devAddr = 0x52;
    } else if (a0 == SDA) {
      devAddr = 0x54;
    } else if (a0 == SCL) {
      devAddr = 0x56;
    } else {
      return OBC_ERR_CODE_INVALID_ARG;
    }
  } else if (a1 == SCL) {
    if (a0 == GND) {
      devAddr = 0x58;
    } else if (a0 == VS) {
      devAddr = 0x5A;
    } else if (a0 == SDA) {
      devAddr = 0x5C;
    } else if (a0 == SCL) {
      devAddr = 0x5E;
    } else {
      return OBC_ERR_CODE_INVALID_ARG;
    }
  } else {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint8_t opModeRegData;
  if (opMode > 7) {
    return OBC_ERR_CODE_INVALID_ARG;
  } else {
    opModeRegData = opMode;
  }
}

// TODO: read config func

// TODO: write calibration func

// TODO: current, voltage, power funcs
