#include "ds3232_mz.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include "stdio.h"

uint8_t get_rtc_seconds(uint8_t *seconds) {
    uint8_t data[1];
    if(i2c_read_register(DS3232_I2C_ADDRESS, DS3232_SECONDS, data, 1) == 0) {
        sci_print_text((uint8_t*)"Failed to read seconds data\r\n", 35);
        return 0;
    }
    *seconds = data[0];
}