#pragma once

#include "obc_errors.h"
#include <metadata_struct.h>
#include <stdint.h>

/**
 * NOTE: app_metadata_t is found in shared/metadata/metadata_struct. Please update changes following the instructions
 * there if you edit these files
 */

/**
 * @brief reads app metadata from the start of the metadata region specified in memory.ld
 *
 * @param metadata Pointer to metadata_struct_t instance to store metadata in
 * @return obc_error_code_t Success if app is flashed using bootloader, No Metadata otherwise.
 *
 *
 */

obc_error_code_t readAppMetadata(app_metadata_t* metadata);
