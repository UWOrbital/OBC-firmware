#include "obc_metadata.h"

extern void _c_int00(void);
extern uint32_t __crc_addr;

const metadata_t app_metadata __attribute__((section(".metadata"), used)) = {
    .magic_num = 0xFFFF,
    .build_num = 0,
    .git_hash = 0,
    .app_entry_func_addr = (uint32_t)&_c_int00,
    .release_ver_major = 0,
    .release_ver_minor = 0,
    .reserved = {0, 0, 0, 0, 0, 0, 0},
    .crc_addr = (uint32_t)&__crc_addr,
};
