#ifndef DRIVERS_INCLUDE_DS3232_MZH
#define DRIVERS_INCLUDE_DS3232_MZH

#include "stdint.h"

//TODO
// Implement get_rtc_hour, get_rtc_minutes, get_rtc_seconds, get_rtc_date, get_rtc_year, get_rtc_time, and get_rtc_date

/*I2C Address for device*/

#define DS3232_I2C_ADDRESS  0x68

/*DS3232 registers*/

#define DS3232_SECONDS      0x00
#define DS3232_MINUTES      0x01
#define DS3232_HOURS        0x02
#define DS3232_DAY          0x03
#define DS3232_DATE         0X04
#define DS3232_MONTH        0x05
#define DS3232_YEAR         0x06
#define DS3232_TEMP_MSB     0x11
#define DS3232_TEMP_LSB     0x12             
#define DS3232_CONTROL      0X0E
#define DS3232_STATUS       0X0F
#define DS3232_AGING        0X10

#define HOUR_MODE           32


typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
}rtc_time_t;

typedef struct {
    uint8_t date;
    uint8_t month;
    uint8_t year;
}rtc_date_t;

typedef struct {
    uint8_t date;
    uint8_t month;
    uint8_t year;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
}rtc_date_time_t;

typedef struct {
    uint8_t EOSC;     // Enable oscillator
    uint8_t BBSQW;     // Battery-backed square-wave enable
    uint8_t CONV;       // Convert temperature
    uint8_t INTCN;      // Interrupt control
    uint8_t A2IE;       // Alarm 2 interrupt enable
    uint8_t A1IE;       // Alarm 1 interrupt enable
}control_t;

typedef struct {
    uint8_t OSF;     // oscillator stop flag
    uint8_t BB32KHZ;     // Battery-backed 32 kHz output
    uint8_t EN32KHZ;       //  Enabled 32.768kHz output
    uint8_t BSY;      // Device busy
    uint8_t A2F;       // Alarm 2 flag
    uint8_t A1F;       // Alarm 1 flag
}status_t;

/*-------GET FUNCTIONS---------*/
uint8_t getSecondsRTC(uint8_t* seconds);
uint8_t getMinutesRTC(uint8_t* minutes);
uint8_t getHoursRTC(uint8_t* hours);  //having a hard time understanfing how data will be sent
uint8_t getDayRTC(uint8_t* day);
uint8_t getDateRTC(uint8_t* date);
uint8_t getMonthRTC(uint8_t* month);
uint8_t getYearRTC(uint8_t* year); 
uint8_t getCurrentDateTimeRTC(rtc_date_time_t *dateTime);
uint8_t getCurrentTimeRTC(rtc_time_t *time);  //pass array of size 3 into this fucntion
uint8_t getAlarmTimeRTC();
uint8_t getControlRTC(control_t *control);
uint8_t getStatusRTC(status_t *status);
uint8_t getAgingOffsetRTC(int8_t* agingOffset);
float getTemperatureRTC(float* temperature);

/*-------SET FUNCTIONS---------*/
uint8_t setSecondsRTC(uint8_t writeSeconds);
uint8_t setMinutesRTC(uint8_t writeMinutes);
uint8_t setHourRTC(uint8_t writeHour);
uint8_t setDayRTC(uint8_t writeDays);
uint8_t setDateRTC(uint8_t writeDates);
uint8_t setMonthRTC(uint8_t writeMonths);
uint8_t setYearRTC(uint8_t writeYears);
uint8_t setAlarmTimeRTC();
uint8_t setAlarmModeRTC();
uint8_t setControlRTC(control_t *writeControl);
uint8_t setStatusRTC(status_t *writeStatus);
uint8_t setAgingOffsetRTC(int8_t agingOffset);

/*-------UTILITY FUNCTIONS---------*/
uint8_t combineWriteVal(uint8_t inputVal);

#endif /* DS3232_MZH */