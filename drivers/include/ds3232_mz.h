#ifndef DRIVERS_INCLUDE_DS3232_MZ_H_
#define DRIVERS_INCLUDE_DS3232_MZ_H_

#include <stdint.h>
#include <gio.h>


#define DS3232_I2C_ADDRESS  0x68U

/* DS3232 registers */
#define DS3232_SECONDS      0x00U
#define DS3232_MINUTES      0x01U
#define DS3232_HOURS        0x02U
#define DS3232_DAY          0x03U
#define DS3232_DATE         0X04U
#define DS3232_MONTH        0x05U
#define DS3232_YEAR         0x06U
#define DS3232_TEMP_MSB     0x11U
#define DS3232_TEMP_LSB     0x12U            
#define DS3232_CONTROL      0X0EU
#define DS3232_STATUS       0X0FU
#define DS3232_AGING        0X10U

#define DS3232_ALARM_1_SECONDS      0x07U
#define DS3232_ALARM_1_MINUTES      0x08U
#define DS3232_ALARM_1_HOURS        0x09U
#define DS3232_ALARM_1_DAY_OR_DATE  0x0AU

#define HOUR_MODE           32
#define DAY_MODE            64
#define DATE_MODE           0
#define TEMP_RESOLUTION     0.25

#define RTC_RST_GIO_PIN     1
#define RTC_RST_GIO_PORT    gioPORTA

/**
 * @struct rtc_time_t
 * @brief DS3232 time structure definition
 * @var hours Specifies the RTC hour value. (0-23) 
 * @var minutes Specifies the RTC minutes value. (0-59)
 * @var seconds Specifies the RTC seconds value. (0-59)
 * @note This stucture is used to set and get the RTC time. The time is set in the 24-hour format by default.
 */
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} rtc_time_t;

/**
 * @struct rtc_date_t
 * @brief DS3232 date structure definition
 * @var date Specifies the RTC date value. (01-31) 
 * @var month Specifies the RTC month value. (01-12)
 * @var year Specifies the RTC year value. (00-99)
 */
typedef struct {
    uint8_t date;
    uint8_t month;
    uint8_t year;
} rtc_date_t;

/**
 * @struct rtc_date_time_t
 * @brief DS3232 date plus time structure definition
 * @var date Specifies the RTC date value. (01-31) 
 * @var month Specifies the RTC month value. (01-12)
 * @var year Specifies the RTC year value. (00-99)
 * @var hours Specifies the RTC hour value. (0-23) 
 * @var minutes Specifies the RTC minutes value. (0-59)
 * @var seconds Specifies the RTC seconds value. (0-59)
 */
typedef struct {
    rtc_date_t *date;
    rtc_time_t *time;
} rtc_date_time_t;

/**
 * @struct rtc_control_t
 * @brief DS3232 control register structure definition
 * @var EOSC    Enable oscillator
 * @var BBSQW   Battery-backed square-wave enable
 * @var CONV    Convert temperature
 * @var INTCN   Interrupt control 
 * @var A2IE    Alarm 2 interrupt enable
 * @var A1IE    Alarm 1 interrupt enable
 * @note Used to get and set the control registers
 */
typedef struct {
    uint8_t EOSC;    
    uint8_t BBSQW;     
    uint8_t CONV;      
    uint8_t INTCN;     
    uint8_t A2IE;     
    uint8_t A1IE;       
} rtc_control_t;

/**
 * @struct rtc_status_t
 * @brief DS3232 status register structure definition
 * @var OSF     Oscillator stop flag
 * @var BB32KHZ Battery-backed 32kHz output
 * @var EN32KHZ Enabled 32.768kHz output
 * @var BSY     Busy. This bit indicates the device is busy executing temperature conversion function
 * @var A2F     Alarm 2 flag
 * @var A1F     Alarm 1 flag
 * @note Used to get and set the status registers
 */
typedef struct {
    uint8_t OSF;     // oscillator stop flag
    uint8_t BB32KHZ;     // Battery-backed 32 kHz output
    uint8_t EN32KHZ;       //  Enabled 32.768kHz output
    uint8_t BSY;      // Device busy
    uint8_t A2F;       // Alarm 2 flag
    uint8_t A1F;       // Alarm 1 flag
} rtc_status_t;

