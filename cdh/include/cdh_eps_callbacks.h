#ifndef CDH_INCLUDE_CDH_EPS_CALLBACKS_H_
#define CDH_INCLUDE_CDH_EPS_CALLBACKS_H_

#include "obc_errors.h"
#include "cdh_eps_protocol_data.h"

typedef obc_error_code_t (*rx_callback_t)(cdh_eps_queue_msg_t *);

/* Declare all command callbacks below */

// CMD_SUBSYS_SHUTDDOWN
obc_error_code_t subsysShutdownCmdCallback(cdh_eps_queue_msg_t *msg);

// CMD_HEARTBEAT
obc_error_code_t heartbeatCmdCallback(cdh_eps_queue_msg_t *msg);

// CMD_GET_TELEMETRY
obc_error_code_t getTelemetryCmdCallback(cdh_eps_queue_msg_t *msg);

// RESP_SUBSYS_SHUTDDOWN_ACK
obc_error_code_t respSubsysShutdownAckCallback(cdh_eps_queue_msg_t *msg);

// RESP_HEARTBEAT_ACK
obc_error_code_t respHeartbeatAckCallback(cdh_eps_queue_msg_t *msg);

#endif