#pragma once

#include "obc_gs_command_id.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define CMD_RESPONSE_SUCCESS_MASK 0x01
#define CMD_RESPONSE_MAX_PACKED_SIZE sizeof(cmd_unpacked_response_t)

typedef struct {
  float data1;
  uint32_t data2;
} obc_cmd_reset_response_t;  // This is only a sample response. Implement an actual one

typedef enum { CMD_RESPONSE_SUCCESS = 0x0, CMD_RESPONSE_ERROR } cmd_response_error_code_t;

typedef struct {
  cmd_response_error_code_t errCode;
  cmd_callback_id_t cmdId;
  union {
    obc_cmd_reset_response_t obcResetResponse;
  };
} cmd_unpacked_response_t;
