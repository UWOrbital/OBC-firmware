#include <spi.h>
#include <gio.h>
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_board_config.h"

#include "ov5642_reg.h"
#include "camera_reg.h"

#define CAM_I2C_WR_ADDR 0x3C
#define CAM_I2C_RD_ADDR 0x3C

#define TCA_I2C_ADDR 0x70

#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

obc_error_code_t tcaSelect(camera_t cam) {
  uint8_t tca = 0;
  if (cam == PRIMARY) {
    tca = (1 << 0);
  } else {
    tca = (1 << 1);
  }
  return i2cSendTo(TCA_I2C_ADDR, 1, &tca, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT);
}
