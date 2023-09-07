#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_board_config.h"
#include "obc_logging.h"
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
#include <string.h>

#define VN100_BAUDRATE 1152000
#define MAX_COMMAND_SIZE 256U
#define VN100_ERR_CODE_STRING "$VNERR,"
#define TICK_TIMEOUT portMAX_DELAY

/* ---------------------------- Command Byte Sizes ----------------------------------- */
#define YPR_PACKET_SIZE 12U
#define MAG_PACKET_SIZE 12U
#define ACCEL_PACKET_SIZE 12U
#define GYRO_PACKET_SIZE 12U
#define YMR_PACKET_SIZE 48U

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
 * @param packet unparsed packet in string form
 *
 *      VN_YPR      // Get yaw, pitch and roll measurements
 *      VN_MAG      // Get magnetic measurements
 *      VN_ACC      // Get acceleration measurements
 *      VN_GYR      // Get angular rate measurements
 *      VN_YMR      // Get all of the above
 *
 * @return OBC_ERR_CODE_SUCCESS on success, else an error code
 */

static obc_error_code_t serialRequestCMD(vn_cmd_t cmd, unsigned char* packet);
static obc_error_code_t __decodePacket(vn_cmd_t cmd, unsigned char* packet, VN100_decoded_packet_t* parsedPacket);
static obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket, VN100_error_t* error);

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

obc_error_code_t serialRequestCMD(vn_cmd_t cmd, unsigned char* packet) {
  obc_error_code_t errCode;
  size_t numBytesToRead;
  // Send a request command to the IMU depending on what packet we want to retrieve
  // Need to make this more generalized, currently kinda looks sucky
  switch (cmd) {
    case VN_YPR: {
      unsigned char YPRRequest[] = "$VNRRG,8*XX\r\n";
      numBytesToRead = YPR_PACKET_SIZE;
      errCode = sciSendBytes(YPRRequest, sizeof(YPRRequest), TICK_TIMEOUT, UART_VN100_REG);
      break;
    }
    case VN_MAG: {
      unsigned char MAGRequest[] = "$VNRRG,17*XX\r\n";
      numBytesToRead = MAG_PACKET_SIZE;
      errCode = sciSendBytes(MAGRequest, sizeof(MAGRequest), TICK_TIMEOUT, UART_VN100_REG);
      break;
    }
    case VN_ACC: {
      unsigned char ACCELRequest[] = "$VNRRG,18*XX\r\n";
      numBytesToRead = ACCEL_PACKET_SIZE;
      errCode = sciSendBytes(ACCELRequest, sizeof(ACCELRequest), TICK_TIMEOUT, UART_VN100_REG);
      break;
    }
    case VN_GYR: {
      unsigned char GYRORequest[] = "$VNRRG,19*XX\r\n";
      numBytesToRead = GYRO_PACKET_SIZE;
      errCode = sciSendBytes(GYRORequest, sizeof(GYRORequest), TICK_TIMEOUT, UART_VN100_REG);
      break;
    }
    case VN_YMR: {
      unsigned char YMRRequest[] = "$VNRRG,27*XX\r\n";
      numBytesToRead = YMR_PACKET_SIZE;
      errCode = sciSendBytes(YMRRequest, sizeof(YMRRequest), TICK_TIMEOUT, UART_VN100_REG);
      break;
    }
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  unsigned char received[MAX_COMMAND_SIZE];

  sciReadBytes(received, numBytesToRead, TICK_TIMEOUT, pdMS_TO_TICKS(10));

  /* TODO:
      - Add sciReadBytes with the appropriate bytes to read
      - error checking
  */
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

obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket, VN100_error_t* error) {
  if (packet == NULL || parsedPacket == NULL) return OBC_ERR_CODE_INVALID_ARG;

  /* Parsing for error */
  char errorCodePacket[] = VN100_ERR_CODE_STRING;
  if (!memcmp(packet, errorCodePacket, sizeof(errorCodePacket) - 1)) {
    obc_error_code_t errCode = 0;
    RETURN_IF_ERROR_CODE(recoverErrorCodeFromPacket(packet, error));
    return OBC_ERR_CODE_VN100_RESPONSE_ERROR;
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
  sciSendBytes(req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t VN100SetBaudrate(uint32_t baudrate) {
  /*  TODO:
      - Set the baudrate on the VN_100 to the desired baudrate
        - Figure out how to configure command to append the inputted baudrate
      - Set the local baudrate to the desired baudrate
      - Check if the inputted baudrate is acceptable
  */

  // Make the size of the string representation sufficiently large, use memcpy to append string onto req
  char baud[7];

  // Set to XX for now, means to ignore the checksum
  const char checksum[] = "*XX\r\n";
  const char base[] = "$VNWRG,05,";
  unsigned char req[MAX_COMMAND_SIZE];
  snprintf(baud, sizeof(baud), "%ld", baudrate);

  size_t len1 = strlen(base);
  size_t len2 = strlen(baud);
  size_t len3 = strlen(checksum);

  // Begin appending the command
  memcpy(req, base, len1);
  memcpy(req + len1, baud, len2);
  memcpy(req + len1 + len2, checksum, len3);

  size_t numBytes = sizeof(req);

  // Send the message via UART
  sciSendBytes(req, numBytes, TICK_TIMEOUT, UART_VN100_REG);
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
  obc_error_code_t errCode;
  unsigned char unparsedPacket[YPR_PACKET_SIZE];
  RETURN_IF_ERROR_CODE(serialRequestCMD(VN_YPR, unparsedPacket));

  VN100_error_t error;
  RETURN_IF_ERROR_CODE(parsePacket(VN_YPR, unparsedPacket, packet, &error));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveMAG(vn_mag_packet_t* packet) {
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;
  unsigned char unparsedPacket[MAG_PACKET_SIZE];
  RETURN_IF_ERROR_CODE(serialRequestCMD(VN_MAG, unparsedPacket));

  VN100_error_t error;
  RETURN_IF_ERROR_CODE(parsePacket(VN_MAG, unparsedPacket, packet, &error));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveACCEL(vn_accel_packet_t* packet) {
  if (packet == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  obc_error_code_t errCode;
  unsigned char unparsedPacket[ACCEL_PACKET_SIZE];
  RETURN_IF_ERROR_CODE(serialRequestCMD(VN_ACC, unparsedPacket));
  VN100_error_t error;
  RETURN_IF_ERROR_CODE(parsePacket(VN_ACC, unparsedPacket, packet, &error));
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
  obc_error_code_t errCode;
  unsigned char unparsedPacket[YPR_PACKET_SIZE];
  RETURN_IF_ERROR_CODE(serialRequestCMD(VN_YMR, unparsedPacket));

  VN100_error_t error;
  RETURN_IF_ERROR_CODE(parsePacket(VN_YMR, unparsedPacket, packet, &error));
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
      // unsigned char req [MAX_COMMAND_SIZE] = "$VNWRG,75,2,16,01,0009*XX\r\n";
      // sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
      break;
    default:
      break;
  }
  return OBC_ERR_CODE_SUCCESS;
}
