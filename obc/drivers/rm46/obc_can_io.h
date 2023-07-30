#pragma once

#include <stdint.h>

/**
 * @brief Initialize the CAN bus mutex
 */
void initCANMutex(void);

/**
 * @brief Send a buffer of bytes to the CAN bus
 * @param canReg Pointer to CAN node 
 * @param messageBox Message box number of CAN node
 * @param data TX data to transmit
 * @return OBC_ERR_CODE_SUCCESS if transmit was successful
 *         OBC_ERR_CODE_CAN_SEND_FAILURE if transmit was unsuccessful
*/
obc_error_code_t canSendMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *data);

/**
 * @brief Recieve a buffer of bytes from the CAN bus
 * @param canReg Pointer to CAN node 
 * @param messageBox Message box number of CAN node
 * @param data RX data to recieve
 * @return OBC_ERR_CODE_SUCCESS if receive was successful
 *         OBC_ERR_CODE_CAN_NO_RECV there is no data to receive
 *         OBC_ERR_CODE_CAN_RECV_FAILURE when data was stored in buffer but message was lost
*/
obc_error_code_t canGetMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *data);