#pragma once

#include "obc_persistent_data_config.h"
#include "obc_assert.h"

typedef struct {
  size_t len;
  uint32_t crc32;
} obc_persist_section_header_t;

typedef struct {
  obc_persist_section_header_t header;
  obc_time_persist_data_t data;
} obc_time_persist_t;

typedef struct {
  obc_time_persist_t obcTime;
} obc_persist_t;

#define FRAM_ADDRESS_OF(data) (0x0 + offsetof(obc_persist_t, data))
