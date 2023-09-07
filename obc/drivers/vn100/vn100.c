#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_board_config.h"
#include "obc_sci_io.h"
#include "vn100.h"

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sci.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define VN100_BAUDRATE 1152000
#define MAX_COMMAND_SIZE 256U
#define VN100_ERR_CODE_STRING "$VNERR,"
#define TICK_TIMEOUT portMAX_DELAY

#define VALID_RESPONSE_STRING "$VNWRG,75"

static const uint8_t PAYLOAD_OFFSET = sizeof(VALID_RESPONSE_STRING);
static const uint16_t MAX_PAYLOAD_SIZE = sizeof(vn_ymr_packet_t);

typedef enum {
  HARD_FAULT = 1,
  SERIAL_BUFFER_OVERFLOW = 2,
  INVALID_CHECKSUM = 3,
  INVALID_COMMAND = 4,
  NOT_ENOUGH_PARAMETERS = 5,
  TOO_MANY_PARAMETERS = 6,
  INVALID_PARAMETER = 7,
  INVALID_REGISTER = 8,
  UNATHORIZED_ACCESS = 9,
  WATCHDOG_RESET = 10,
  OUTPUT_BUFFER_OVERFLOW = 11,
  INSUFFICIENT_BAUD_RATE = 12,
  ERROR_BUFFER_OVERFLOW = 255
} VN100_error_t;

typedef struct {
  uint8_t groups;
  uint16_t groupField;
} VN100_header_t;

typedef struct {
  float payload[MAX_PAYLOAD_SIZE];
} VN100_payload_t;

typedef struct {
  VN100_header_t header;
  VN100_payload_t data;
  uint16_t crc;
} VN100_decoded_packet_t;

/**
 * @brief Generalized request function to handle mulitple different packet types
 * @param cmd the input command to determine what packet to request
 *
 *      VN_YPR      // Get yaw, pitch and roll measurements
 *      VN_MAG      // Get magnetic measurements
 *      VN_ACC      // Get acceleration measurements
 *      VN_GYR      // Get angular rate measurements
 *      VN_YMR      // Get all of the above
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */

static obc_error_code_t serialRequestCMD(vn_cmd_t cmd);

static obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket);

void initVN100(void) {
  sciSetBaudrate(UART_VN100_REG, VN100_BAUDRATE);
  /* TODO:
     - Setup vn-100 mutex
     - setup the configuration register
     - configure baudrate
     - configure asyncronous outputs
     - Add any offsets to gyro or acceleration or additional info about velocity compensation
  */
}

obc_error_code_t serialRequestCMD(vn_cmd_t cmd) {
  /* TODO:
     - Do some error checking in case the imu returns an error code
     -
  */
  obc_error_code_t errCode;

  switch (cmd) {
    case VN_YPR:
      char req[MAX_COMMAND_SIZE] = "$VNRRG,8*XX\r\n";
      errCode = sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
    case VN_MAG:
      char req[MAX_COMMAND_SIZE] = "$VNRRG,17*XX\r\n";
      errCode = sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
      break;
    case VN_ACC:
      char req[MAX_COMMAND_SIZE] = "$VNRRG,18*XX\r\n";
      errCode = sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
      break;
    case VN_GYR:
      char req[MAX_COMMAND_SIZE] = "$VNRRG,19*XX\r\n";
      errCode = sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
      break;
    case VN_YMR:
      char req[MAX_COMMAND_SIZE] = "$VNRRG,27*XX\r\n";
      errCode = sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
      break;
    default:
      errCode = OBC_ERR_CODE_INVALID_ARG;
      break;
  }
  return errCode;
}

