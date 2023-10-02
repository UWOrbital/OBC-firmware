#include "wsl_uart.h"

#include <stdint.h>

#define SUCCESS 0
#define FAIL -1

int openSerialPort(int serialFD, char *portName) {
    // Open the serial port (replace "/dev/ttyS0" with your device)
    serialFD = open("/dev/USBS0", O_RDWR | O_NOCTTY);
    if (serialFD == -1) {
        perror("Error opening serial port");
        return -1;
    }
    return 0;
}

int closeSerialPort(int serialFD) {
    close(serialFD);
    return 0;
}

int getDeviceState(int serialFD, struct termios *serial_settings) {
    if (serial_settings == NULL){
        perror("INVALID PARAMETERS");
        return -1;
    }
    if (tcgetattr(serialFD, serial_settings) == -1) {
        perror("Error getting comm settings");
        close(serialFD);
        return -1;
    }
    return 0;
}

int setDeviceParameters(int serialFD, struct termios *serial_settings) {
    if (serial_settings == NULL){
        perror("INVALID PARAMETERS");
        return -1;
    }
    if(tcsetattr(serialFD, TCSANOW, serial_settings) == -1 ) {
        perror("Error setting comm settings");
        close(serialFD);
        return -1;
    }
    return 0;
}

int setCommTimeouts(int serialFD, int timeouts) {
    //   // Set read timeout (e.g., 5 seconds)
    // serial_settings.c_cc[VTIME] = 50; 
    
    if (timeouts <= 0) {
        perror("INVALID PARAMETERS");
        return -1;
    }

    if(tcsendbreak(serialFD, timeouts) == -1) {
        perror("Error sending timeout");
        close(serialFD);
        return -1;
    }
    return 0;
}

long unsigned int writeSerialPort(int serialFD, uint8_t *buff, size_t len) {
    if (buff == NULL || len <= 0) {
        return -1;
    }

    if(write(serialFD, buff, len) != len) {
        return -1;
    }
    return 0;
}

int readSerialPort(int serialFD, uint8_t *buff, size_t len) {
    int bytes_read = read(serialFD, buff, len);
    if (bytes_read > 0) {
        buff[bytes_read] = '\0';
        return 0;
    }
    perror("Error reading from serial port");
    return -1;
}