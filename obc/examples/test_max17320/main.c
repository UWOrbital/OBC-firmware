#include "obc_sci_io.h"

#include "obc_i2c_io.h"
#include "obc_logging.h"
#include "max17320.h"

#include "obc_scheduler_config.h"
#include "state_mgr.h"
#include <FreeRTOS.h>

#include <sys_common.h>
#include <sci.h>
#include <i2c.h>

#include <string.h>

int main(void) {
  sciInit();
  i2cInit();

  initSciMutex();
  initI2CMutex();

  sciPrintf("Starting BMS test.... \r\n");

  obc_error_code_t errCode;
  bms_register_t bmsRegister = {.address = BMS_VEMPTY, .isThreshold = 0, .configurationValue = 0xDEAD};
  errCode = writeBmsRegister(&bmsRegister);

  if (errCode != OBC_ERR_CODE_SUCCESS) sciPrintf("Did not recieve Success error code during write  \r\n");

  bms_register_t bmsRegisterCopy = bmsRegister;
  bmsRegisterCopy.configurationValue = 0x00;
  errCode = readBmsRegister(&bmsRegisterCopy);
  if (errCode != OBC_ERR_CODE_SUCCESS) sciPrintf("Did not recieve Success error code during read \r\n");
  if (bmsRegisterCopy.configurationValue != bmsRegister.configurationValue)
    sciPrintf("Did not recieve the expected value. Recieved %X \r\n", bmsRegisterCopy.configurationValue);

  while (1)
    ;
}
