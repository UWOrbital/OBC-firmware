#include "ds3232_mz.h"
#include "obc_i2c_io.h"
#include "obc_errors.h"
#include <stdint.h>

const rtc_alarm_mode_t ALARM_ONCE_A_SECOND = {1, 1, 1, 1};
const rtc_alarm_mode_t SECONDS_MATCH = {1, 1, 1, 0};
const rtc_alarm_mode_t SECONDS_MINUTES_MATCH = {1, 1, 0, 0};
const rtc_alarm_mode_t SECONDS_MINUTES_HOURS_MATCH = {1, 0, 0, 0};
const rtc_alarm_mode_t SECONDS_MINUTES_HOURS_DAY_OR_DATE_MATCH = {0, 0, 0, 0};

const uint8_t ONES_DIGIT_BIT_MASK = 0b00001111;
const uint8_t TENS_DIGIT_BIT_MASK = 0b01110000;
const uint8_t GET_SEVENTH_BIT = 0b1000000;

void resetRTC(void) {
    /* GIO pin output in the rm46 is the input for the RST pin the RTC. Setting GIOApin1 to 0
    means that the open drain circuit in the RTC will be high impedence (no current flows). So
    nothing will happen. Setting it high will pull the open draain circuit to ground making it 0
    which resets the RTC. */ 
    gioSetBit(RTC_RST_GIO_PORT, 1, 1);
    gioSetBit(RTC_RST_GIO_PORT, 1, 0);
}

