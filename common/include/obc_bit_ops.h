#ifndef COMMON_INCLUDE_OBC_BIT_OPS_H_
#define COMMON_INCLUDE_OBC_BIT_OPS_H_

#include <stdint.h>

/**
 * @brief Converts a 2-digit decimal number to BCD format.
 * 
 * @param inputVal Decimal value to be converted
 * @return the BCD value.
 */
static inline uint8_t twoDigitDecimalToBCD(uint8_t inputVal) {
    return ((inputVal / 10) << 4) | (inputVal % 10);
}

/**
 * @brief Converts a 2-digit BCD value to a decimal number.
 * 
 * @param data Binary coded decimal value
 * @return the decimal value.
 */
static inline uint8_t twoDigitDecimalFromBCD(uint8_t data) {
    return ((data >> 4) * 10) + (data & 0xF);
}

#endif // COMMON_INCLUDE_OBC_BIT_OPS_H_
