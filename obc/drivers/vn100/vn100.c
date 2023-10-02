#include "obc_errors.h"
#include "obc_assert.h"
#include "obc_board_config.h"
#include "obc_sci_io.h"
#include "vn100.h"
#include "vn100_parsing.h"

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <os_portmacro.h>
#include <os_semphr.h>
#include <os_task.h>

#include <sci.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define VN100_BAUDRATE 1152000U
#define MAX_COMMAND_SIZE 256U
#define TICK_TIMEOUT portMAX_DELAY
#define DEFAULT_OUTPUT_RATE 20U

/* ---------------------------- Command Byte Sizes ----------------------------------- */
#define YPR_PACKET_SIZE 12U
#define MAG_PACKET_SIZE 12U
#define ACCEL_PACKET_SIZE 12U
#define GYRO_PACKET_SIZE 12U
#define YMR_PACKET_SIZE 48U

/* ---------------------------- Request Commands ------------------------------------- */
#define YPR_REQUEST_CMD "$VNRRG,8*XX\r\n"
#define MAG_REQUEST_CMD "$VNRRG,17*XX\r\n"
#define ACCEL_REQUEST_CMD "$VNRRG,18*XX\r\n"
#define GYRO_REQUEST_CMD "$VNRRG,19*XX\r\n"
#define YMR_REQUEST_CMD "$VNRRG,27*XX\r\n"

/* ---------------------------- ASYNC Configuration Commands -------------------------- */
#define ASYNC_YPR "$VNWRG,06,1*XX\r\n"
#define ASYNC_MAG "$VNWRG,06,10*XX\r\n"
#define ASYNC_ACCEL "$VNWRG,06,11*XX\r\n"
#define ASYNC_GYRO "$VNWRG,06,12*XX\r\n"
#define ASYNC_YMR "$VNWRG,06,14*XX\r\n"

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

static obc_error_code_t isValidBaudRate(uint32_t baudRate);

static obc_error_code_t isValidOutputRate(uint32_t outputRate);

static obc_error_code_t isValidBaudRate(uint32_t baudRate) {
  uint32_t validBaudRate[] = {9600, 19200, 38400, 57600, 115200, 128000, 230400, 460800, 921600};
  uint32_t length = sizeof(validBaudRate) / sizeof(uint32_t);
  for (uint32_t i = 0; i < length; i++) {
    if (baudRate == validBaudRate[i]) {
      return OBC_ERR_CODE_SUCCESS;
    }
  }
  return OBC_ERR_CODE_INVALID_ARG;
}

static obc_error_code_t isValidOutputRate(uint32_t outputRate) {
  uint32_t validOutputRate[] = {1, 2, 4, 5, 10, 20, 25, 40, 50, 100, 200};
  uint32_t length = sizeof(validOutputRate) / sizeof(uint32_t);
  for (uint32_t i = 0; i < length; i++) {
    if (outputRate == validOutputRate[i]) {
      return OBC_ERR_CODE_SUCCESS;
    }
  }
  return OBC_ERR_CODE_INVALID_ARG;
}

