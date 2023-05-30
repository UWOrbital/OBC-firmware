#ifndef GROUND_STATION_SERIAL_UTILS_WIN_UART_H_
#define GROUND_STATION_SERIAL_UTILS_WIN_UART_H_

#include <Windows.h>

#include <stdint.h>

int openSerialPort(HANDLE *hSerial, char *portName);

int closeSerialPort(HANDLE hSerial);

int getDeviceState(HANDLE hSerial, DCB *dcbSerialParams);

int setDeviceParameters(HANDLE hSerial, DCB *dcbSerialParams);

int setCommTimeouts(HANDLE hSerial, COMMTIMEOUTS *timeouts);

long unsigned int writeSerialPort(HANDLE hSerial, uint8_t *buff, size_t len);

int readSerialPort(HANDLE hSerial, uint8_t *buff, size_t len);

#endif /* GROUND_STATION_SERIAL_UTILS_WIN_UART_H_ */
