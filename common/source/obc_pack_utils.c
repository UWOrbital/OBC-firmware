#include "obc_pack_utils.h"
#include "obc_assert.h"

#include <stdint.h>
#include <stddef.h>

void packUint8(uint8_t value, uint8_t* buffer, size_t* offset) {
    buffer[*offset] = value;
    (*offset)++;
}

void packUint16(uint16_t value, uint8_t* buffer, size_t* offset) {
    buffer[*offset] = (uint8_t)(value >> 8);
    buffer[*offset + 1] = (uint8_t)value;
    (*offset) += 2;
}

void packUint32(uint32_t value, uint8_t* buffer, size_t* offset) {
    buffer[*offset] = (uint8_t)(value >> 24);
    buffer[*offset + 1] = (uint8_t)(value >> 16);
    buffer[*offset + 2] = (uint8_t)(value >> 8);
    buffer[*offset + 3] = (uint8_t)value;
    (*offset) += 4;
}

void packInt8(int8_t value, uint8_t* buffer, size_t* offset) {
    packUint8((uint8_t)value, buffer, offset);
}

void packInt16(int16_t value, uint8_t* buffer, size_t* offset) {
    packUint16((uint16_t)value, buffer, offset);
}

void packInt32(int32_t value, uint8_t* buffer, size_t* offset) {
    packUint32((uint32_t)value, buffer, offset);
}

void packFloat(float value, uint8_t* buffer, size_t* offset) {
    STATIC_ASSERT(sizeof(float) == sizeof(uint32_t), "float and uint32_t must be the same size");

    uint32_t tmp = *((uint32_t*)&value);
    packUint32(tmp, buffer, offset);
}
