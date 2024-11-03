#pragma once

#include "obc_errors.h"
#include <stdint.h>

/*
Steps to update metadata:
    1. Update metadata size in memory.ld. To do this, reduce the size of APP_FLASH and edit the origin to be earlier in
flash
    2. Update the struct in this file to include the new metadata
    3. Update metadata.c in order to read from more memory
    4. Update bin_formatter class to include new metadata, find a way to make the struct with the bin_formatter before
sending with serial
    5. Update bl_main.c struct to read the proper amount of bytes
    6. Update bl_config.h to represent correct origin (METADATA_START_ADDRESS, METADATA_SIZE_BYTES)
 */

typedef struct {
  uint32_t vers;
  uint32_t bin_size;
} metadata_struct_t;

/**
 * @brief reads app metadata from the start of the metadata region specified in memory.ld
 *
 * @param metadata Pointer to metadata_struct_t instance to store metadata in
 * @return obc_error_code_t Success if app is flashed using bootloader, No Metadata otherwise.
 *
 *
 */

obc_error_code_t readAppMetadata(metadata_struct_t* metadata);