obc_error_code_t recoverErrorCodeFromPacket(unsigned char* packet, VN100_error_t* error) {
  if (packet == NULL || error == NULL) return OBC_ERR_CODE_INVALID_ARG;

  char errorCodePacket[] = VN100_ERR_CODE_STRING;
  if (!memcmp(packet, errorCodePacket, sizeof(errorCodePacket) - 1)) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  const uint8_t errorCodeIndex = sizeof(errorCodePacket) - 1;
  const errorCode = packet[errorCodeIndex];

  if (!((errorCode <= INSUFFICIENT_BAUD_RATE) || (errorCode == ERROR_BUFFER_OVERFLOW))) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  *error = errorCode;
  return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t __decodePacket(vn_cmd_t cmd, unsigned char* packet, VN100_decoded_packet_t* parsedPacket) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  unsigned char* payload = packet[PAYLOAD_OFFSET];  // The main payload

  VN100_decoded_packet_t decodedPacket = {0};
  memcpy(&decodedPacket.header, payload, sizeof(decodedPacket.header));

  unsigned char* data = payload[sizeof(decodedPacket.header)];
  uint16_t packetSize = 0;
  switch (cmd) {
    case VN_YPR:
      packetSize = sizeof(vn_ypr_packet_t);
      break;
    case VN_MAG:
      packetSize = sizeof(vn_mag_packet_t);
      break;
    case VN_ACC:
      packetSize = sizeof(vn_accel_packet_t);
      break;
    case VN_GYR:
      packetSize = sizeof(vn_gyro_packet_t);
      break;
    case VN_YMR:
      packetSize = sizeof(vn_ymr_packet_t);
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  memcpy(&decodedPacket.data, data, packetSize);
  memcpy(&decodedPacket.crc, data[packetSize], sizeof(decodedPacket.crc));
  *parsedPacket = decodedPacket;

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  /* Parsing for error */
  char errorCodePacket[] = VN100_ERR_CODE_STRING;
  if (!memcmp(packet, errorCodePacket, sizeof(errorCodePacket) - 1)) {
    VN100_error_t error = 0;
    obc_error_code_t errCode = 0;
    RETURN_IF_ERROR_CODE(recoverErrorCodeFromPacket(packet, &error));
  }

  VN100_decoded_packet_t packet = {0};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(__decodePacket(cmd, packet, &parsedPacket));
}

obc_error_code_t resetModule() {
  /* TODO:
    - Confirm that the right response message is received from the module, otherwise throw an error
  */
  unsigned char req[MAX_COMMAND_SIZE] = "$VNRST*4D\r\n";
  sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t VN100SetBaudrate(uint32_t baudrate) {
  /*  TODO:
      - Set the baudrate on the VN_100 to the desired baudrate
        - Figure out how to configure command to append the inputted baudrate
      - Set the local baudrate to the desired baudrate
      - Check if the inputted baudrate is acceptable
  */

  // Make the size of the string representation sufficiently large
  unsigned char baud[12] = {0};
  // Set to XX for now, means to ignore the checksum
  unsigned char checksum[3] = "*XX\r\n";
  unsigned char req[MAX_COMMAND_SIZE] = "$VNWRG,05,";
  snprintf(baud, sizeof(baud), "%d", baudrate);
  // Concatenate baudrate and checksum to the end of request command
  strcat(req, baud);
  strcat(req, checksum);

  // Send the message via UART
  sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
  sciSetBaudrate(UART_VN100_REG, baudrate);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveYPR(vn_ypr_packet_t* packet) {
  /*  TODO:
      - Call serialRequest for YPR
      - Wait until flag has set and the response packet is ready to be retrieved
  */
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  serialRequestCMD(VN_YPR);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveMAG(vn_mag_packet_t* packet) {
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  serialRequestCMD(VN_MAG);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveACCEL(vn_accel_packet_t* packet) {
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  serialRequestCMD(VN_ACC);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveGYRO(vn_gyro_packet_t* packet) {
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  serialRequestCMD(VN_GYR);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveYMR(vn_ymr_packet_t* packet) {
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  serialRequestCMD(VN_YMR);
  return OBC_ERR_CODE_SUCCESS;
}
obc_error_code_t setASYNCOutputs(vn_cmd_t cmd) {
  /* TODO:
     - Let the user choose from the defined packet types to configure the asyncronous output register with a particular
     packet type
     - See Example Case 1 in section 4.2.4 of the user manual
  */

  switch (cmd) {
    case VN_YPR:
      char req[MAX_COMMAND_SIZE] = "$VNWRG,75,2,16,01,0009*XX\r\n";
      sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
      break;
    default:
      break;
  }
  return OBC_ERR_CODE_SUCCESS;
}
