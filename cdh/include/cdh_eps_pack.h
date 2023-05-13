#ifndef CDH_INCLUDE_CDH_EPS_PACK_H_
#define CDH_INCLUDE_CDH_EPS_PACK_H_

#include "obc_errors.h"
#include "cdh_eps_protocol.h"

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Pack command packet into message union
 * 
 * @param msg Union to pack into
 * @param id ID of the command
 * @param param parameters to accompany the command
 * 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the data was packed successfully, otherwise an error code
 */
void packCmdMsg(cdh_eps_queue_msg_t *msg, uint8_t id, uint8_t param[7]);

/**
 * @brief Pack telemtry packet into message union
 * 
 * @param msg Union to pack into
 * @param id ID of the telemetry
 * @param data data of the telemetry packet
 * 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the data was packed successfully, otherwise an error code
 */
void packTleMsg(cdh_eps_queue_msg_t *msg, uint8_t id, uint8_t data[2]);

/**
 * @brief Pack response packet into message union
 * 
 * @param msg Union to pack into
 * @param id ID of the response
 * @param request request to accompany request
 * 
 * @return obc_error_code_t OBC_ERR_CODE_SUCCESS if the data was packed successfully, otherwise an error code
 */
void packRespMsg(cdh_eps_queue_msg_t *msg, uint8_t id, uint8_t request);

#endif /* CDH_INCLUDE_CDH_EPS_PACK_H_ */