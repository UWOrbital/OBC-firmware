#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  size_t len;
  uint32_t crc32;
} obc_persist_section_header_t;

/* obc_time module */
typedef struct {
  uint32_t unixTime;
} obc_time_persist_data_t;

typedef struct {
  obc_persist_section_header_t header;
  obc_time_persist_data_t data;
} obc_time_persist_t;

/* Format of the entire persistent store */
typedef struct {
  obc_time_persist_t obcTime;
} obc_persist_t;

#define FRAM_ADDRESS_OF(data) (0x0 + offsetof(obc_persist_t, data))
