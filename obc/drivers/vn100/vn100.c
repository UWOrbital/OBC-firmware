#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"
#include "obc_sci_io.h"
#include "obc_vn100.h"

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

static obc_error_code_t serialRequestCMD(vn_cmd_t cmd, unsigned char* packet);
static obc_error_code_t __decodePacket(vn_cmd_t cmd, unsigned char* packet, VN100_decoded_packet_t* parsedPacket);
static obc_error_code_t parsePacket(vn_cmd_t cmd, unsigned char* packet, void* parsedPacket, VN100_error_t* error);

void initVN100(void) {
    /* TODO:
       - Setup vn-100 mutex
       - configure baudrate
       - configure asyncronous outputs
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
  const uint8_t errorCodeIndex = sizeof(errorCodePacket) - 1;
  const errorCode = packet[errorCodeIndex];

  if (!((errorCode <= INSUFFICIENT_BAUD_RATE) || (errorCode == ERROR_BUFFER_OVERFLOW))) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
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
        break;
    case VN_MAG:
        break;
    case VN_ACC:
        break;
    case VN_GYR:
        break;
    case VN_YMR:
        break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  memcpy(&decodedPacket.data, data, packetSize);
  uint16_t checksum = calculateCRC(data, packetSize);

  memcpy(&decodedPacket.crc, data[packetSize], sizeof(decodedPacket.crc));
  if (checksum != decodedPacket.crc) {
    return OBC_ERR_CODE_VN100_PARSE_ERROR;
  }

  *parsedPacket = decodedPacket;
  return OBC_ERR_CODE_SUCCESS;
}
static uint16_t calculateCRC(unsigned char data[], unsigned int length) {
  unsigned int i;
  uint16_t crc = 0;

  for (i = 0; i < length; i++) {
    crc = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data[i];
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0x00ff) << 5;
  }
  return crc;
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

  VN100_decoded_packet_t decodedPacket = {0};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(__decodePacket(cmd, packet, &decodedPacket));

  switch (cmd) {
    case VN_YPR:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_ypr_packet_t));
      break;
    case VN_MAG:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_mag_packet_t));
      break;
    case VN_ACC:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_accel_packet_t));
      break;
    case VN_GYR:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_gyro_packet_t));
      break;
    case VN_YMR:
      memcpy(parsedPacket, &decodedPacket.data, sizeof(vn_ymr_packet_t));
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
}

obc_error_code_t resetModule() {
    unsigned char req [MAX_COMMAND_SIZE] = "$VNRST*4D";
    return sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
}

obc_error_code_t VN100SetBaudrate(uint32_t baudrate) {
    /*  TODO: 
        - Set the baudrate on the VN_100 to the desired baudrate
        - Set the local baudrate to the desired baudrate
    */
    unsigned char req [MAX_COMMAND_SIZE] = ""; 
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
obc_error_code_t requestYPR() {
    // use sciSendByte, to transmit a request command to the VN-100
    char req [MAX_COMMAND_SIZE] = "$VNWRG,75,2,16,01,0029*XX\r\n";
    return sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
}

obc_error_code_t retrieveYPR(vn_ypr_packet_t * packet) {
    /*  TODO:
        - Maybe have this trigger when the packet is ready to come back
        - Setup some kind of flag?
        - Fill in parameters for sciReadBytes
    */
    if (packet == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
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
