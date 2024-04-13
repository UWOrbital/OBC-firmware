#ifndef __BL_CRC_H__
#define __BL_CRC_H__

#include "bl_errors.h"
#include <stdint.h>

typedef struct crc_dma_request {
  void* const crcStartAddress;
  uint32_t crcElementCount;
  uint64_t crcSeedValue;
  uint64_t crcExpectedValue;
} crc_dma_request_t;

bl_error_code_t crcInit();

bl_error_code_t performCrcVerification(crc_dma_request_t* request);

#endif
