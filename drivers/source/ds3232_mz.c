#include "ds3232_mz.h"
#include "obc_sci_io.h"
#include "obc_i2c_io.h"

#include "stdio.h"
const uint8_t LOW_BIT_MASK = 15; //00001111
const uint8_t HIGH_BIT_MASK = 112; // 01110000

uint8_t getSecondsRTC(uint8_t *seconds) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_SECONDS, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read seconds data\r\n", 35);
        return 0;
    }
    uint8_t oneSeconds = data[0] & LOW_BIT_MASK;
    uint8_t ten_seconds = (data[0] & HIGH_BIT_MASK) >> 4;

    *seconds = ten_seconds*10 + oneSeconds;

    return 1;
}

uint8_t getMinutesRTC(uint8_t *minutes) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_MINUTES, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read minutes data\r\n", 35);
        return 0;
    }
    uint8_t one_minutes = data[0] & LOW_BIT_MASK;
    uint8_t ten_minutes = (data[0] & HIGH_BIT_MASK) >> 4;

    *minutes = ten_minutes*10 + one_minutes;

    return 1;
}

uint8_t getHourRTC(uint8_t *hours){
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_HOURS, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read hours data\r\n", 35);
        return 0;
    }

    return 1;

} //having a hard time understanfing how data will be sent

uint8_t getDayRTC(uint8_t *days) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_DAY, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read days data\r\n", 35);
        return 0;
    }
    *days = data[0];

    return 1;
}

uint8_t getDateRTC(uint8_t* date) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_DATE, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read date data\r\n", 35);
        return 0;
    }
    uint8_t ones_date = data[0] & LOW_BIT_MASK;
    uint8_t tens_date = (data[0] & 48) >> 4;   // 48 = 0011 0000
    *date = tens_date*10 + ones_date;

    return 1;
}

uint8_t getMonthRTC(uint8_t *month) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_MONTH, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read month data\r\n", 35);
        return 0;
    }
    uint8_t ones_month = data[0] & LOW_BIT_MASK;
    uint8_t tens_month = (data[0] & 16) >> 4; 
    *month = tens_month*10 + ones_month;

    return 1;
}

uint8_t getYearRTC(uint8_t* year) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_YEAR, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read years data\r\n", 35);
        return 0;
    }
    uint8_t ones_year = data[0] & LOW_BIT_MASK;
    uint8_t tens_year = (data[0] & HIGH_BIT_MASK) >> 4;

    *year = tens_year*10 + ones_year;

    return 1;
}

uint8_t getCurrentTimeRTC(rtc_time_t *time) {

    if(getMinutesRTC(&(time->minutes)) && getSecondsRTC(&(time->seconds)) && getHourRTC(&(time->hours)))
        return 1;
    else
        return 0;
}

uint8_t getCurrentDateTimeRTC(rtc_date_time_t *dateTime) {
    if(getDateRTC(&(dateTime->date)) && getMonthRTC(&(dateTime->month)) && getYearRTC(&(dateTime->year))
     && getHoursRTC(&(dateTime->hours)) && getMinutesRTC(&(dateTime->minutes)) && getSeconds(&(dateTime->seconds)))
}






