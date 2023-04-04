#ifndef CDH_INCLUDE_CDH_EPS_PROTOCOL_H_
#define CDH_INCLUDE_CDH_EPS_PROTOCOL_H_

#include "obc_errors.h"
#include "cdh_eps_protocol_data.h"

#include <sys_common.h>

/**
 * @brief Initializes the CDH-EPS task
 * 
 */
void initCDHEPS(void);

/**
 * @brief Sends a command to the cdh-eps queue to be sent over CAN to EPS
 * 
 * @param 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCDHEPSCmdTxQueue(cdh_eps_cmd_msg_t *cmd);

/**
 * @brief Sends a recieved command CAN message from EPS to the cdh-eps task
 * 
 * @param 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCDHEPSCmdRxQueue(cdh_eps_cmd_msg_t *cmd);

/**
 * @brief Sends a response to the cdh-eps queue to be sent over CAN to EPS
 * 
 * @param 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCDHEPSRespTxQueue(cdh_eps_resp_msg_t *resp);

/**
 * @brief Sends a recieved response CAN message from EPS to the cdh-eps task
 * 
 * @param 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCDHEPSRespRxQueue(cdh_eps_resp_msg_t *resp);

#endif /* CDH_INCLUDE_CDH_EPS_PROTOCOL_H_ */