#ifndef CDH_INCLUDE_CDH_EPS_CALLBACKS_H_
#define CDH_INCLUDE_CDH_EPS_CALLBACKS_H_

#include "obc_errors.h"
#include "cdh_eps_protocol_data.h"

typedef obc_error_code_t (*rx_handler_t)(cdh_eps_queue_msg_t *);

/* Declare all command callbacks below */

// CMD_SUBSYS_SHUTDDOWN
obc_error_code_t subsysShutdownCmdHandler(cdh_eps_queue_msg_t *msg);

// TLE_
obc_error_code_t tleMsgHandler(cdh_eps_queue_msg_t *msg);

// RESP_SUBSYS_SHUTDDOWN_ACK
obc_error_code_t respSubsysShutdownAckHandler(cdh_eps_queue_msg_t *msg);

// RESP_HEARTBEAT_ACK
obc_error_code_t respHeartbeatAckHandler(cdh_eps_queue_msg_t *msg);

#endif