#include "obc_gs_command_pack.h"
#include "obc_gs_command_unpack.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_id.h"

#include "obc_gs_errors.h"
#include "gs_errors.h"

#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "obc_gs_uplink_flow.h"

#include <cserialport.h>
#include <aes.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                               0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

static uint32_t getCurrentTime(void);

static void printData(uint8_t *data, uint32_t len) {
  printf("{");
  for (uint32_t i = 0; i < len; ++i) {
    printf("0x%x, ", data[i]);
  }
  printf("}\n");
}

void generatePingPacketData(void) {
  // printf("Uplink Flow Demo\n");
  obc_gs_error_code_t gsErrCode;
  cmd_msg_t cmdMsg = {.isTimeTagged = false, .id = CMD_PING};
  uint32_t cmdPacketOffset = 0;

  // printf("Packing Ping command\n");
  uint8_t packedSingleCmdSize = 0;
  uint8_t packedSingleCmd[MAX_CMD_MSG_SIZE] = {0};
  gsErrCode = packCmdMsg(packedSingleCmd, &cmdPacketOffset, &cmdMsg, &packedSingleCmdSize);
  if (gsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
    printf("packCmdMsg returned %d\n", gsErrCode);
    exit(1);
  }
  // printf("Packed command: ");
  // printData(packedSingleCmd, packedSingleCmdSize);
  setCurrentLinkDestAddress(&groundStationCallsign);
  uplink_flow_packet_t packet = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
  memcpy(packet.data, packedSingleCmd,
         packedSingleCmdSize < AES_DECRYPTED_SIZE ? packedSingleCmdSize : AES_DECRYPTED_SIZE);
  // printf("Packet data: ");
  // printData(packet.data, AES_DECRYPTED_SIZE);

  // printf("Encoding packet\n");
  packed_ax25_i_frame_t ax25Data = {0};

  gsErrCode = uplinkEncodePacket(&packet, &ax25Data, TEMP_STATIC_KEY);
  if (gsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
    printf("uplinkEncodePacket returned %d\n", gsErrCode);
    exit(1);
  }
  // printf("Packet data after encoding: ");
  printData(ax25Data.data, ax25Data.length);
  printf("Length of encoded packet: %d\n", ax25Data.length);

  packed_ax25_i_frame_t ax25DataExpect = {
      .data = {0x7e, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
               0xf0, 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xa,  0xb,  0xc,  0xd,  0xe,  0xf,
               0x1d, 0x9d, 0x19, 0xea, 0xdb, 0xa7, 0xd1, 0x87, 0x6c, 0xb9, 0x40, 0x47, 0x9f, 0x4a, 0xe8, 0xc,  0x31,
               0x5e, 0x70, 0xc8, 0x30, 0xa9, 0xcf, 0x1f, 0x11, 0xb1, 0xee, 0xf6, 0x3e, 0xae, 0xb4, 0xe9, 0xf0, 0xee,
               0x75, 0x37, 0xaa, 0x50, 0x34, 0xee, 0x57, 0xcf, 0xb6, 0xd9, 0xa,  0xc8, 0x90, 0x9a, 0x8b, 0x34, 0xfa,
               0x91, 0x33, 0xd4, 0xba, 0xb9, 0xf6, 0xcb, 0x12, 0x14, 0x1b, 0xc4, 0xf1, 0x8,  0x5a, 0xa1, 0x4d, 0x21,
               0x72, 0x88, 0xe2, 0x92, 0x66, 0xe2, 0x56, 0xfa, 0x94, 0x7c, 0x78, 0xe8, 0x2f, 0x36, 0x21, 0xe4, 0x20,
               0x20, 0x15, 0x61, 0xcf, 0x2e, 0xb1, 0xce, 0xf9, 0xbd, 0x76, 0x86, 0xe8, 0x9c, 0x46, 0xea, 0x5d, 0xe2,
               0xea, 0xc,  0xd0, 0x82, 0xb,  0xdc, 0x75, 0x9a, 0xe8, 0x59, 0xba, 0x44, 0xc1, 0x20, 0x3e, 0x3e, 0xe3,
               0x14, 0x6b, 0x14, 0xc2, 0x4b, 0xc1, 0xa1, 0x8f, 0xb6, 0xa2, 0x16, 0x4a, 0x66, 0xa,  0x68, 0x1,  0xe7,
               0xf,  0xbe, 0x6a, 0x9d, 0xd3, 0xb7, 0xd8, 0xee, 0x85, 0xe,  0xbe, 0x44, 0x82, 0xd7, 0x80, 0x81, 0x89,
               0x43, 0x5,  0x3e, 0x76, 0x83, 0xeb, 0x59, 0x7d, 0xb,  0xbd, 0x4,  0xf5, 0xe5, 0x80, 0xc,  0xd8, 0x19,
               0xe,  0x8e, 0x1a, 0x4b, 0x11, 0xdc, 0x6a, 0xa7, 0x87, 0xb8, 0xf9, 0x86, 0x93, 0x55, 0x37, 0x56, 0x28,
               0x98, 0xc4, 0x1,  0xa7, 0xd3, 0x34, 0x9d, 0x15, 0x5a, 0x68, 0x87, 0x23, 0xd0, 0x35, 0xe1, 0xf1, 0x7,
               0xd1, 0x60, 0xbd, 0x2b, 0x7b, 0x7c, 0xa0, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
               0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
               0x0,  0x0,  0x0,  0x0,  0xc,  0x4b, 0x60, 0x7e},
      .length = 280};
  printf("Expected packet data vs actual packet data: %d\n",
         memcmp(ax25DataExpect.data, ax25Data.data, ax25Data.length));

  if (ax25Data.length != ax25DataExpect.length) {
    printf("Length of encoded packet is not equal to expected length\n");
  } else {
    printf("Length of encoded packet is equal to expected length\n");
  }
  if (memcmp(ax25DataExpect.data, ax25Data.data, ax25Data.length) != 0) {
    printf("Encoded packet data is not equal to expected packet data\n");
  } else {
    printf("Encoded packet data is equal to expected packet data\n");
  }

  uplink_flow_packet_t output = {0};
  gsErrCode = (uplinkDecodePacket(&ax25Data, &output));
  if (gsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
    printf("uplinkDecodePacket returned %d\n", gsErrCode);
    exit(1);
  }
  printf("Packet original vs output from encoding %d (0 for true)\n",
         memcmp(packet.data, output.data, AES_DECRYPTED_SIZE));
  // printf("gsErrCode: %d\n", gsErrCode);
  // Unpack the command Message
  cmd_msg_t cmdMsgOutput = {0};
  uint32_t cmdPacketOffsetOutput = 0;
  printData(output.data, AES_DECRYPTED_SIZE);
  gsErrCode = unpackCmdMsg(output.data, &cmdPacketOffsetOutput, &cmdMsgOutput);
  if (gsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
    printf("unpackCmdMsg returned %d\n", gsErrCode);
    exit(1);
  }
  printf("Unpacked command message: ");
  printf("ID: %d, Timestamp: %d, isTimeTagged: %d\n", cmdMsgOutput.id, cmdMsgOutput.timestamp,
         cmdMsgOutput.isTimeTagged);
  printf("cmdPacketOffsetOutput: %d\n", cmdPacketOffsetOutput);
}

