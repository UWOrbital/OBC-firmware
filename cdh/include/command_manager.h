#ifndef CDH_INCLUDE_COMMAND_MANAGER_H_
#define CDH_INCLUDE_COMMAND_MANAGER_H_

#include "obc_errors.h"
#include "command_data.h"

/**
 * @brief Initializes the command manager task
 * 
 */
void initCommandManager(void);

/**
 * @brief Sends a command to the command queue
 * 
 * @param cmd Pointer to the command message
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd);

#endif /* CDH_INCLUDE_COMMAND_MANAGER_H_ */
