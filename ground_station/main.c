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

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define COM_PORT_NAME_PREFIX "\\\\.\\COM"

/* THIS GROUND STATION IS FOR DEMO PURPOSES */
/* AND ONLY SENDS A SINGLE COMMAND PER PACKET */

static const uint8_t TEMP_STATIC_KEY[AES_KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

static correct_reed_solomon *rsGs;

static obc_error_code_t decodePacket(packed_ax25_i_frame_t *data, packed_rs_packet_t *rsData);
static uint32_t getCurrentTime(void);

int main(int argc, char *argv[]) {
  obc_error_code_t errCode;

  initLogger();

  if (argc < 3) {
    printf(
        "Usage: %s <COM port> <Demo Number>"
        "\n0: Immediate RTC Sync"
        "\n1: Immediate Ping"
        "\n2: Time-tagged Ping (15 sec in future)"
        "\n",
        argv[0]);
    return 1;
  }

  long int comPort = strtol(argv[1], NULL, 10);
  if (comPort < 0 || comPort > 255) {
    printf("Invalid com port: %ld\n", comPort);
    return 1;
  }

  int demoNum = strtol(argv[2], NULL, 10);

  /* Setup the serial port */

  char comPortName[16] = {0};
  snprintf(comPortName, sizeof(comPortName), "%s%ld", COM_PORT_NAME_PREFIX, comPort);

  // Declare variables and structures
  HANDLE hSerial;
  if (openSerialPort(&hSerial, comPortName) != 0) {
    printf("Failed to open serial port!");
    exit(1);
  }

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (getDeviceState(hSerial, &dcbSerialParams) != 0) {
    printf("Failed to get device state!");
    return 1;
  }

  dcbSerialParams.BaudRate = CBR_9600;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = TWOSTOPBITS;
  dcbSerialParams.Parity = NOPARITY;

  if (setDeviceParameters(hSerial, &dcbSerialParams) != 0) {
    printf("Failed to set serial parameters!");
    exit(1);
  }

  COMMTIMEOUTS timeouts = {0};

  if (demoNum == 2) {
    timeouts.ReadIntervalTimeout = 30000;  // ms
  } else {
    timeouts.ReadIntervalTimeout = 2500;  // ms
  }

  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  if (SetCommTimeouts(hSerial, &timeouts) == 0) {
    printf("Error setting timeouts!");
    CloseHandle(hSerial);
    return 1;
  }

  printf("Serial port configured!\n");

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

  struct AES_ctx ctx = {0};
  AES_init_ctx(&ctx, TEMP_STATIC_KEY);

  uint8_t iv[AES_IV_SIZE] = {0};
  memset(iv, 1, AES_IV_SIZE);
  AES_ctx_set_iv(&ctx, iv);

  rsGs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 1, 1, 32);

  uint32_t cmdPacketOffset = 0;

  uint8_t packedSingleCmdSize = 0;
  uint8_t packedSingleCmd[MAX_CMD_MSG_SIZE] = {0};
  RETURN_IF_ERROR_CODE(packCmdMsg(packedSingleCmd, &cmdPacketOffset, &cmdMsg, &packedSingleCmdSize));

  uint8_t encryptedCmd[RS_DECODED_SIZE] = {0};

  memcpy(encryptedCmd + AES_IV_SIZE, packedSingleCmd, packedSingleCmdSize);

  AES_CTR_xcrypt_buffer(&ctx, encryptedCmd + AES_IV_SIZE, AES_DECRYPTED_SIZE);

  memcpy(encryptedCmd, iv, AES_IV_SIZE);

  if (cmdPacketOffset != 0) {
    packed_ax25_i_frame_t ax25Pkt = {0};
    unstuffed_ax25_i_frame_t unstuffedAx25Pkt = {0};

    // Perform AX.25 framing
    RETURN_IF_ERROR_CODE(ax25SendIFrame(encryptedCmd, RS_DECODED_SIZE, &unstuffedAx25Pkt, &groundStationCallsign));

    packed_rs_packet_t fecPkt = {0};

    // Apply Reed Solomon FEC
    if ((uint8_t)correct_reed_solomon_encode(rsGs, unstuffedAx25Pkt.data + AX25_INFO_FIELD_POSITION, RS_DECODED_SIZE,
                                             fecPkt.data) < RS_ENCODED_SIZE) {
      exit(1);
    };

    memcpy(unstuffedAx25Pkt.data + AX25_INFO_FIELD_POSITION, fecPkt.data, RS_ENCODED_SIZE);

    RETURN_IF_ERROR_CODE(ax25Stuff(unstuffedAx25Pkt.data, unstuffedAx25Pkt.length, ax25Pkt.data, &ax25Pkt.length));

    ax25Pkt.data[0] = AX25_FLAG;
    ax25Pkt.data[ax25Pkt.length - 1] = AX25_FLAG;

    long unsigned int bytesWritten = writeSerialPort(hSerial, ax25Pkt.data, ax25Pkt.length);
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
    if (readSerialPort(hSerial, &byte, 1) == 0) {
      break;
    }

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
        LOG_IF_ERROR_CODE(decodePacket(&axData, &rsData));

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

  if (closeSerialPort(hSerial) != 0) {
    printf("Failed to close serial port!");
    exit(1);
  }

  return 0;
}

static obc_error_code_t decodePacket(packed_ax25_i_frame_t *ax25Data, packed_rs_packet_t *rsData) {
  obc_error_code_t errCode;
  // perform bit unstuffing
  unstuffed_ax25_i_frame_t unstuffedPacket = {0};
  RETURN_IF_ERROR_CODE(ax25Unstuff(ax25Data->data, ax25Data->length, unstuffedPacket.data, &unstuffedPacket.length));
  if (unstuffedPacket.length == AX25_MINIMUM_I_FRAME_LEN) {
    // copy the unstuffed data into rsData
    memcpy(rsData->data, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_ENCODED_SIZE);
    // clear the info field of the unstuffed packet
    memset(unstuffedPacket.data + AX25_INFO_FIELD_POSITION, 0, RS_ENCODED_SIZE);
    // decode the info field and store it in the unstuffed packet
    uint8_t decodedLength = correct_reed_solomon_decode(rsGs, rsData->data, RS_ENCODED_SIZE,
                                                        unstuffedPacket.data + AX25_INFO_FIELD_POSITION);

    if (decodedLength == -1) {
      return OBC_ERR_CODE_CORRUPTED_MSG;
    }
  }

  // check for a valid ax25 frame and perform the command response if necessary
  RETURN_IF_ERROR_CODE(ax25Recv(&unstuffedPacket));

  uint8_t decodedData[RS_DECODED_SIZE] = {0};
  memcpy(decodedData, unstuffedPacket.data + AX25_INFO_FIELD_POSITION, RS_DECODED_SIZE);

  printf("Received (and decoded) data: ");
  for (uint8_t i = 0; i < RS_DECODED_SIZE; ++i) {
    printf("%x ", decodedData[i]);
  }
  printf("\n");
  return OBC_ERR_CODE_SUCCESS;
}

static uint32_t getCurrentTime(void) {
  time_t ts = time(NULL);
  if (ts < 0 || ts > UINT32_MAX) {
    printf("Current time not a uint32_t!");
    exit(1);
  }
  return (uint32_t)ts;
}
