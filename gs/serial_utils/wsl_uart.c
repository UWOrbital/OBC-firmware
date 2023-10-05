#include "wsl_uart.h"

#include <stdint.h>

#define SUCCESS 0
#define FAIL -1

int openSerialPort(int *serialFD, char *portAddr) {
  // Note that you may need to change ttyACM0 with whatever device addr appears
  *serialFD = open(portAddr, O_RDWR | O_NOCTTY);
  if (*serialFD == -1) {
    printf("Error opening serial port\r\n");
    return -1;
  }
  return 0;
}

int closeSerialPort(int serialFD) {
  close(serialFD);
  return 0;
}

int getDeviceState(int serialFD, struct termios *serial_settings) {
  if (serial_settings == NULL) {
    printf("INVALID PARAMETERS\r\n");
    return -1;
  }
  if (tcgetattr(serialFD, serial_settings) == -1) {
    printf("Error getting comm settings\r\n");
    close(serialFD);
    return -1;
  }
  return 0;
}

int setDeviceParameters(int serialFD, struct termios *serial_settings) {
  if (serial_settings == NULL) {
    printf("INVALID PARAMETERS\r\n");
    return -1;
  }
  if (tcsetattr(serialFD, TCSANOW, serial_settings) == -1) {
    printf("Error setting comm settings\r\n");
    close(serialFD);
    return -1;
  }
  return 0;
}

int setCommTimeouts(int serialFD, int timeouts) {
  //   // Set read timeout (e.g., 5 seconds)
  // serial_settings.c_cc[VTIME] = 50;

  if (timeouts <= 0) {
    printf("INVALID PARAMETERS\r\n");
    return -1;
  }

  if (tcsendbreak(serialFD, timeouts) == -1) {
    printf("Error sending timeout\r\n");
    close(serialFD);
    return -1;
  }
  return 0;
}

long unsigned int writeSerialPort(int serialFD, uint8_t *buff, size_t len) {
  if (buff == NULL || len <= 0) {
    return -1;
  }

  if (write(serialFD, buff, len) != len) {
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
  return -1;
}
