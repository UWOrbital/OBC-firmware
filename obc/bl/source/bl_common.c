#include "bl_common.h"
#include "bl_config.h"
#include "bl_utils.h"
#include "bl_errors.h"
#include "bl_crc.h"

#include <stdint.h>

bl_error_code_t verifyBinaryChecksum(app_header_t* header) {
  crc_dma_request request = {0};
  request.crcStartAddress = (uint32_t)APP_START_ADDRESS;
  request.crcElementCount = header->size;
  request.crcSeedValue = header->blChecksum.seed;
  request.crcExpectedValue = header->blChecksum.crc32Checksum;

  RETURN_IF_ERROR_CODE(performCrcVerification(&request));
}
