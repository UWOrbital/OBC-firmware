#ifndef __BL_COMMON__
#define __BL_COMMON__

#include "bl_errors.h"
#include "bl_utils.h"
#include <stdint.h>

typedef struct {
  uint32_t crc32Checksum;
  uint32_t seed;
} bl_integrity_t;

// If this header changes, update the host utility
typedef struct {
  uint32_t version;
  uint32_t size;
  bl_integrity_t blIntegrityData;
} app_header_t;

bl_error_code_t verifyBinaryChecksum(app_header_t* header);

#endif
