#ifndef __OBC_CRC_H__
#define __OBC_CRC_H__

#include "obc_errors.h"
#include <os_portmacro.h>
#include <stdint.h>

// Default suggested timeout.
#define OBC_CRC_MUTEX_TIMEOUT pdMS_TO_TICKS(10)
#define OBC_CRC_DMA_TIMEOUT pdMS_TO_TICKS(15)

typedef struct obc_dma_request {
  uint32_t crcStartAddress;
  uint32_t crcElementCount;
  uint64_t crcSeedValue;
  uint64_t crcExpectedValue;
} obc_dma_request_t;

obc_error_code_t obcCrcInit();

obc_error_code_t performCrcVerification(crc_dma_request_t* request, TickType_t mutexTimeout, TickType_t dmaTimeout);

#endif
