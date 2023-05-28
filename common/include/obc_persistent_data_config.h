#ifndef COMMON_INCLUDE_OBC_PERSISTENT_DATA_CONFIG_H_
#define COMMON_INCLUDE_OBC_PERSISTENT_DATA_CONFIG_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "obc_states.h"

// obc_time module
typedef struct {
    uint32_t unixTime;
} obc_time_persist_data_t;

// obc_state module
typedef struct {
    obc_state_t obcState;
} obc_state_persist_data_t;

// obc config
typedef struct {
    bool isInitialized; // OBC is initialized
} obc_config_persist_data_t;

#endif  // COMMON_INCLUDE_OBC_PERSISTENT_DATA_CONFIG_H_
