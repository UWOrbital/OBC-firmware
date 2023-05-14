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
 * @brief Sends an TX message to the cdh-eps queue to be sent over CAN to EPS
 * 
 * @param msg pointer to union holding message
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCDHEPSTxQueue(cdh_eps_queue_msg_t *msg);

/**
 * @brief Sends an RX message to the cdh-eps queue to be proccessed by cdh-eps task
 * 
 * @param msg pointer to union holding message
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
obc_error_code_t sendToCDHEPSRxQueue(cdh_eps_queue_msg_t *msg);

#endif /* CDH_INCLUDE_CDH_EPS_PROTOCOL_H_ */