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

void initVN100(void) {
    
}

obc_error_code_t VN100SetBaudRate(int baudrate) {

}

obc_error_code_t requestYPR(sciBase_t * sciREG) {
    // use sciSendByte, to transmit a request packet to the VN-100
    sciSendByte()
}

obc_error_code_t retrieveYPR(ypr_packet* buf) {
    if (buf == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

}

