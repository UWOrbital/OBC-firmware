#include "obc_unpack_utils.h"

#include <stdint.h>
#include <string.h>

uint8_t unpackUint8(const uint8_t* buffer, uint32_t* offset) {
    uint8_t value = buffer[*offset];
    (*offset)++;
    return value;
}

uint16_t unpackUint16(const uint8_t* buffer, uint32_t* offset) {
    uint16_t value = ((uint16_t)buffer[*offset] << 8) |
                     ((uint16_t)buffer[*offset + 1]);
    (*offset) += 2;
    return value;
}

uint32_t unpackUint32(const uint8_t* buffer, uint32_t* offset) {
    uint32_t value = ((uint32_t)buffer[*offset] << 24) |
                     ((uint32_t)buffer[*offset + 1] << 16) |
                     ((uint32_t)buffer[*offset + 2] << 8) |
                     ((uint32_t)buffer[*offset + 3]);
    (*offset) += 4;
    return value;
}

int8_t unpackInt8(const uint8_t* buffer, uint32_t* offset) {
    return (int8_t)unpackUint8(buffer, offset);
}

int16_t unpackInt16(const uint8_t* buffer, uint32_t* offset) {
    return (int16_t)unpackUint16(buffer, offset);
}

int32_t unpackInt32(const uint8_t* buffer, uint32_t* offset) {
    return (int32_t)unpackUint32(buffer, offset);
}

float unpackFloat(const uint8_t* buffer, uint32_t* offset) {
    float val;

    uint32_t tmp = unpackUint32(buffer, offset);
    memcpy(&val, &tmp, sizeof(val));

    return val;
}
