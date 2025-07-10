#pragma once
#include <stdint.h>

typedef struct __attribute__((packed)) {
  uint16_t magic_num;            // Something
  uint32_t build_num;            // 0
  uint64_t git_hash;             // 0
  uint32_t app_entry_func_addr;  // _c_int00
  uint8_t release_ver_major;     // 0
  uint8_t release_ver_minor;     // 0
  uint8_t board_id;              // Find value from CMake and match boards before crc check
  uint8_t reserved[7];
  uint32_t crc_addr;  // crc
} metadata_t;
