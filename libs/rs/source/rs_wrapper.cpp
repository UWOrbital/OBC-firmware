#include "rs.hpp"

#ifdef __cplusplus
extern "C" {
#endif

constexpr uint8_t MSG_LENGTH = 223;
constexpr uint8_t ECC_LENGTH = 32;

static RS::ReedSolomon<MSG_LENGTH, ECC_LENGTH> rs;

void rs_init() {
    // Initialize the single instance if needed
    // For example, if your class has an initialization method, call it here
}

void rs_encode(void* msg, void* encoded) {
    rs.Encode(msg, encoded);
}

int rs_decode(void* encoded, void* repaired) {
    return rs.Decode(encoded, repaired);
}

#ifdef __cplusplus
}
#endif
