#ifndef COMMON_INCLUDE_OBC_MATH_H_
#define COMMON_INCLUDE_OBC_MATH_H_

#include <stdint.h>

/**
 * @brief returns the minimum of 2 uint32_t numbers
 * 
 * @param a - first uint32_t number to compare
 * @param b - second uint32_t number to compare
 * @return the minimum of the 2 numbers
 */
static inline uint32_t uint32Min(uint32_t a, uint32_t b) {
    return (a) < (b) ? (a) : (b);
}
#endif // COMMON_INCLUDE_OBC_MATH_H_