int main(void) {
  generatePingPacketData();
  return 0;
  obc_gs_error_code_t obcGsErrCode;

  uint32_t demoNum = 0;

  printf(
      "\n0: Immediate RTC Sync"
      "\n1: Immediate Ping"
      "\n2: Time-tagged Ping (15 sec in future)");

  scanf("%u", &demoNum);

  // Check if the input is within the valid range
  if (demoNum >= 0 && demoNum <= 2) {
    printf("You entered: %d\n", demoNum);
  } else {
    printf("Invalid input. Please enter a number between 0 and 2.\n");
  }

  /* ----------------------------------- Begin CSerialPort setup ------------------------------------------- */

  void *pSerialPort = NULL;

  pSerialPort = CSerialPortMalloc();
  printf("Version: %s\n\n", CSerialPortGetVersion(pSerialPort));

  printf("Available Friendly Ports:\n");

  struct SerialPortInfoArray portInfoArray = {0};
  CSerialPortAvailablePortInfos(&portInfoArray);

  for (unsigned int i = 0; i < portInfoArray.size; ++i) {
    printf("%u - %s %s\n", i + 1, portInfoArray.portInfo[i].portName, portInfoArray.portInfo[i].description);
  }

  if (portInfoArray.size == 0) {
    printf("No Valid Port\n");
  } else {
    printf("\n");

    unsigned int input = 0;
    do {
      printf("Please Input The Index Of Port(1 - %d)\n", portInfoArray.size);

      scanf("%u", &input);

      if (input >= 1 && input <= portInfoArray.size) {
        break;
      }
    } while (1);

    char portName[256] = {0};
    strcpy(portName, portInfoArray.portInfo[input - 1].portName);
    printf("Port Name: %s\n", portName);

    CSerialPortAvailablePortInfosFree(&portInfoArray);

    CSerialPortInit(pSerialPort,
                    portName,    // windows:COM1 Linux:/dev/ttyS0
                    115200,      // baudrate
                    ParityNone,  // parity
                    DataBits8,   // data bit
                    StopTwo,     // stop bit
                    FlowNone,    // flow
                    4096         // read buffer size
    );

    if (demoNum == 2) {
      CSerialPortSetReadIntervalTimeout(pSerialPort, 30000);  // read interval timeout
    } else {
      CSerialPortSetReadIntervalTimeout(pSerialPort, 2500);  // read interval timeout
    }

    CSerialPortOpen(pSerialPort);

    printf("Open %s %s\n", portName, 1 == CSerialPortIsOpen(pSerialPort) ? "Success" : "Failed");
    printf("Code: %d, Message: %s\n", CSerialPortGetLastError(pSerialPort), CSerialPortGetLastErrorMsg(pSerialPort));
  }

  printf("Serial port configured!\n");

  /* ------------------------------------------- Demo Code ----------------------------------------------------- */

  /* Construct packet */

  cmd_msg_t cmdMsg = {0};

  switch (demoNum) {
    case 0: {
      printf("Sending RTC Sync Command\n");
      cmdMsg.id = CMD_RTC_SYNC;
      cmdMsg.isTimeTagged = false;
      cmdMsg.rtcSync.unixTime = getCurrentTime();
      break;
    }

    case 1: {
      printf("Sending immediate ping command\n");
      cmdMsg.id = CMD_PING;
      cmdMsg.isTimeTagged = false;
      break;
    }

    case 2: {
      printf("Sending time-tagged ping command (15 sec in the future)\n");
      cmdMsg.id = CMD_PING;
      cmdMsg.isTimeTagged = true;
      cmdMsg.rtcSync.unixTime = getCurrentTime() + 15;
      break;
    }

    default:
      printf("Invalid demo number!");
      exit(1);
  }

  /* Pack command message */
  uint32_t cmdPacketOffset = 0;

  uint8_t packedSingleCmdSize = 0;
  uint8_t packedSingleCmd[MAX_CMD_MSG_SIZE] = {0};
  if (packCmdMsg(packedSingleCmd, &cmdPacketOffset, &cmdMsg, &packedSingleCmdSize) != OBC_GS_ERR_CODE_SUCCESS) {
    printf("Failed to pack command message!");
    exit(1);
  }

  if (cmdPacketOffset != 0) {
    packed_ax25_i_frame_t ax25Pkt = {0};
    uplink_flow_packet_t command = {.data = {0}, .type = UPLINK_FLOW_DECODED_DATA};
    memcpy(command.data, packedSingleCmd,
           packedSingleCmdSize < AES_DECRYPTED_SIZE ? packedSingleCmdSize : AES_DECRYPTED_SIZE);
    setCurrentLinkDestAddress(&groundStationCallsign);
    obcGsErrCode = uplinkEncodePacket(&command, &ax25Pkt, TEMP_STATIC_KEY);
    if (obcGsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
      printf("Failed to encode packet!");
      exit(1);
    }

    /* Write data to serial port */
    long unsigned int bytesWritten = CSerialPortWriteData(pSerialPort, ax25Pkt.data, ax25Pkt.length);
    if (bytesWritten < ax25Pkt.length) {
      printf("Failed to write entire AX.25 packet!");
      exit(1);
    }

    printf("Bytes Written: %lu\n", bytesWritten);
  }

  /* Receive Data */

  uint16_t axDataIndex = 0;
  packed_ax25_i_frame_t axData = {0};
  bool startFlagReceived = false;

  while (1) {
    uint8_t byte = '\0';
    if (CSerialPortReadData(pSerialPort, &byte, 1) < 0) {
      printf("Error Reading! \n");
      break;
    }

    if (axDataIndex >= sizeof(axData.data)) {
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
        uplink_flow_packet_t command = {0};

        obcGsErrCode = uplinkDecodePacket(&axData, &command);
        if (obcGsErrCode != OBC_GS_ERR_CODE_SUCCESS) {
          printf("Failed to decode packet!");
          exit(1);
        }
        printData(command.data, AES_DECRYPTED_SIZE);

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

  /* ----------------------------- Disconnect from the Serial Port ----------------------------- */

  CSerialPortFree(pSerialPort);
}

static uint32_t getCurrentTime(void) {
  time_t ts = time(NULL);
  if (ts < 0 || ts > UINT32_MAX) {
    printf("Current time not a uint32_t!");
    exit(1);
  }
  return (uint32_t)ts;
}
