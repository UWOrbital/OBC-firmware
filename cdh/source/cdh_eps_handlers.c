#include "cdh_eps_handlers.h"
#include "command_data.h"
#include "telemetry_manager.h"

#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_unpack_utils.h"

#include <stddef.h>

obc_error_code_t subsysShutdownCmdHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    LOG_DEBUG("Handling subsystem shutdown command");

    // need to figure out where to send request from here
    return OBC_ERR_CODE_SUCCESS;
}

// This handler is not needed since cdh will never recieve a heartbeat command
obc_error_code_t heartbeatCmdHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    LOG_DEBUG("Handling heartbeat command");

    return OBC_ERR_CODE_SUCCESS;
}

// This handler is not needed since the get telemetry cmd should never be recieved by CDH
obc_error_code_t getTelemetryCmdHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    LOG_DEBUG("Sending get ");

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t tleMsgHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    LOG_DEBUG("Recieved telemetry packet with ID: %d", msg->tle.id);
    
    float data = unpackFloat(msg->tle.data, 0);
    // Initialize telemetry struct to send to telemetry manager
    telemetry_data_t tle = 
    {.cc1120Temp = data, 
     .id = 0x00, // need way to map CDH-EPS telemetry_data_id
     .timestamp = 0 // Use getCurrentUnixTime in obc_time.c (need to merge in)
    };

    obc_error_code_t res = addTelemetryData(&tle);

    return res;
}

obc_error_code_t respSubsysShutdownAckHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    obc_error_code_t res = OBC_ERR_CODE_SUBSYS_SHUTDOWN_DENIED;
    if(msg->resp.request == 1) {
        res = OBC_ERR_CODE_SUBSYS_SHUTDOWN_GRANTED;
        LOG_DEBUG("Subsystem shut down request granted");
    }
    else {
        LOG_DEBUG("Subsystem shut down request granted");
    }

    return res;
}

obc_error_code_t respHeartbeatAckHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    obc_error_code_t res = OBC_ERR_CODE_HEARTBEAT_INVALID;
    if(msg->resp.request = 0) {
        res = OBC_ERR_CODE_HEARTBEAT_VALID;
        LOG_DEBUG("Valid heartbeat connection");
    }
    else {
        LOG_DEBUG("Invalid heartbeat connection");
    }

    return res;
}