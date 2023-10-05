#include "wsl_uart.h"

#define PORT_PATH "/dev/ttyACM0"

int main() {
  printf("Beginning Demo");
  int serial_fd;
  struct termios serial_settings;

  openSerialPort(&serial_fd, PORT_PATH);

  // Configure the serial port
  memset(&serial_settings, 0, sizeof(serial_settings));
  serial_settings.c_cflag = B115200;  // Baud rate: 115200
  serial_settings.c_cflag |= CS8;     // 8 data bits
  serial_settings.c_cflag |= CREAD;   // Enable receiver
  serial_settings.c_iflag = IGNPAR;   // Ignore parity errors
  serial_settings.c_cflag |= CSTOPB;  // Set 2 stop bits

  setDeviceParameters(serial_fd, &serial_settings);

  // Receive data
  unsigned char buffer[1024];
  while (1) {
    if (readSerialPort(serial_fd, buffer, sizeof(buffer)) == 0) {
      printf("%s", buffer);
    }
  }

  // Close the serial port
  closeSerialPort(serial_fd);
  return 0;
}
