#include <spi.h>
#include <gio.h>
#include "obc_i2c_io.h"
#include "obc_spi_io.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_board_config.h"

#define TCA_I2C_ADDR 0x70

#define I2C_MUTEX_TIMEOUT portMAX_DELAY
#define I2C_TRANSFER_TIMEOUT pdMS_TO_TICKS(100)

// TODO: Fully develop and test this driver
// 1. Check if more features need to be implemented
// 2. Test driver
// 3. Integrate with camera control

obc_error_code_t tcaSelect(uint8_t portNum) {
  uint8_t tca = 0;
  if (portNum > 7) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  tca = 1 << portNum;
  return i2cSendTo(TCA_I2C_ADDR, 1, &tca, I2C_MUTEX_TIMEOUT, I2C_TRANSFER_TIMEOUT);
}
