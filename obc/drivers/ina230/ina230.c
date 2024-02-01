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

// TODO: write config func

// TODO: read config func

// TODO: write calibration func

// TODO: current, voltage, power funcs
