#include "obc_crc.h"

#include "obc_errors.h"

#include <stdint.h>

#define SUSPICIOUS_CRC_VALUE (0xBAADC0DEU)
#define CCITT_32_POLYNOMIAL (0xEDB88320U)

/** @brief Compute a CRC32 for the given data buffer. (taken from obc/reliance_edge/utils)

    For CCITT-32 compliance, the initial CRC must be set to 0.  To CRC multiple
    buffers, call this function with the previously returned CRC value.

    @param ulInitCrc32  Starting CRC value.
    @param pBuffer      Data buffer to calculate the CRC from.
    @param ulLength     Number of bytes of data in the given buffer.

    @return The updated CRC value.
*/
static uint32_t RedCrc32Update(uint32_t ulInitCrc32, const void *pBuffer, uint32_t ulLength) {
  uint32_t ulCrc32;

  if (pBuffer == NULL) {
    ulCrc32 = SUSPICIOUS_CRC_VALUE;
  } else {
    const uint8_t *pbBuffer = pBuffer;
    uint32_t ulIdx;

    ulCrc32 = ~ulInitCrc32;

    for (ulIdx = 0U; ulIdx < ulLength; ++ulIdx) {
      uint32_t ulBit;

      ulCrc32 ^= pbBuffer[ulIdx];

      /*  Branchless inner loop (greatly improves performance).
       */
      for (ulBit = 0U; ulBit < 8U; ulBit++) {
        ulCrc32 = ((ulCrc32 & 1U) * CCITT_32_POLYNOMIAL) ^ (ulCrc32 >> 1U);
      }
    }

    ulCrc32 = ~ulCrc32;
  }

  return ulCrc32;
}

uint32_t computeCrc32(const uint32_t prevCrc32, const uint8_t *buffer, size_t len) {
  return RedCrc32Update(prevCrc32, buffer, len);
}
