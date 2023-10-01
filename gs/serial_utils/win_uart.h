#pragma once

#include <windows.h>

#include <stdint.h>

int openSerialPort(HANDLE *hSerial, char *portName);

int closeSerialPort(HANDLE hSerial);

int getDeviceState(HANDLE hSerial, DCB *dcbSerialParams);

int setDeviceParameters(HANDLE hSerial, DCB *dcbSerialParams);

int setCommTimeouts(HANDLE hSerial, COMMTIMEOUTS *timeouts);

long unsigned int writeSerialPort(HANDLE hSerial, uint8_t *buff, size_t len);

int readSerialPort(HANDLE hSerial, uint8_t *buff, size_t len);
