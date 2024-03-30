#include "bl_utils.h"
#include <stdint.h>

#define DMA_COMPLETE 0xFF
#define DMA_NOT_COMPLETE 0x00

#define CRC_CONTROL_BASE_REGISTER 0xFE000000
#define CRC_BUSY_FLAG_REGISTER (0xFE000000 | 0x38)
#define CRC_PATTERN_COUNT_REGISTER (CRC_CONTROL_BASE_REGISTER | 0x40)
#define CRC_SECTOR_COUNT_REGISTER (CRC_CONTROL_BASE_REGISTER | 0x44)

#define CRC_SELECT_SIGNATURE_LOW_REGISTER (CRC_CONTROL_BASE_REGISTER | 0x70)
#define CRC_SELECT_SIGNATURE_HIGH_REGISTER (CRC_CONTROL_BASE_REGISTER | 0x74)

#define CRC_CRC_VALUE_LOW_REGISTER (CRC_CONTROL_BASE_REGISTER | 0x68)
#define CRC_CRC_VALUE_HIGH_REGISTER (CRC_CONTROL_BASE_REGISTER | 0x6C)

static volatile uint32_t* crcBusyFlagRegister = (void*)CRC_BUSY_FLAG_REGISTER;

static uint8_t dmaCompleteFlag = DMA_NOT_COMPLETE;

static inline void setDmaCompleteFlag();
static inline void clearDmaCompleteFlag();

uint32_t computeCRC32Checksum(const uint8_t* s, uint16_t n) {
  uint32_t crc = 0xFFFFFFFF;

  for (uint16_t i = 0; i < n; i++) {
    char ch = s[i];
    for (uint16_t j = 0; j < 8; j++) {
      uint32_t b = (ch ^ crc) & 1;
      crc >>= 1;
      if (b) crc = crc ^ 0xEDB88320;
      ch >>= 1;
    }
  }

  return ~crc;
}

static inline void setDmaCompleteFlag() { dmaCompleteFlag = DMA_COMPLETE; }
static inline void clearDmaCompleteFlag() { dmaCompleteFlag = DMA_NOT_COMPLETE; }
