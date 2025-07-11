#include "obc_metadata.h"
#include "obc_errors.h"

#if defined(BOARD_TYPE_TO_INT_MAP)
#define BOARD_ID BOARD_TYPE_TO_INT_MAP
#else
#define BOARD_ID OBC_ERR_CODE_BOARD_NOT_DEFINED
#endif

extern void _c_int00(void);
extern uint32_t __crc_addr;

const metadata_t app_metadata __attribute__((section(".metadata"), used)) = {
    .magic_num = 0xFFFF,
    .build_num = 0,
    .git_hash = 0,
    .app_entry_func_addr = (uint32_t)&_c_int00,
    .release_ver_major = 0,
    .release_ver_minor = 0,
    .board_id = BOARD_ID,
    .reserved = {0, 0, 0, 0, 0, 0, 0},
    .crc_addr = (uint32_t)&__crc_addr,
};
