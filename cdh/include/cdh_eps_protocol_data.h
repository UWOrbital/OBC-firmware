#ifndef CDH_INCLUDE_CDH_EPS_PROTOCOL_DATA_H_
#define CDH_INCLUDE_CDH_EPS_PROTOCOL_DATA_H_

#include "obc_assert.h"
#include "obc_logging.h"
#include "obc_assert.h"

#include <stdint.h>
#include <stdbool.h>

/* -------------------------- */
/*   Command Message Struct   */
/* -------------------------- */
typedef struct {
    uint8_t id; /* Command ID */
    uint8_t param[7]; /* Command Paramaters */
} cdh_eps_cmd_msg_t;

/* -------------------------- */
/*  Telemetry Message Struct  */
/* -------------------------- */
typedef struct {
    uint8_t id; /* Telemetry ID */
    uint8_t reserved[5]; /* Reserved Bytes */
    uint8_t data[2]; /* Telemetry Data */
} cdh_eps_tle_msg_t;

/* -------------------------- */
/*  Response Message Struct   */
/* -------------------------- */
typedef struct {
    uint8_t id; /* Response ID */
    uint8_t reserved[6]; /* Reserved Bytes */
    uint8_t request; /* Request granted or denied */
} cdh_eps_resp_msg_t;

STATIC_ASSERT_INT_EQ(sizeof(cdh_eps_cmd_msg_t), 8);
STATIC_ASSERT_INT_EQ(sizeof(cdh_eps_tle_msg_t), 8);
STATIC_ASSERT_INT_EQ(sizeof(cdh_eps_resp_msg_t), 8);

/* -------------------------- */
/*      Union for Queue       */
/* -------------------------- */
typedef union {
    cdh_eps_cmd_msg_t cmd;
    cdh_eps_tle_msg_t tle;
    cdh_eps_resp_msg_t resp;
} cdh_eps_queue_msg_t;

STATIC_ASSERT_EQ((int)sizeof(cdh_eps_cmd_msg_t), (int)8);
STATIC_ASSERT_EQ((int)sizeof(cdh_eps_tle_msg_t), (int)8);
STATIC_ASSERT_EQ((int)sizeof(cdh_eps_resp_msg_t), (int)8);

#endif /* CDH_INCLUDE_CDH_EPS_PROTOCOL_DATA_H_ */