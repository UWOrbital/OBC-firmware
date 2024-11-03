#include "metadata.h"
#include "obc_errors.h"

#include <stddef.h>

extern uint32_t __metadata_start__;

obc_error_code_t readAppMetadata(metadata_struct_t* metadata) {
  if (metadata == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

// If BL is used, get the metadata and return success. Else do not return success
#if ENABLE_BL_BYPASS
  return OBC_ERR_CODE_NO_METADATA_APP_NOT_OVER_SERIAL;
#else
  volatile uint8_t *flash_ptr = (volatile uint8_t *)(&__metadata_start__);

  metadata->vers = ((uint32_t)flash_ptr[0]) | ((uint32_t)flash_ptr[1] << 8) | ((uint32_t)flash_ptr[2] << 16) |
                   ((uint32_t)flash_ptr[3] << 24);

  metadata->bin_size = ((uint32_t)flash_ptr[4]) | ((uint32_t)flash_ptr[5] << 8) |
                       ((uint32_t)flash_ptr[6] << 16 | (uint32_t)flash_ptr[7] << 24);
  // Add more struct members and follow the pattern if metadata is added
  return OBC_ERR_CODE_SUCCESS;
#endif
}