obc_error_code_t getSecondsRTC(uint8_t *seconds) {
    if (seconds == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_SECONDS, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *seconds = TwoDigitDecimalFromBCD(data, ONES_DIGIT_BIT_MASK, TENS_DIGIT_BIT_MASK);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getMinutesRTC(uint8_t *minutes) {
    if (minutes == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_MINUTES, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *minutes = TwoDigitDecimalFromBCD(data, ONES_DIGIT_BIT_MASK, TENS_DIGIT_BIT_MASK);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getHourRTC(uint8_t *hours){
    if (hours == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_HOURS, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *hours = TwoDigitDecimalFromBCD(data, ONES_DIGIT_BIT_MASK, TENS_DIGIT_BIT_MASK);
    return OBC_ERR_CODE_SUCCESS;

} 

obc_error_code_t getDayRTC(uint8_t *days) {
    if (days == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_DAY, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *days = data;

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getDateRTC(uint8_t* date) {
    if (date == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_DATE, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *date = TwoDigitDecimalFromBCD(data, ONES_DIGIT_BIT_MASK, 0b00110000);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getMonthRTC(uint8_t *month) {
    if (month == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_MONTH, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *month = TwoDigitDecimalFromBCD(data, ONES_DIGIT_BIT_MASK, 0b00010000);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getYearRTC(uint8_t* year) {
    if (year == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_YEAR, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    *year = TwoDigitDecimalFromBCD(data, ONES_DIGIT_BIT_MASK, TENS_DIGIT_BIT_MASK);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getCurrentTimeRTC(rtc_time_t *time) {
    uint8_t result = 0;
    rtc_time_t *temp;
    obc_error_code_t retValMin = getMinutesRTC(&(temp->minutes));
    obc_error_code_t retValSec = getSecondsRTC(&(temp->seconds));
    obc_error_code_t retValHour = geHourRTC(&(temp->hours));
    obc_error_code_t retVal;

    if(retValMin == OBC_ERR_CODE_SUCCESS && retValSec == OBC_ERR_CODE_SUCCESS 
        && retValHour == OBC_ERR_CODE_SUCCESS) 
    {
        time = temp;
        return OBC_ERR_CODE_SUCCESS;
    }
    else
        return OBC_ERR_CODE_UNKNOWN;
        

    if (getMinutesRTC(&(temp->minutes)) == OBC_ERR_CODE_SUCCESS) 
        retVal = OBC_ERR_CODE_SUCCESS;
    else
        return OBC_ERR_CODE_UNKNOWN;

    if (getSecondsRTC(&(temp->seconds)) == OBC_ERR_CODE_SUCCESS) 
        retVal = OBC_ERR_CODE_SUCCESS;
    else
        return OBC_ERR_CODE_UNKNOWN;

    if (getHourRTC(&(temp->hours)) == OBC_ERR_CODE_SUCCESS) 
        retVal = OBC_ERR_CODE_SUCCESS;
    else
        return OBC_ERR_CODE_UNKNOWN;

    return result;

}

obc_error_code_t getCurrentDateTimeRTC(rtc_date_time_t *dateTime) {
    uint8_t result = 0;

    if (getDateRTC(&(dateTime->date->date))) 
        result = 1;
    else
        result = 0;

    if (getMonthRTC(&(dateTime->date->month))) 
        result = 1;
    else
        result = 0;

    if (getYearRTC(&(dateTime->date->year))) 
        result = 1;
    else
        result = 0;

    if (getHourRTC(&(dateTime->time->hours))) 
        result = 1;
    else
        result = 0;

    if (getMinutesRTC(&(dateTime->time->minutes))) 
        result = 1;
    else
        result = 0;

    if (getSecondsRTC(&(dateTime->time->seconds))) 
        result = 1;
    else
        result = 0;

    return result;
}

obc_error_code_t getAlarmTimeRTC(rtc_alarm_time_t *alarmTime) {
    uint8_t seconds;

    if (i2cReadReg(DS3232_I2C_ADDRESS, DS3232_ALARM_1_SECONDS, &seconds, 1) == 0) {
        return 0;
    }
    alarmTime->time->seconds = (seconds & ONES_DIGIT_BIT_MASK) | (seconds & TENS_DIGIT_BIT_MASK);

    uint8_t minutes;
    if (i2cReadReg(DS3232_I2C_ADDRESS, DS3232_ALARM_1_MINUTES, &minutes, 1) == 0) {
        return 0;
    }
    alarmTime->time->minutes = (minutes & ONES_DIGIT_BIT_MASK) | (minutes & TENS_DIGIT_BIT_MASK);

    uint8_t hours;
    if (i2cReadReg(DS3232_I2C_ADDRESS, DS3232_ALARM_1_HOURS, &hours, 1) == 0) {
        return 0;
    }
    alarmTime->time->hours = (hours & ONES_DIGIT_BIT_MASK) | (hours & 48);

    uint8_t dayOrDates;
    if (i2cReadReg(DS3232_I2C_ADDRESS, DS3232_ALARM_1_DAY_OR_DATE, &dayOrDates, 1) == 0) {
        return 0;
    }

    // True if day, false if date
    if (dayOrDates & GET_SEVENTH_BIT)
        alarmTime->day = (dayOrDates & ONES_DIGIT_BIT_MASK);
    else
        alarmTime->date = TwoDigitDecimalFromBCD(dayOrDates, ONES_DIGIT_BIT_MASK, 0b110000);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getControlRTC(rtc_control_t *control) {
    if (control == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_CONTROL, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }

    control->EOSC = (data >> 7) & 1;
    control->BBSQW = (data >> 6) & 1;
    control->CONV = (data >> 5) & 1;
    control->INTCN = (data >> 2) & 1;
    control->A2IE = (data >> 1) & 1;
    control->A1IE = (data & 1);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getStatusRTC(rtc_status_t *status) {
    if (status == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_STATUS, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }

    status->OSF = (data >> 7) & 1;
    status->BB32KHZ = (data >> 6) & 1;
    status->EN32KHZ = (data >> 3) & 1;
    status->BSY = (data >> 2) & 1;
    status->A2F = (data >> 1) & 1;
    status->A1F = (data & 1);

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getAgingOffsetRTC(int8_t* agingOffset) {
    if (agingOffset == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t data;
    obc_error_code_t retVal = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_AGING, &data, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }   

    *agingOffset = data;
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t getTemperatureRTC(float* temperature) {
    if (temperature == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
        
    uint8_t dataLSB, dataMSB;
    obc_error_code_t retVal1 = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_TEMP_MSB, &dataMSB, 1);  
    obc_error_code_t retVal2 = i2cReadReg(DS3232_I2C_ADDRESS, DS3232_TEMP_MSB, &dataLSB, 1);
    if (retVal1 != OBC_ERR_CODE_SUCCESS || retVal2 != OBC_ERR_CODE_SUCCESS) {
        return retVal1;
    }

    int8_t tempDecimal = dataMSB;

    uint8_t tempFraction = dataLSB;
    int16_t tempCombined = (tempDecimal<<2) | (tempFraction>>6);
    *temperature = tempCombined * TEMP_RESOLUTION;

    return OBC_ERR_CODE_SUCCESS;
}


obc_error_code_t setSecondsRTC(uint8_t writeSeconds) {
    uint8_t writeVal =  TwoDigitDecimalToBCD(writeSeconds);
    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_SECONDS, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setMinutesRTC(uint8_t writeMinutes) {
    uint8_t writeVal =  TwoDigitDecimalToBCD(writeMinutes);
    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_MINUTES, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setHourRTC(uint8_t writeHours) {
    // DEFAULT setting hour to 24 hour mode
    uint8_t writeVal = HOUR_MODE | TwoDigitDecimalToBCD(writeHours);
    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_HOURS, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setDayRTC(uint8_t writeDays) {
    if (!i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_DAY, &writeDays, 1))
        return 0;
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setDateRTC(uint8_t writeDates) {
    uint8_t writeVal =  TwoDigitDecimalToBCD(writeDates);
    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_DATE, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setMonthRTC(uint8_t writeMonths) {
    uint8_t writeVal =  TwoDigitDecimalToBCD(writeMonths);
    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_MONTH, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setYearRTC(uint8_t writeYears) {
    uint8_t writeVal =  TwoDigitDecimalToBCD(writeYears);
    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_YEAR, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    
    return OBC_ERR_CODE_SUCCESS;
}


/* writeSeconds, writeminutes and so on written on the same address as A1m1, a2m2 etc. Thus in my implementation, 
I have combined seconds value with A1m1, minutes value with a2m2 before writing it to the correct address. Something
similar with day or date mode */

obc_error_code_t setAlarmRTC(rtc_alarm_time_t *writeAlarmTime, rtc_alarm_mode_t *writeAlarmMode,  uint8_t dayOrDate) {
    uint8_t writeSeconds = TwoDigitDecimalToBCD(writeAlarmTime->time->seconds) | ((writeAlarmMode-> A1M1) << 7);
    if (!i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_ALARM_1_SECONDS, &writeSeconds, 1)) {
        return 0;
    }

    uint8_t writeMinutes =  TwoDigitDecimalToBCD(writeAlarmTime->time->minutes) | ((writeAlarmMode-> A1M2) << 7);
    if (!i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_ALARM_1_MINUTES, &writeMinutes, 1)) {
        return 0;
    }

    // DEFAULT setting hour to 24 hour mode
    uint8_t writeHours = HOUR_MODE | TwoDigitDecimalToBCD(writeAlarmTime->time->hours) | ((writeAlarmMode-> A1M3) >> 7);
    if (!i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_HOURS, &writeHours, 1)) {
        return 0;
    }

    // if dayOrDate is 1, its in day mode else date mode
    if (dayOrDate) {
        uint8_t writeDay =  TwoDigitDecimalToBCD(writeAlarmTime->day) | ((writeAlarmMode-> A1M4) >> 7) | DAY_MODE;
        if (!i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_DATE, &writeDay, 1)) {
          return 0;
        }
    }
    else {
        uint8_t writeDate =  TwoDigitDecimalToBCD(writeAlarmTime->date) | ((writeAlarmMode-> A1M4) >> 7) | DATE_MODE;
        if (!i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_DATE, &writeDate, 1)) {
          return 0;
        }
    }

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setControlRTC(rtc_control_t *writeControl) {
   uint8_t writeVal =  (writeControl->EOSC << 7) |
                        (writeControl->BBSQW << 6) |
                        (writeControl->CONV << 5) |
                        (writeControl->INTCN << 2) |
                        (writeControl->A2IE << 1) |
                        (writeControl->A1IE);

    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_CONTROL, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setStatusRTC(rtc_status_t *writeStatus) {
    uint8_t writeVal = (writeStatus->OSF << 7) |
                        (writeStatus->BB32KHZ << 6) |
                        (writeStatus->EN32KHZ << 3) |
                        (writeStatus->BSY << 2) |
                        (writeStatus->A2F << 1) |
                        (writeStatus->A1F);

    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_STATUS, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t setAgingOffsetRTC(int8_t writeAgingOffset) {
    uint8_t writeVal = (writeAgingOffset < 0) ? (1 << 7) : 0;
    writeVal |= (writeAgingOffset < 0) ? -writeAgingOffset : writeAgingOffset;

    obc_error_code_t retVal = i2cWriteReg(DS3232_I2C_ADDRESS, DS3232_AGING, &writeVal, 1);
    if (retVal != OBC_ERR_CODE_SUCCESS) {
        return retVal;
    }
    
    return OBC_ERR_CODE_SUCCESS;
}


static uint8_t TwoDigitDecimalToBCD(uint8_t inputVal) {
    uint8_t onesdigit = inputVal % 10;
    uint8_t tensdigit = (inputVal / 10) << 4;
    uint8_t writeVal = onesdigit | tensdigit;

    return writeVal;
}

static uint8_t TwoDigitDecimalFromBCD(uint8_t data, uint8_t onesDigitBitMask, uint8_t tensDigitBitMask) {
    uint8_t onesDigit = data & onesDigitBitMask;
    uint8_t tensDigit = (data & tensDigitBitMask) >> 4;
    uint8_t result = tensDigit*10 + onesDigit;
    return result;
}