#pragma once

#include <stdint.h>

/*
Steps to update metadata:
    1. Update metadata size in memory.ld. To do this, reduce the size of APP_FLASH and edit the origin to be earlier in
        flash (skip this step if buffers are still available in the struct below)
    2. Update the struct in this file to include the new metadata. Just replace one of the buffers if available
        (assuming you are trying to store a 32 bit value).
    3. Update obc_metadata.c in order to read new struct members
    4. Update bin_formatter class to include new metadata, find a way to make the struct with the bin_formatter
    5. Update bl_config.h to represent correct origin (update METADATA_START_ADDRESS and METADATA_SIZE_BYTES) (skip this
step if buffers are still available in the struct below)
 */

typedef struct {
  uint32_t vers;
  uint32_t binSize;
  uint32_t boardType;
  uint32_t unused0;
  uint32_t unused1;
  uint32_t unused2;
  uint32_t unused3;
  uint32_t unused4;
} app_metadata_t;
