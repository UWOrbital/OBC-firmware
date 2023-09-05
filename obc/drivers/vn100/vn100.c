#include "obc_errors.h"
#include "obc_assert.h"
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
#define TICK_TIMEOUT portMAX_DELAY

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

static obc_error_code_t parsePacket(vn_cmd_t cmd, void* packet);

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

obc_error_code_t parsePacket(vn_cmd_t cmd, void* packet) {
    /* TODO:
        - Implement parsing for each of the packet types to extract the data 
          and organize them into their respective packet types
    */
    
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t resetModule() {
  /* TODO:
    - Confirm that the right response message is received from the module, otherwise throw an error
  */
  unsigned char req [MAX_COMMAND_SIZE] = "$VNRST*4D\r\n";
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
  unsigned char baud[12];
  // Set to XX for now, means to ignore the checksum
  unsigned char checksum[3] = "*XX\r\n"; 
  unsigned char req [MAX_COMMAND_SIZE] = "$VNWRG,05,";
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
       - Let the user choose from the defined packet types to configure the asyncronous output register with a particular packet type
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
