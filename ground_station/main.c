#include <stdio.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include "win_uart.h"

#define COM_PORT_NAME_PREFIX "\\\\.\\COM"

int main(int argc, char** argv) {
    obc_error_code_t errCode;

    // Grab com port from cmd line args
    if (argc < 2) {
        printf("Usage: %s <com port>\n", argv[0]);
        return 1;
    }
    
    long int comPort = strtol(argv[1], NULL, 10);
    if (comPort < 0 || comPort > 255) {
        printf("Invalid com port: %ld\n", comPort);
        return 1;
    }

    char comPortName[16] = {0};
    snprintf(comPortName, sizeof(comPortName), "%s%ld", COM_PORT_NAME_PREFIX, comPort);

    initLogger();

    /* Construct packet */

    cmd_msg_t cmdMsg = {.id = CMD_EXEC_OBC_RESET, .isTimeTagged = true, .timestamp = 0x12345678UL};
    
    uint8_t buff[24] = {0};
    size_t offset = 0;

    // AX25 start flag
    buff[offset++] = 0x7E;

    LOG_IF_ERROR_CODE(packCmdMsg(buff, &offset, &cmdMsg));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        return 1;
    }

    // AX25 end flag
    buff[offset++] = 0x7E;

    printf("Packet: ");
    for (int i = 0; i < offset; i++) {
        printf("%02x ", buff[i]);
    }
    printf("\n");

    // Declare variables and structures
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
         
    // Open the serial port
    printf("Opening serial port...");
    hSerial = CreateFile(
                comPortName, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    
    if (hSerial == INVALID_HANDLE_VALUE) {
            printf("Error\n");
            return 1;
    } else {
        printf("OK\n");
    }

    // Set device parameters
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0) {
        printf("Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
     
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = TWOSTOPBITS;
    dcbSerialParams.Parity = NOPARITY;
    if (SetCommState(hSerial, &dcbSerialParams) == 0) {
        printf("Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 500;
    timeouts.ReadTotalTimeoutConstant = 500;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(hSerial, &timeouts) == 0) {
        printf("Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Send packet
    long unsigned int bytesWritten = 0;
    printf("Sending bytes...");
    if (!WriteFile(hSerial, buff, offset, &bytesWritten, NULL)) {
        printf("Error\n");
        CloseHandle(hSerial);
        return 1;
    }   
    printf("%lu bytes written\n", bytesWritten);

    unsigned char rxChar = '\0';
    while (1) {
        if (readSerialPort(hSerial, (uint8_t *)&rxChar, 1) == 0) {
            break;
        }
        printf("%c", rxChar);
    }
    printf("\n");

     
    // Close serial port
    printf("Closing serial port...");
    if (CloseHandle(hSerial) == 0) {
        printf("Error\n");
        return 1;
    }
    printf("OK\n");

    return 0;
}
