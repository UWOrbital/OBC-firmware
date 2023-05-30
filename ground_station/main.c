#include <stdio.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "ax25.h"
#include "fec.h"
#include "aes128.h"

#include <windows.h>

#define ARGUMENTS_PER_COMMAND 3

#define COM_PORT_NAME "\\\\.\\COM5"

int main(int argc, char *argv[]) {
    obc_error_code_t errCode;

    initLogger();

    // Declare variables and structures
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
         
    // Open the serial port
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

    /* Construct packet */
    cmd_msg_t cmdMsg;

    // Initialize important variables related to packing and queueing the telemetry to be sent
    uint8_t packedSingleCmd[MAX_CMD_SIZE]; // Holds a serialized version of the current command

    packed_telem_packet_t cmdPacket = {0}; // Holds 223B of "raw" command data.
                                             // Zero initialized because commands of 0 are ignored 
    size_t cmdPacketOffset = 0; // Number of bytes filled in cmdPacket

    packed_rs_packet_t fecPkt; // Holds a 255B RS packet
    packed_ax25_packet_t ax25Pkt; // Holds an AX.25 packet

    for(uint32_t i = 0; i < (argc - 1)/ARGUMENTS_PER_COMMAND; ++i){
        /* do stuff to get the cmdMsg */
        printf("Sending telemetry: %u", cmdMsg.id);

        uint8_t packedSingleCmdSize = 0; // Size of the packed single telemetry
        // Pack the single telemetry into a uint8_t array
        RETURN_IF_ERROR_CODE(packCmdMsg(packedSingleCmd,
                             &cmdPacketOffset,
                             &cmdMsg,
                             &packedSingleCmdSize));
        
        // If the single telemetry is too large to continue adding to the packet, send the packet
        if (cmdPacketOffset + packedSingleCmdSize > RS_DECODED_SIZE-AES_IV_SIZE) {
            /* TODO: implement AES128 encryption */
            // Apply Reed Solomon FEC
            RETURN_IF_ERROR_CODE(rsEncode(&cmdPacket, &fecPkt));

            // Perform AX.25 framing
            RETURN_IF_ERROR_CODE(ax25Send(&fecPkt, &ax25Pkt, &cubesatCallsign, &groundStationCallsign));

            // transmit the ax25 packet
            printf("Packet: ");
            for (int i = 0; i < cmdPacketOffset; i++) {
                printf("%02x ", cmdPacket.data[i]);
            }
            printf("\n");

            // Send packet
            long unsigned int bytesWritten = 0;

            fprintf(stderr, "Sending bytes...");
            if (!WriteFile(hSerial, cmdPacket.data, cmdPacketOffset, &bytesWritten, NULL)) {
                fprintf(stderr, "Error\n");
                CloseHandle(hSerial);
                return 1;
            }   
            fprintf(stderr, "%lu bytes written\n", bytesWritten);
            // Reset the packedTelem struct and offset
            cmdPacket = (packed_telem_packet_t){0};
            cmdPacketOffset = 0;
        }

        // Copy the telemetry data into the packedTelem struct
        memcpy(&cmdPacket.data[cmdPacketOffset], packedSingleCmd, packedSingleCmdSize);
        cmdPacketOffset += packedSingleCmdSize;
    }
    
    // if there are any bytes left, send them
    if(cmdPacketOffset != 0){
        // Apply Reed Solomon FEC
        RETURN_IF_ERROR_CODE(rsEncode(&cmdPacket, &fecPkt));

        // Perform AX.25 framing
        RETURN_IF_ERROR_CODE(ax25Send(&fecPkt, &ax25Pkt, &cubesatCallsign, &groundStationCallsign));

        // transmit the ax25 packet
        printf("Packet: ");
        for (int i = 0; i < cmdPacketOffset; i++) {
            printf("%02x ", cmdPacket.data[i]);
        }
        printf("\n");

        // Send packet
        long unsigned int bytesWritten = 0;

        fprintf(stderr, "Sending bytes...");
        if (!WriteFile(hSerial, cmdPacket.data, cmdPacketOffset, &bytesWritten, NULL)) {
            fprintf(stderr, "Error\n");
            CloseHandle(hSerial);
            return 1;
        }   
        fprintf(stderr, "%lu bytes written\n", bytesWritten);
    }
     
    // Close serial port
    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0) {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");

    // switch to receive 

    return 0;
}
