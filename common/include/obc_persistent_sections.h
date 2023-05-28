#ifndef COMMON_INCLUDE_OBC_PERSISTENT_SECTIONS_H_
#define COMMON_INCLUDE_OBC_PERSISTENT_SECTIONS_H_

#include "obc_persistent_data_config.h"
#include "obc_assert.h"
#include "fram.h"

typedef struct {
    size_t len;
    uint32_t crc32;
} obc_persist_section_header_t;

typedef struct {
    obc_persist_section_header_t header;
    obc_time_persist_data_t data;
} obc_time_persist_t;

typedef struct {
    obc_persist_section_header_t header;
    obc_state_persist_data_t data;
} obc_state_persist_t;

typedef struct {
    obc_persist_section_header_t header;
    obc_config_persist_data_t data;
} obc_config_persist_t;

typedef struct {
    obc_time_persist_t obcTime;
    obc_state_persist_t obcState;
    obc_config_persist_t obcConfig;
} fram_persist_t;

STATIC_ASSERT(sizeof(fram_persist_t) <= FRAM_MAX_ADDRESS, "fram_persist_t exceeds available FRAM space");

#define FRAM_ADDRESS_OF(data) (0x0 + offsetof(fram_persist_t, data))

#endif // COMMON_INCLUDE_OBC_PERSISTENT_SECTIONS_H_
