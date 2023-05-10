#ifndef COMMON_INCLUDE_OBC_PERSIST_H
#define COMMON_INCLUDE_OBC_PERSIST_H

#include "obc_errors.h"

#include <stdint.h>
#include <stddef.h>

#define OBC_PERSIST_BASE_ADDR 0x0000UL

typedef struct {
	size_t len;
	uint32_t crc32;
} fram_header_t;

// Timekeeper
typedef struct {
	uint32_t unixTime;
} fram_data_timekeeper_t;

typedef struct {
	fram_header_t header;
	fram_data_timekeeper_t data;
} fram_persist_timekeeper_t;

/* This implementation assumes the underlying storage device
   is byte-addressable with no requirement for 4-byte alignment */
typedef struct __attribute__((packed)) {
	fram_persist_timekeeper_t timekeeperPersist;
} fram_persistent_data_t;

obc_error_code_t getPersistTimekeeper(fram_data_timekeeper_t *persist);
obc_error_code_t setPersistTimekeeper(const fram_data_timekeeper_t *persist);

#endif // COMMON_INCLUDE_OBC_PERSIST_H
