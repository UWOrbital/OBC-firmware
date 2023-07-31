#pragma once

#include "obc_errors.h"

#include <stdint.h>
#include <stdbool.h>

#define AX25_TIMEOUT_MILLISECONDS 300000

/**
 * @brief flag timeout callback that sets isFlagReceived to false due to a timeout
 *
 * @param none
 *
 * @return void
 */
void flagTimeoutCallback();

/**
 * @brief parses the completely decoded data and sends it to the command manager and detects end of transmission
 *
 * @param cmdBytes 223B-AES_IV_SIZE array storing the completely decoded data
 *
 * @return obc_error_code_t - whether or not the data was successfullysent to the command manager
 */
obc_error_code_t handleCommands(uint8_t *cmdBytes);

/**
 * @brief initializes the decode data pipeline task
 *
 * @return void
 */
void initDecodeTask(void);

/**
 * @brief send a received packet to the decode data pipeline to be sent to command manager
 *
 * @param data pointer to a single byte
 *
 * @return obc_error_code_t - whether or not the packet was successfully sent to the queue
 */
obc_error_code_t sendToDecodeDataQueue(uint8_t *data);
