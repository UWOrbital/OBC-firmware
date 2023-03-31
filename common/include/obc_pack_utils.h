#ifndef COMMON_INCLUDE_OBC_PACK_UTILS_H_
#define COMMON_INCLUDE_OBC_PACK_UTILS_H_

#include <stdint.h>
#include <stddef.h>

/**
 * Packs an unsigned 8-bit integer into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packUint8(uint8_t value, uint8_t* buffer, size_t* offset);

/**
 * Packs an unsigned 16-bit integer into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packUint16(uint16_t value, uint8_t* buffer, size_t* offset);

/**
 * Packs an unsigned 32-bit integer into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packUint32(uint32_t value, uint8_t* buffer, size_t* offset);

/**
 * Packs a signed 8-bit integer into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packInt8(int8_t value, uint8_t* buffer, size_t* offset);

/**
 * Packs a signed 16-bit integer into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packInt16(int16_t value, uint8_t* buffer, size_t* offset);

/**
 * Packs a signed 32-bit integer into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packInt32(int32_t value, uint8_t* buffer, size_t* offset);

/**
 * Packs a floating point value into a buffer at the given offset.
 *
 * @param value   The value to pack.
 * @param buffer  The buffer to pack the value into.
 * @param offset  A pointer to the offset within the buffer to pack the value at.
 */
void packFloat(float value, uint8_t* buffer, size_t* offset);

#endif  // COMMON_INCLUDE_OBC_PACK_UTILS_H_
