#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void rs_init();
void rs_encode(void* msg, void* encoded);
int rs_decode(void* encoded, void* repaired);

#ifdef __cplusplus
}
#endif
