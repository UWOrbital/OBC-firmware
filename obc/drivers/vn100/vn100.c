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

#define VN100_BAUDRATE      1152000
#define MAX_COMMAND_SIZE    256U
#define TICK_TIMEOUT        portMAX_DELAY

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

static obc_error_code_t requestCMD (vn_cmd_t cmd);

void initVN100(void) {
    /* TODO:
       - Setup vn-100 mutex
       - configure baudrate
       - configure asyncronous outputs
    */
}

obc_error_code_t requestCMD (vn_cmd_t cmd) {
    unsigned char req [MAX_COMMAND_SIZE] = "";
    switch (cmd)
    {
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
        break;
    }
    sciSendBytes(&req, MAX_COMMAND_SIZE, TICK_TIMEOUT, UART_VN100_REG);
    return OBC_ERR_CODE_SUCCESS;
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

