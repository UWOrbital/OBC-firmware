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
    uint8_t result = 0;

    // More SPECIFIC error handling conditions to be added in the future hence split each 
    // function call into seperate if else statements

    if(getMinutesRTC(&(time->minutes))) 
        result = 1;
    else
        result = 0;
    
    if(getSecondsRTC(&(time->seconds))) 
        result = 1;
    else
        result = 0;

    if(getHourRTC(&(time->hours))) 
        result = 1;
    else
        result = 0;

    
}

uint8_t getCurrentDateTimeRTC(rtc_date_time_t *dateTime) {
   
    uint8_t result = 0;

    // More SPECIFIC error handling conditions to be added in the future hence split each 
    // function call into seperate if else statements
    if(getDateRTC(&(dateTime->date))) 
        result = 1;
    else
        result = 0;
    
    if(getMonthRTC(&(dateTime->month))) 
        result = 1;
    else
        result = 0;

    if(getYearRTC(&(dateTime->year))) 
        result = 1;
    else
        result = 0;

    if(getHoursRTC(&(dateTime->hours))) 
        result = 1;
    else
        result = 0;

    if(getMinutesRTC(&(dateTime->minutes))) 
        result = 1;
    else
        result = 0;

    if(getSeconds(&(dateTime->seconds))) 
        result = 1;
    else
        result = 0;
}

uint8_t getAlarmTimeRTC() {

}

uint8_t getControlRTC(control_t *control) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_CONTROL, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read control data\r\n", 35);
        return 0;
    }

    // setting all control struct variables to 1 or 0 based on what is read

    control->EOSC = (data[0] & 128) >> 7;
    control->BBSQW = (data[0] & 64) >> 6;
    control->CONV = (data[0] & 32) >> 5;
    control->INTCN = (data[0] & 4) >> 2;
    control->A2IE = (data[0] & 2) >> 1;
    control->A1IE = data[0] & 1;
}

uint8_t getStatusRTC(status_t *status) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_STATUS, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read status data\r\n", 35);
        return 0;
    }

    // setting all STATUS struct variables to 1 or 0 based on what is read

    status->OSF = (data[0] & 128) >> 7;
    status->BB32KHZ = (data[0] & 64) >> 6;
    status->EN32KHZ = (data[0] & 8) >> 3;
    status->BSY = (data[0] & 4) >> 2;
    status->A2F = (data[0] & 2) >> 1;
    status->A1F = data[0] & 1;
}
uint8_t getAgingOffsetRTC(int8_t* agingOffset) {
    uint8_t data[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_AGING, data, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read aging offset data\r\n", 35);
        return 0;
    }
    agingOffset = data[0];

    return 1;
}

float getTemperatureRTC(float* temperature) {
    int8_t dataMSB[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_TEMP_MSB, dataMSB, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read temperature MSB data\r\n", 35);
        return 0;
    }

    int8_t dataLSB[1];
    if(i2cReadReg(DS3232_I2C_ADDRESS, DS3232_TEMP_LSB, dataLSB, 1) == 0) {
        printTextSci(scilinREG, (uint8_t*)"Failed to read temperature LSB data\r\n", 35);
        return 0;
    }
    
    
    int8_t tempDecimal = dataMSB[0];

    // TODO need to convert to fraction
    uint8_t tempFraction = dataLSB[0];
    int16_t tempCombined = (tempDecimal<<2) | (tempFraction>>6);
    *temperature = tempCombined * 0.25;

    return 1;
}







