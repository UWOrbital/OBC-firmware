#include "ds3232_mz.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include "stdio.h"
const uint8_t LOW_BIT_MASK = 15; //00001111
const uint8_t HIGH_BIT_MASK = 240; // 01110000

uint8_t get_rtc_seconds(uint8_t *seconds) {
    uint8_t data[1];
    if(i2c_read_register(DS3232_I2C_ADDRESS, DS3232_SECONDS, data, 1) == 0) {
        sci_print_text((uint8_t*)"Failed to read seconds data\r\n", 35);
        return 0;
    }
    uint8_t one_seconds = data[0] & LOW_BIT_MASK;
    uint8_t ten_seconds = (data[0] & HIGH_BIT_MASK) >> 4;

    *seconds = ten_seconds*10 + one_seconds;
}

uint8_t get_rtc_minutes(uint8_t *minutes) {
    uint8_t data[1];
    if(i2c_read_register(DS3232_I2C_ADDRESS, DS3232_MINUTES, data, 1) == 0) {
        sci_print_text((uint8_t*)"Failed to read minutes data\r\n", 35);
        return 0;
    }
    uint8_t one_minutes = data[0] & LOW_BIT_MASK;
    uint8_t ten_minutes = (data[0] & HIGH_BIT_MASK) >> 4;

    *minutes = ten_minutes*10 + one_minutes;;
}

uint8_t get_rtc_hour(uint8_t *hours){
    uint8_t data[1];
    if(i2c_read_register(DS3232_I2C_ADDRESS, DS3232_HOURS, data, 1) == 0) {
        sci_print_text((uint8_t*)"Failed to read hours data\r\n", 35);
        return 0;
    }

} //having a hard time understanfing how data will be sent

uint8_t get_rtc_time(int *time) {
    uint8_t min, sec, hours;

    get_rtc_minutes(&min);
    get_rtc_seconds(&sec);    
    get_rtc_hour(&hours);
    *time = sec;
    *(time + 1) = min;
    *(time + 2) = hours;
}

uint8_t get_rtc_date(uint8_t* date) {
    uint8_t data[1];
    if(i2c_read_register(DS3232_I2C_ADDRESS, DS3232_DATE, data, 1) == 0) {
        sci_print_text((uint8_t*)"Failed to read date data\r\n", 35);
        return 0;
    }
    uint8_t one_date = data[0] & LOW_BIT_MASK;
    uint8_t ten_date = (data[0] & HIGH_BIT_MASK) >> 4;

    *date = ten_date*10 + one_date;
}

uint8_t get_rtc_year(uint8_t* year) {
    uint8_t data[1];
    if(i2c_read_register(DS3232_I2C_ADDRESS, DS3232_YEAR, data, 1) == 0) {
        sci_print_text((uint8_t*)"Failed to read year data\r\n", 35);
        return 0;
    }
    uint8_t one_year = data[0] & LOW_BIT_MASK;
    uint8_t ten_year = (data[0] & HIGH_BIT_MASK) >> 4;

    *year = ten_year*10 + one_year;
}