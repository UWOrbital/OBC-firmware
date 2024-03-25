#include "bl_common.h"
#include "bl_config.h"
#include "bl_utils.h"
#include "bl_errors.h"

#include <stdint.h>

static bl_error_code_t corruptedFlashApplicationCallback();

bl_error_code_t verifyBinaryChecksum(app_header_t* header) {
  if (computeCRC32Checksum((uint8_t*)APP_START_ADDRESS, header->size, header->blChecksum.seed) !=
      header->blChecksum.crc32Checksum) {
    RETURN_IF_ERROR_CODE(corruptedFlashApplicationCallback());
  }
}
