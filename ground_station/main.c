#include <stdio.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <windows.h>

#define COM_PORT_NAME_PREFIX "\\\\.\\COM"

int main(int argc, char** argv) {
    obc_error_code_t errCode;

    // Grab com port from cmd line args
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <com port>\n", argv[0]);
        return 1;
    }
    
    long int comPort = strtol(argv[1], NULL, 10);
    if (comPort < 0 || comPort > 255) {
        fprintf(stderr, "Invalid com port: %ld\n", comPort);
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
    fprintf(stderr, "Opening serial port...");
    hSerial = CreateFile(
                comPortName, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    
    if (hSerial == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Error\n");
            return 1;
    } else {
        fprintf(stderr, "OK\n");
    }

    // Set device parameters
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0) {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
     
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = TWOSTOPBITS;
    dcbSerialParams.Parity = NOPARITY;
    if (SetCommState(hSerial, &dcbSerialParams) == 0) {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(hSerial, &timeouts) == 0) {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Send packet
    long unsigned int bytesWritten = 0;
    fprintf(stderr, "Sending bytes...");
    if (!WriteFile(hSerial, buff, offset, &bytesWritten, NULL)) {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }   
    fprintf(stderr, "%lu bytes written\n", bytesWritten);
     
    // Close serial port
    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0) {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");

    return 0;
}
