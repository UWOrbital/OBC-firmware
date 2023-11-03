#include "max17320.h"
#include "max17320_defs.h"
#include "obc_sci_io.h"

static configuration_value_map_t volatileConfig[2] = {
    {.address = 0x01D9, .value = 0xFF00},  // Page 78 Register nJEITAV Register
    {.address = 0x01DE, .value = 0xFFEE}   // Page 81 nODSCCfg Register (1DEh) Format
};

static max17320_config_t max17320Config = {
    .volatileConfigSize = 2,
    .volatileConfiguration = &volatileConfig[0],
    .thresholdConfigSize = 0,
    .thresholdIsNonVolatile = 0,
    .nonVolatileConfiguration = 0,
    .nonVolatileConfigSize = 0,
    .measurementThresholds = 0,
};

void testMax17320() {
  initBmsInterface(max17320Config);
  obc_error_code_t errCode = 0;
  uint16_t data = 0;
  for (uint8_t i = 0; i < max17320Config.volatileConfigSize; ++i) {
    errCode = readBmsRegister(volatileConfig[i].address, &data);
    sciPrintf("Expected data... %d \r \n", volatileConfig[i].value);
    sciPrintf("Recieved data... %d \r \n", data);
  }
}
