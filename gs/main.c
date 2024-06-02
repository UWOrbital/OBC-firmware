#include "obc_gs_command_pack.h"
#include "obc_gs_command_data.h"
#include "obc_gs_command_id.h"

#include "obc_gs_errors.h"
#include "gs_errors.h"

#include "obc_gs_ax25.h"
#include "obc_gs_fec.h"
#include "obc_gs_aes128.h"
#include "uplink_flow.h"

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

static void printData(uint8_t *data, uint8_t len) {
  for (uint8_t i = 0; i < len; ++i) {
    printf("%x ", data[i]);
  }
  printf("\n");
}

int main(void) {
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
      printf("Error Reading! \r\n");
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
