#include "test_i2c.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

void testI2C(void) {
    sciPrintf("Testing I2C...\r\n");
    uint8_t messageData[] = {0xff, 0x00, 0xff};
    if(i2cSendTo(0x00, 3, messageData) != OBC_ERR_CODE_SUCCESS)
    {
        sciPrintf("Failed sending mesage through I2C\r\n");
        return;
    }
    sciPrintf("Successfully sending message (0xff, 0x00, 0xff) through I2C...\r\n");
}
