#include "obc_errors.h"
#include "obc_board_config.h"
#include "obc_logging.h"
#include "obc_sci_io.h"

#include "vn100_binary.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* -------------------------------------- Other relevant packet info -------------------------- */
#define DEFAULT_BINARY_OUTPUT_RATE_HZ 10U
#define MAX_OUTPUT_RATE_SIZE 3U
#define MAX_SEND_SIZE 120U

obc_error_code_t startBinaryOutputs(void) {
  /* Outputs: Yaw Pitch Roll, Angular rates, Accelerometer data, Magnetometer, Temp and Pressure. */
  // unsigned char buf[] = "$VNWRG,75,2,80,01,0528*XX\r\n";

  uint8_t outputRate = 800 / DEFAULT_BINARY_OUTPUT_RATE_HZ;  // See section

  const char header[] = "$VNWRG,75,2,";
  char outputRateString[MAX_OUTPUT_RATE_SIZE];  // Sufficient space for an int
  const char checksum[] = ",01,0528*XX\r\n";

  snprintf(outputRateString, sizeof(outputRateString), "%d", outputRate);

  size_t headerLength = strlen(header);
  size_t outputRateLength = strlen(outputRateString);
  size_t checksumLength = strlen(checksum);

  unsigned char buf[MAX_SEND_SIZE];

  // Begin appending the command
  memcpy(buf, header, headerLength);
  memcpy(buf + headerLength, outputRateString, outputRateLength);
  memcpy(buf + headerLength + outputRateLength, checksum, checksumLength);

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t stopBinaryOutputs(void) {
  // unsigned char buf[] = "$VNWRG,75,0,80,01,0528*XX\r\n";

  uint8_t outputRate = 800 / DEFAULT_BINARY_OUTPUT_RATE_HZ;  // See section

  const char header[] = "$VNWRG,75,0,";
  char outputRateString[MAX_OUTPUT_RATE_SIZE];  // Sufficient space for an int
  const char checksum[] = ",01,0528*XX\r\n";

  snprintf(outputRateString, sizeof(outputRateString), "%d", outputRate);

  size_t headerLength = strlen(header);
  size_t outputRateLength = strlen(outputRateString);
  size_t checksumLength = strlen(checksum);

  unsigned char buf[MAX_SEND_SIZE];

  // Begin appending the command
  memcpy(buf, header, headerLength);
  memcpy(buf + headerLength, outputRateString, outputRateLength);
  memcpy(buf + headerLength + outputRateLength, checksum, checksumLength);

  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciSendBytes(buf, sizeof(buf), portMAX_DELAY, UART_VN100_REG));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t readBinaryOutputs(vn100_binary_packet_t* parsedPacket) {
  unsigned char buf[BINARY_PACKET_SIZE] = {'\0'};
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sciReadBytes(buf, BINARY_PACKET_SIZE, portMAX_DELAY, pdMS_TO_TICKS(1000), UART_VN100_REG));

  RETURN_IF_ERROR_CODE(parsePacket(buf, parsedPacket));
  return OBC_ERR_CODE_SUCCESS;
}
