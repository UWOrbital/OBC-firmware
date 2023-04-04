#ifndef CDH_INCLUDE_CDH_EPS_PROTOCOL_DATA_H_
#define CDH_INCLUDE_CDH_EPS_PROTOCOL_DATA_H_

#include "obc_logging.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t id; /* Command ID */
    uint8_t param[7]; /* Command Paramaters */
} cdh_eps_cmd_msg_t;

typedef struct {
    uint8_t id; /* Telemetry ID */
    uint8_t reserved[5]; /* Reserved Bytes */
    uint8_t data[2]; /* Telemetry Data */
} cdh_eps_tle_msg_t;

typedef struct {
    uint8_t id; /* Response ID */
    uint8_t reserved[6]; /* Reserved Bytes */
    uint8_t request; /* Request granted or denied */
} cdh_eps_resp_msg_t;

#endif /* CDH_INCLUDE_CDH_EPS_PROTOCOL_DATA_H_ */