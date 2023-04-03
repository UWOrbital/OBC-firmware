#ifndef DRIVERS_INCLUDE_OBC_CAN_IO
#define DRIVERS_INCLUDE_OBC_CAN_IO

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
 * @return 1 if succcesful, 0 if not
*/
uint8_t canSendMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *data);

/**
 * @brief Recieve a buffer of bytes from the CAN bus
 * @param canReg Pointer to CAN node 
 * @param messageBox Message box number of CAN node
 * @param data RX data to recieve
 * @return 1 if succcesful, 0 if not
*/
uint8_t canGetMessage(canBASE_t *canReg, uint32_t messageBox, const uint8_t *data);

#endif