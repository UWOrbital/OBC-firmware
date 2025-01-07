#include "obc_metadata.h"
#include "obc_errors.h"

#include <stddef.h>

extern uint32_t __metadata_start__[sizeof(app_metadata_t) / sizeof(uint32_t)];

obc_error_code_t readAppMetadata(app_metadata_t* metadata) {
  if (metadata == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

// If BL is used, get the metadata and return success. Else return error code stating that there is no metadata
#if ENABLE_BL_BYPASS
  return OBC_ERR_CODE_NO_METADATA;
#else
  *metadata = *(const app_metadata_t*)(&__metadata_start__);
  return OBC_ERR_CODE_SUCCESS;
#endif
}
