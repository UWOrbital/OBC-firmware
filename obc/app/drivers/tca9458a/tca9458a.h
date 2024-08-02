#pragma once

#include "stdint.h"
#include "obc_spi_io.h"
#include "obc_board_config.h"

/**
 * @brief Select an I2C port on the TCA9458a multiplexer
 * @param tca Port number to select
 * @return Error code indicating if an ACK was received
 */
obc_error_code_t tcaSelect(uint8_t cam);
