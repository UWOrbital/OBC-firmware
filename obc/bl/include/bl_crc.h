#ifndef __BL_CRC_H__
#define __BL_CRC_H__

#include "bl_errors.h"
#include <stdint.h>

typedef struct crc_dma_request {
  uint32_t crcStartAddress;
  uint32_t crcElementCount;
  uint64_t crcSeedValue;
  uint64_t crcExpectedValue;
} crc_dma_request_t;

bl_error_code_t blCrcInit();

bl_error_code_t performCrcVerification(crc_dma_request_t* request);

#endif
