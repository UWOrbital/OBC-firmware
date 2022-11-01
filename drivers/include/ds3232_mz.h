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
uint8_t getControlRTC();
uint8_t getStatusRTC();
uint8_t getAgingOffsetRTC();
uint8_t getTemperatureRTC();


#endif /* DS3232_MZH */