/**
 * @struct rtc_alarm_time_t
 * @brief DS3232 alarm time structure definition
 * @var date    Specifies the alarm date value. (01-31) 
 * @var day     Specifies the alarm day value. (1-7)
 * @var hours   Specifies the alarm hours value. (00-24)
 * @var minutes Specifies the alarm minutes value. (0-59)
 * @var seconds Specifies the alarm seconds value. (0-59)
 * @note Used to get and set alarm 1. Day or date depends on the mode selected as both of them write to the same address. This is provisioned in the setAlarmRTC() function
 */
typedef struct {
    uint8_t date;   
    uint8_t day;   
    rtc_time_t *time;
} rtc_alarm_time_t;

/**
 * @struct rtc_alarm_mode_t
 * @brief DS3232 alarm mode structure definition
 * @var A1M1, A1M2, A1M3, A1M4 are variables that need to be set to one or zero
 * @note The different modes and combinations in which A1M1 - A1M4 need to be set are given as ready constants at the top of the source file for this driver
 */
typedef struct {
    uint8_t A1M1;
    uint8_t A1M2;
    uint8_t A1M3;
    uint8_t A1M4;
} rtc_alarm_mode_t;

/*-------RESET RTC---------*/
void resetRTC(void);

/*-------GET FUNCTIONS---------*/

/**
 * @brief Gets seconds data from RTC.
 * 
 * @param seconds uint8_t pointer to store seconds.
 * @return 1 if the seconds was accesed, 0 otherwise.
 */
uint8_t getSecondsRTC(uint8_t* seconds);

/**
 * @brief Gets minutes data from RTC.
 * 
 * @param minutes uint8_t pointer to store minutes.
 * @return 1 if the minutes was accesed, 0 otherwise.
 */
uint8_t getMinutesRTC(uint8_t* minutes);

/**
 * @brief Gets hour data from RTC.
 * 
 * @param hour uint8_t pointer to store hour.
 * @return 1 if the hour was accesed, 0 otherwise.
 */
uint8_t getHoursRTC(uint8_t* hours);

/**
 * @brief Gets days data from RTC.
 * 
 * @param days uint8_t pointer to store days.
 * @return 1 if the days was accesed, 0 otherwise.
 */
uint8_t getDayRTC(uint8_t* day);

/**
 * @brief Gets date data from RTC.
 * 
 * @param date uint8_t pointer to store date.
 * @return 1 if the date was accesed, 0 otherwise.
 */
uint8_t getDateRTC(uint8_t* date);

/**
 * @brief Gets month data from RTC.
 * 
 * @param month uint8_t pointer to store month.
 * @return 1 if the month was accesed, 0 otherwise.
 */
uint8_t getMonthRTC(uint8_t* month);

/**
 * @brief Gets year data from RTC.
 * 
 * @param year uint8_t pointer to store year.
 * @return 1 if the year was accesed, 0 otherwise.
 */
uint8_t getYearRTC(uint8_t* year); 

/**
 * @brief Gets date and time data from RTC by populating rtc_date_time_t struct members.
 * 
 * @param dateTime struct pointer to store date and time.
 * @return 1 if the date and time were accesed, 0 otherwise.
 */
uint8_t getCurrentDateTimeRTC(rtc_date_time_t *dateTime);

/**
 * @brief Gets current time data from RTC by populating rtc_date_time_t struct members.
 * 
 * @param time struct pointer to store current time.
 * @return 1 if the current time was accesed, 0 otherwise.
 */
uint8_t getCurrentTimeRTC(rtc_time_t *time); 

/**
 * @brief Gets current alarm configuration from RTC by populating rtc_alarm_time_t struct members.
 * 
 * @param alarmTime struct pointer to store alarm time.
 * @return 1 if the alarm time was accesed, 0 otherwise.
 */
uint8_t getAlarmTimeRTC(rtc_alarm_time_t *alarmTime);

/**
 * @brief Gets current control register configuration from RTC by populating rtc_control_t struct members.
 * 
 * @param control struct pointer to store control configuration.
 * @return 1 if control register was accesed, 0 otherwise.
 */
uint8_t getControlRTC(rtc_control_t *control);

/**
 * @brief Gets current status register configuration from RTC by populating rtc_status_t struct members.
 * 
 * @param status struct pointer to store status configuration.
 * @return 1 if status register was accesed, 0 otherwise.
 */
uint8_t getStatusRTC(rtc_status_t *status);

