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

#define VN100_BAUDRATE  1152000
#define MAX_COMMAND_SIZE    256
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
    //TODO:
}

obc_error_code_t resetModule() {
    char cmd [MAX_COMMAND_SIZE] = "$VNRST*4D";
    return sciSendBytes(cmd, MAX_COMMAND_SIZE, TICK_TIMEOUT);
}

obc_error_code_t VN100SetBaudrate(uint32_t baudrate) {
    //TODO:
}

obc_error_code_t requestYPR() {
    // use sciSendByte, to transmit a request command to the VN-100
    char cmd [MAX_COMMAND_SIZE] = "$VNWRG,75,2,16,01,0029*XX\r\n";
    return sciSendBytes(cmd, MAX_COMMAND_SIZE, TICK_TIMEOUT);
}

obc_error_code_t retrieveYPR(vn_ypr_packet_t * packet) {
    if (packet == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }
    
}

