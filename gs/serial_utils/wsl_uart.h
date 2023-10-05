#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>    // Contains file controls like O_RDWR
#include <errno.h>    // Error integer and strerror() function
#include <termios.h>  // Contains POSIX terminal control definitions
#include <unistd.h>   // write(), read(), close()

int openSerialPort(int *serialFD, char *portPath);

int closeSerialPort(int serialFD);

int getDeviceState(int serialFD, struct termios *serial_settings);

int setDeviceParameters(int serialFD, struct termios *serial_settings);

int setCommTimeouts(int serialFD, int timeouts);

long unsigned int writeSerialPort(int serialFD, uint8_t *buff, size_t len);

int readSerialPort(int serialFD, uint8_t *buff, size_t len);