/**
 * @brief Gets aging offset from RTC.
 * 
 * @param agingOffset int8_t pointer to store aging offset.
 * @return 1 if aging register was accesed, 0 otherwise.
 */
uint8_t getAgingOffsetRTC(int8_t* agingOffset);

/**
 * @brief Gets temperature from RTC.
 * 
 * @param temperature float pointer to store temperature.
 * @return 1 if temperature register was accesed, 0 otherwise.
 */
float getTemperatureRTC(float* temperature);

/*-------SET FUNCTIONS---------*/

/**
 * @brief set seconds in RTC.
 * 
 * @param writeSeconds uint8_t variable for seconds value to be set in RTC.
 * @return 1 if seconds was set, 0 otherwise.
 */
uint8_t setSecondsRTC(uint8_t writeSeconds);

/**
 * @brief set minutes in RTC.
 * 
 * @param writeMinutes uint8_t variable for minutes value to be set in RTC.
 * @return 1 if minutes was set, 0 otherwise.
 */
uint8_t setMinutesRTC(uint8_t writeMinutes);

/**
 * @brief set hours in RTC.
 * 
 * @param writeHouruint8_t variable for hours value to be set in RTC.
 * @return 1 if hours was set, 0 otherwise.
 */
uint8_t setHourRTC(uint8_t writeHour);

/**
 * @brief set day in RTC.
 * 
 * @param writeDays uint8_t variable for day value to be set in RTC.
 * @return 1 if day was set, 0 otherwise.
 */
uint8_t setDayRTC(uint8_t writeDays);

/**
 * @brief set date in RTC.
 * 
 * @param writeDates uint8_t variable for date value to be set in RTC.
 * @return 1 if date was set, 0 otherwise.
 */
uint8_t setDateRTC(uint8_t writeDates);

/**
 * @brief set month in RTC.
 * 
 * @param writeMonths uint8_t variable for month value to be set in RTC.
 * @return 1 if month was set, 0 otherwise.
 */
uint8_t setMonthRTC(uint8_t writeMonths);

/**
 * @brief set year in RTC.
 * 
 * @param writeYears uint8_t variable for year value to be set in RTC.
 * @return 1 if year was set, 0 otherwise.
 */
uint8_t setYearRTC(uint8_t writeYears);

/**
 * @brief set alarm in RTC.
 * 
 * @param writeAlarmTime struct with alarm time data to be set in RTC
 * @param writeAlarmMode struct with alarm mode data to be set in RTC
 * @param dayOrDate choose mode to set either day or date in alarm along with h,m,s
 * @return 1 if alarm was set, 0 otherwise.
 */
uint8_t setAlarmRTC(rtc_alarm_time_t *writeAlarmTime, rtc_alarm_mode_t *writeAlarmMode, uint8_t dayOrDate);

/**
 * @brief set control mode in RTC.
 * 
 * @param writeControl struct used to set control variables in RTC.
 * @return 1 if control was set, 0 otherwise.
 */
uint8_t setControlRTC(rtc_control_t *writeControl);

/**
 * @brief set status mode in RTC.
 * 
 * @param writeStatus struct used to set status variables in RTC.
 * @return 1 if status was set, 0 otherwise.
 */
uint8_t setStatusRTC(rtc_status_t *writeStatus);

/**
 * @brief set aging offset in RTC.
 * 
 * @param writeAgingOffset int8_t variable used to set aging offset in RTC.
 * @return 1 if year was set, 0 otherwise.
 */
uint8_t setAgingOffsetRTC(int8_t writeAgingOffset);

/*-------UTILITY FUNCTIONS---------*/

/**
 * @brief Converts a two digit decimal number to corresponding binary coded data to write to RTC registers.
 * 
 * @param inputVal 2 digit decimla number to be converted
 * @return the binary coded data.
 */
static uint8_t TwoDigitDecimalToBCD(uint8_t inputVal);

/**
 * @brief Gets a two digit decimal number from corresponding binary coded data from RTC registers.
 * 
 * @param data variable populated with the BCD.
 * @param onesDigitBitMask uint8_t variable used to get the ones digit value.
 * @param tensDigitBitMask uint8_t variable used to get the tens digit value.
 * @return the two digit decimal number.
 */
static uint8_t TwoDigitDecimalFromBCD(uint8_t data, uint8_t onesDigitBitMask, uint8_t tensDigitBitMask);

#endif /* DRIVERS_INCLUDE_DS3232_MZ_H_ */