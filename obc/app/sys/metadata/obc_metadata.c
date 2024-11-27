#include "obc_metadata.h"
#include "obc_errors.h"

#include <stddef.h>

extern uint32_t __metadata_start__;

obc_error_code_t readAppMetadata(app_metadata_t* metadata) {
  if (metadata == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

// If BL is used, get the metadata and return success. Else return error code stating that there is no metadata
#if ENABLE_BL_BYPASS
  return OBC_ERR_CODE_NO_METADATA;
#else
  volatile app_metadata_t* flash_metadata = (volatile app_metadata_t*)(&__metadata_start__);

  metadata->vers = flash_metadata->vers;
  metadata->binSize = flash_metadata->binSize;
  metadata->boardType = flash_metadata->boardType;
  // If buffers are replaced with actual data, add more here

  return OBC_ERR_CODE_SUCCESS;
#endif
}
