#include <stdio.h>
#include <stdint.h>

// Example - todo: remove
#include "command_pack.h"
#include "command_data.h"
#include "command_id.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "ax25.h"
#include "fec.h"
#include "aes128.h"
#include "aes.h"

#include "win_uart.h"

#define COM_PORT_NAME_PREFIX "\\\\.\\COM"
#define ARGUMENTS_PER_COMMAND 3

static const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03,
                                                      0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0A, 0x0B,
                                                      0x0C, 0x0D, 0x0E, 0x0F};

static struct AES_ctx ctx;
static correct_reed_solomon* rsGs = NULL;

static obc_error_code_t decodePacket(packed_ax25_packet_t *data, packed_rs_packet_t *rsData, aes_data_t *aesData);

int main(int argc, char *argv[]) {
    obc_error_code_t errCode;

    // Grab com port from cmd line args
    if (argc < 5) {
        printf("Usage: %s <com port> <CMD1 id> <CMD1 isTimeTagged> <CMD1 timetag> <CMD2 id> <CMD2 isTimeTagged> <CMD2 timetag> ...\n", argv[0]);
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
    timeouts.ReadIntervalTimeout = 10000;
    timeouts.ReadTotalTimeoutConstant = 20000;
    timeouts.ReadTotalTimeoutMultiplier = 100;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(hSerial, &timeouts) == 0) {
        printf("Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }

    /* Construct packet */
    cmd_msg_t cmdMsg = {0};

    // Initialize important variables related to packing and queueing the telemetry to be sent
    uint8_t packedSingleCmd[MAX_CMD_MSG_SIZE]; // Holds a serialized version of the current command

    packed_telem_packet_t cmdPacket = {0}; // Holds 223B of "raw" command data.
                                             // Zero initialized because commands of 0 are ignored 
    size_t cmdPacketOffset = 0; // Number of bytes filled in cmdPacket

    packed_rs_packet_t fecPkt; // Holds a 255B RS packet
    packed_ax25_packet_t ax25Pkt; // Holds an AX.25 packet

    AES_init_ctx(&ctx, TEMP_STATIC_KEY);

    uint8_t iv[AES_IV_SIZE];

    memset(iv, 1, AES_IV_SIZE);

    AES_ctx_set_iv(&ctx, iv);

    rsGs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);

    for(uint32_t i = 0; i < (argc - 2)/ARGUMENTS_PER_COMMAND; ++i){
        unsigned long int id = strtoul(argv[ARGUMENTS_PER_COMMAND*i + 2], NULL, 10);
        if(id > 255){
            printf("invalid ID: %lu\n", id);
            return 1;
        }
        cmdMsg.id = (uint8_t) id;
        printf("command id is %lu\n", id);
        if (strcmp(argv[ARGUMENTS_PER_COMMAND*i + 3], "0") == 0) {
            cmdMsg.isTimeTagged = false;
        } else if (strcmp(argv[ARGUMENTS_PER_COMMAND*i + 3], "1") == 0) {
            cmdMsg.isTimeTagged = true;
        } else {
            printf("invalid isTimeTagged value: %s\n", argv[ARGUMENTS_PER_COMMAND*i + 3]);
        }
        cmdMsg.timestamp = (uint32_t) strtoul(argv[ARGUMENTS_PER_COMMAND*i + 4], NULL, 10);

        uint8_t packedSingleCmdSize = 0; // Size of the packed single telemetry
        // Pack the single telemetry into a uint8_t array
        RETURN_IF_ERROR_CODE(packCmdMsg(packedSingleCmd,
                             &cmdPacketOffset,
                             &cmdMsg,
                             &packedSingleCmdSize));
        for (int i = 0; i < packedSingleCmdSize; i++) {
            printf("%02x ", packedSingleCmd[i]);
        }
        printf("\n");
        
        // If the single telemetry is too large to continue adding to the packet, send the packet
        if (cmdPacketOffset + packedSingleCmdSize > RS_DECODED_SIZE-AES_IV_SIZE) {
            // encrypt
            AES_CTR_xcrypt_buffer(&ctx, cmdPacket.data, RS_DECODED_SIZE-AES_IV_SIZE);
            printf("Encrypted: ");
            for (int i = 0; i < RS_DECODED_SIZE-AES_IV_SIZE; i++) {
                printf("%02x ", cmdPacket.data[i]);
            }
            printf("\n");

            // Apply Reed Solomon FEC
            if((uint8_t) correct_reed_solomon_encode(rsGs, cmdPacket.data, RS_DECODED_SIZE, fecPkt.data) < RS_ENCODED_SIZE){
                return 1;
            }
            printf("FEC: ");
            for (int i = 0; i < RS_ENCODED_SIZE; i++) {
                printf("%02x ", fecPkt.data[i]);
            }
            printf("\n");

            // Perform AX.25 framing
            RETURN_IF_ERROR_CODE(ax25Send(&fecPkt, &ax25Pkt, &cubesatCallsign, &groundStationCallsign));

            // transmit the ax25 packet
            printf("Packet: ");
            for (int i = 0; i < AX25_MAXIMUM_PKT_LEN; i++) {
                printf("%x ", cmdPacket.data[i]);
            }
            printf("\n");

            // Send packet
            long unsigned int bytesWritten = 0;

            printf("Sending bytes...");
            if (!WriteFile(hSerial, cmdPacket.data, cmdPacketOffset, &bytesWritten, NULL)) {
                printf("Error\n");
                CloseHandle(hSerial);
                return 1;
            }   
            printf("%lu bytes written\n", bytesWritten);
            // Reset the packedTelem struct and offset
            cmdPacket = (packed_telem_packet_t){0};
            cmdPacketOffset = 0;
        }

        // Copy the telemetry data into the packedTelem struct
        memcpy(&cmdPacket.data[cmdPacketOffset], packedSingleCmd, packedSingleCmdSize);
        cmdPacketOffset += packedSingleCmdSize;
    }
    printf("We made it\n");
    // if there are any bytes left, send them
    if(cmdPacketOffset != 0){
        // encrypt
        AES_CTR_xcrypt_buffer(&ctx, cmdPacket.data, RS_DECODED_SIZE-AES_IV_SIZE);
        uint8_t data[RS_DECODED_SIZE];
        memcpy(data, iv, AES_IV_SIZE);
        memcpy(&data[AES_IV_SIZE], cmdPacket.data, RS_DECODED_SIZE-AES_IV_SIZE);
        printf("Encrypted: ");
        for (int i = 0; i < RS_DECODED_SIZE; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
        // Apply Reed Solomon FEC
        if((uint8_t) correct_reed_solomon_encode(rsGs, data, RS_DECODED_SIZE, fecPkt.data) < RS_ENCODED_SIZE){
            return 1;
        };
        printf("FEC: ");
        for (int i = 0; i < RS_ENCODED_SIZE; i++) {
            printf("%02x ", fecPkt.data[i]);
        }
        printf("\n");
        // Perform AX.25 framing
        RETURN_IF_ERROR_CODE(ax25Send(&fecPkt, &ax25Pkt, &cubesatCallsign, &groundStationCallsign));

        // transmit the ax25 packet
        printf("Packet: ");
        for (int i = 0; i < ax25Pkt.length; i++) {
            printf("%x ", ax25Pkt.data[i]);
        }
        printf("\n");

        // Send packet
        long unsigned int bytesWritten = 0;

        printf("Sending bytes...");
        if (!WriteFile(hSerial, ax25Pkt.data, ax25Pkt.length, &bytesWritten, NULL)) {
            printf("Error\n");
            CloseHandle(hSerial);
            return 1;
        }   
        printf("%lu bytes written\n", bytesWritten);
    }
    // switch to receive
    uint8_t byte = '\0';
    packed_ax25_packet_t axData = {0};
    uint16_t axDataIndex = 0;

    bool startFlagReceived = false;
    while (1) {
        if (readSerialPort(hSerial, &byte, 1) == 0) {
            break;
        }
        printf("%x ", byte);
        if (axDataIndex >= sizeof(axData.data)) {
            LOG_ERROR_CODE(OBC_ERR_CODE_BUFF_OVERFLOW);

            // Restart the decoding process
            memset(&axData, 0, sizeof(axData));
            axDataIndex = 0;
            startFlagReceived = false;
        }

        if (byte == AX25_FLAG) {
            axData.data[axDataIndex++] = byte;

            // Decode packet if we have start flag, end flag, and at least 1 byte of data
            // During idling, multiple AX25_FLAGs may be sent in a row, so we enforce that
            // axData.data[1] must be something other than AX25_FLAG
            if (axDataIndex > 2) {
                axData.length = axDataIndex;
                    
                packed_rs_packet_t rsData = {0};
                aes_data_t aesData = {0};
                LOG_IF_ERROR_CODE(decodePacket(&axData, &rsData, &aesData));
                    
                // Restart the decoding process
                memset(&axData, 0, sizeof(axData));
                axDataIndex = 0;
                startFlagReceived = false;
            } else {
                startFlagReceived = true;
                axDataIndex = 1;
            }
                
            continue;
        }
            
        if (startFlagReceived) {
            axData.data[axDataIndex++] = byte;
        }
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

static obc_error_code_t decodePacket(packed_ax25_packet_t *data, packed_rs_packet_t *rsData, aes_data_t *aesData) {
    obc_error_code_t errCode;
    
    RETURN_IF_ERROR_CODE(ax25Recv(data, rsData, &groundStationCallsign));
    uint8_t decodedLength = correct_reed_solomon_decode(rsGs, rsData->data, RS_ENCODED_SIZE, aesData->rawData);
    
    if(decodedLength == -1)
        return OBC_ERR_CODE_CORRUPTED_MSG;

    uint8_t decryptedData[RS_DECODED_SIZE-AES_IV_SIZE] = {0};
    memcpy(decryptedData, aesData->aesStruct.ciphertext, RS_DECODED_SIZE-AES_IV_SIZE);
    AES_ctx_set_iv(&ctx, aesData->aesStruct.iv);
    AES_CTR_xcrypt_buffer(&ctx, decryptedData, RS_DECODED_SIZE-AES_IV_SIZE);
    
    for(uint8_t i = 0; i < RS_DECODED_SIZE-AES_IV_SIZE; ++i){
        printf("%x ", decryptedData[i]);
    }
    printf("\n");
    return OBC_ERR_CODE_SUCCESS;
}