void initVN100(void) {
  initSciMutex();
  sciSetBaudrate(UART_VN100_REG, VN100_BAUDRATE);
  /* Configure the asnyc output to output Yaw Pitch Roll, Accelerometer, Angular Rates and Magnetometer readings
     at a fixed output rate of 10Hz */
  setASYNCOutputs(VN_YMR, DEFAULT_OUTPUT_RATE);
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
      unsigned char YPRRequest[] = YPR_REQUEST_CMD;
      numBytesToRead = YPR_PACKET_SIZE;
      RETURN_IF_ERROR_CODE(sciSendBytes(YPRRequest, sizeof(YPRRequest), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_MAG: {
      unsigned char MAGRequest[] = MAG_REQUEST_CMD;
      numBytesToRead = MAG_PACKET_SIZE;
      RETURN_IF_ERROR_CODE(sciSendBytes(MAGRequest, sizeof(MAGRequest), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_ACC: {
      unsigned char ACCELRequest[] = ACCEL_REQUEST_CMD;
      numBytesToRead = ACCEL_PACKET_SIZE;
      RETURN_IF_ERROR_CODE(sciSendBytes(ACCELRequest, sizeof(ACCELRequest), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_GYR: {
      unsigned char GYRORequest[] = GYRO_REQUEST_CMD;
      numBytesToRead = GYRO_PACKET_SIZE;
      RETURN_IF_ERROR_CODE(sciSendBytes(GYRORequest, sizeof(GYRORequest), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_YMR: {
      unsigned char YMRRequest[] = YMR_REQUEST_CMD;
      numBytesToRead = YMR_PACKET_SIZE;
      RETURN_IF_ERROR_CODE(sciSendBytes(YMRRequest, sizeof(YMRRequest), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }

  // Short delay to allow VN-100 to respond
  vTaskDelay(pdMS_TO_TICKS(10));
  RETURN_IF_ERROR_CODE(sciReadBytes(packet, numBytesToRead, TICK_TIMEOUT, pdMS_TO_TICKS(10), UART_VN100_REG));
  return errCode;
}

obc_error_code_t resetModule() {
  /* TODO:
    - Confirm that the right response message is received from the module, otherwise throw an error
  */
  obc_error_code_t errCode;
  unsigned char req[MAX_COMMAND_SIZE] = "$VNRST*4D\r\n";
  RETURN_IF_ERROR_CODE(sciSendBytes(req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t VN100SetBaudrate(uint32_t baudrate) {
  isValidBaudRate(baudrate);

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

obc_error_code_t VN100SetOutputRate(uint32_t outputRate) {
  isValidOutputRate(outputRate);
  // Make the size of the string representation sufficiently large, use memcpy to append string onto req
  char freq[3];

  // Set to XX for now, means to ignore the checksum
  const char checksum[] = "*XX\r\n";
  const char base[] = "$VNWRG,07,";
  unsigned char req[MAX_COMMAND_SIZE];
  snprintf(freq, sizeof(freq), "%ld", outputRate);

  size_t len1 = strlen(base);
  size_t len2 = strlen(freq);
  size_t len3 = strlen(checksum);

  // Begin appending the command
  memcpy(req, base, len1);
  memcpy(req + len1, freq, len2);
  memcpy(req + len1 + len2, checksum, len3);

  size_t numBytes = sizeof(req);

  // Send the message via UART
  sciSendBytes(req, numBytes, TICK_TIMEOUT, UART_VN100_REG);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t retrieveYPR(vn_ypr_packet_t* packet) {
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

  obc_error_code_t errCode;
  unsigned char unparsedPacket[GYRO_PACKET_SIZE];
  RETURN_IF_ERROR_CODE(serialRequestCMD(VN_GYR, unparsedPacket));

  VN100_error_t error;
  RETURN_IF_ERROR_CODE(parsePacket(VN_GYR, unparsedPacket, packet, &error));
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

obc_error_code_t setASYNCOutputs(vn_cmd_t cmd, uint32_t outputRate) {
  obc_error_code_t errCode;
  unsigned char asyncCommand [MAX_COMMAND_SIZE];
  switch (cmd) {
    case VN_YPR: {
      memcpy(asyncCommand, ASYNC_YPR, sizeof(ASYNC_YPR))
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_MAG: {
      memcpy(asyncCommand, ASYNC_MAG, sizeof(ASYNC_MAG));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR), TICK_TIMEOUT, UART_VN100_REG));
      break;
    }
    case VN_ACC:
      memcpy(asyncCommand, ASYNC_ACCEL, sizeof(ASYNC_ACCEL));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR), TICK_TIMEOUT, UART_VN100_REG));
      break;
    case VN_GYR:
      memcpy(asyncCommand, ASYNC_GYRO, sizeof(ASYNC_GYRO));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR), TICK_TIMEOUT, UART_VN100_REG));
      break;
    case VN_YMR:
      memcpy(asyncCommand, ASYNC_YMR, sizeof(ASYNC_YMR));
      RETURN_IF_ERROR_CODE(sciSendBytes(asyncCommand, sizeof(ASYNC_YPR), TICK_TIMEOUT, UART_VN100_REG));
      break;
    default:
      return OBC_ERR_CODE_INVALID_ARG;
  }
  RETURN_IF_ERROR_CODE(VN100SetOutputRate(outputRate));
  return OBC_ERR_CODE_SUCCESS;
}
