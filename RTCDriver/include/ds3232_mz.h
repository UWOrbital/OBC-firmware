#ifndef DS3232_MZH
#define DS3232_MZH

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

uint8_t get_rtc_seconds(uint8_t* seconds);
uint8_t get_rtc_minutes(uint8_t* minutes);
uint8_t get_rtc_hour(uint8_t* hours);  //having a hard time understanfing how data will be sent
uint8_t get_rtc_date(uint8_t* date);
uint8_t get_rtc_year(uint8_t* year); 
uint8_t get_rtc_time(int *time);  //pass array of size 3 into this fucntion

#endif /* DS3232_MZH */