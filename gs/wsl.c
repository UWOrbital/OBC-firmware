#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main() {
    printf("Beginning Demo");
    int serial_fd;
    struct termios serial_settings;

    // Open the serial port (replace "/dev/ttyS0" with your device)
    serial_fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
    if (serial_fd == -1) {
        perror("Error opening serial port");
        return 1;
    }

    // Configure the serial port
    memset(&serial_settings, 0, sizeof(serial_settings));
    serial_settings.c_cflag = B9600;  // Baud rate: 9600
    serial_settings.c_cflag |= CS8;   // 8 data bits
    serial_settings.c_cflag |= CREAD; // Enable receiver
    serial_settings.c_iflag = IGNPAR; // Ignore parity errors

    // Set 2 stop bits
    serial_settings.c_cflag |= CSTOPB;

    tcsetattr(serial_fd, TCSANOW, &serial_settings);

    // Send data
    char data_to_send[] = "Hello, serial device!\n";
    write(serial_fd, data_to_send, strlen(data_to_send));

    while (1) {
        // Receive data
        char buffer[128];
        int bytes_read = read(serial_fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received data: %s", buffer);
        }
    }
    // Close the serial port
    close(serial_fd);

    return 0;
}
