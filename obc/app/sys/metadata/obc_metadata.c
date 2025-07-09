#include "obc_metadata.h"
#include "obc_errors.h"
#include <metadata_struct.h>

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
  *metadata = *(app_metadata_t*)(&__metadata_start__);
  return OBC_ERR_CODE_SUCCESS;
#endif
}

// TODO: DELETE Alll
// Include Metadata.h from
// Set reset handle to c_int(00)
// extern the crc address from app_link.ld
// Add the BL verify CRC check (Before jumping to app verify if crc is valid otherwise keep listening for commands)
