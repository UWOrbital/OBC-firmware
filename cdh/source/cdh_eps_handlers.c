#include "cdh_eps_handlers.h"
#include "cdh_eps_protocol_id.h"
#include "command_data.h"
#include "telemetry_manager.h"

#include "obc_reset.h"
#include "obc_errors.h"
#include "obc_logging.h"
#include "obc_unpack_utils.h"
#include "obc_time.h"

#include <stddef.h>

static const uint8_t tleIdMap[] = {
    [TLE_EPS_BOARD_TEMP] = TELEM_EPS_BOARD_TEMP,
    [TLE_SOLAR_PANEL_1_TEMP] = TELEM_SOLAR_PANEL_1_TEMP,
    [TLE_SOLAR_PANEL_2_TEMP] = TELEM_SOLAR_PANEL_2_TEMP,
    [TLE_SOLAR_PANEL_3_TEMP] = TELEM_SOLAR_PANEL_3_TEMP,
    [TLE_SOLAR_PANEL_4_TEMP] = TELEM_SOLAR_PANEL_4_TEMP,
    [TLE_COMMS_5V_CURRENT] = TELEM_EPS_COMMS_5V_CURRENT,
    [TLE_COMMS_3V3_CURRENT] = TELEM_EPS_COMMS_3V3_CURRENT,
    [TLE_MAG_8V_CURRENT] = TELEM_EPS_MAGNETORQUER_8V_CURRENT,
    [TLE_ADCS_5V_CURRENT] = TELEM_EPS_ADCS_5V_CURRENT,
    [TLE_ADCS_3V3_CURRENT] = TELEM_EPS_ADCS_3V3_CURRENT,
    [TLE_OBC_CURRENT] = TELEM_EPS_OBC_3V3_CURRENT,
    [TLE_COMMS_5V_VOLTAGE] = TELEM_EPS_COMMS_5V_VOLTAGE,
    [TLE_COMMS_3V3_VOLTAGE] = TELEM_EPS_COMMS_3V3_VOLTAGE,
    [TLE_MAG_8V_VOLTAGE] = TELEM_EPS_MAGNETORQUER_8V_VOLTAGE,
    [TLE_ADCS_5V_VOLTAGE] = TELEM_EPS_ADCS_5V_VOLTAGE,
    [TLE_ADCS_3V3_VOLTAGE] = TELEM_EPS_ADCS_3V3_VOLTAGE,
    [TLE_OBC_3V3_VOLTAGE] = TELEM_EPS_OBC_3V3_VOLTAGE 
};


obc_error_code_t subsysShutdownCmdHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    LOG_DEBUG("Handling subsystem shutdown command");

    // need to figure out where to send request from here
    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t tleMsgHandler(cdh_eps_queue_msg_t *msg) {
    if(msg == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    LOG_DEBUG("Recieved telemetry packet with ID: %d", msg->tle.id);
    
    obc_error_code_t res;
    float data = unpackFloat(msg->tle.data, 0);
    // Initialize telemetry struct to send to telemetry manager

    telemetry_data_id_t id;
    if(tleIdMap[msg->tle.id] == NULL) {
        LOG_DEBUG("Invalid telemetry id received");
        return res;
    }
    else {
        id = tleIdMap[msg->tle.id];
    }

    telemetry_data_t tle = 
    {.cc1120Temp = data, 
     .id = id,
     .timestamp = getCurrentunixTime()
    };

    res = addTelemetryData(&tle);

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