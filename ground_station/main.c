#include <stdio.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"

#include <windows.h>

#define COM_PORT_NAME "\\\\.\\COM5"

int main(void) {
    initLogger();

    obc_error_code_t errCode;
    cmd_msg_t cmdMsg = {.id = CMD_EXEC_OBC_RESET, .isTimeTagged = true, .timestamp = 0x12345678UL};
    
    uint8_t buff[24] = {0};
    size_t offset = 0;

    buff[offset++] = 0x7E;

    LOG_IF_ERROR_CODE(packCmdMsg(buff, &offset, &cmdMsg));
    if (errCode != OBC_ERR_CODE_SUCCESS) {
        return 1;
    }

    buff[offset++] = 0x7E;

    for (int i = 0; i < offset; i++) {
        printf("%02x ", buff[i]);
    }
    printf("\n");

    // Declare variables and structures
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
         
    // Open the highest available serial port number
    fprintf(stderr, "Opening serial port...");
    hSerial = CreateFile(
                COM_PORT_NAME, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    
    if (hSerial == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Error\n");
            return 1;
    } else {
        fprintf(stderr, "OK\n");
    }

    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
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
 
    // Send specified text (remaining command line arguments)
    long unsigned int bytes_written = 0;
    fprintf(stderr, "Sending bytes...");
    if (!WriteFile(hSerial, buff, offset, &bytes_written, NULL)) {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }   
    fprintf(stderr, "%lu bytes written\n", bytes_written);
     
    // Close serial port
    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0) {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");

    return 0;
}
