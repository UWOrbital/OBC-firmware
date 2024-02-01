#pragma once

#include "obc_gs_command_id.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define CMD_RESPONSE_SUCCESS_MASK 0x01
#define CMD_RESPONSE_MAX_PACKED_SIZE sizeof(cmd_unpacked_response_t)

typedef uint8_t cmd_callback_encoded_t;

typedef enum {
  EXEC_OBC_RESET_CMD,
  RTL_SYNC_CMD,
  DOWNLINK_LOGS_CMD,
  MICRO_SD_CMD,
  PING_CMD,
  DOWNLINK_TELEM_CMD,
  NUM_CMD_CALLBACKS
} cmd_callback_id_t;

typedef struct {
  float data1;
  uint32_t data2;
} obc_cmd_reset_response_t;

typedef enum { CMD_RESPONSE_SUCCESS = 0x0 } cmd_response_error_code_t;

typedef struct {
  cmd_response_error_code_t errCode;
  cmd_callback_id_t cmdId;
  union {
    obc_cmd_reset_response_t obcResetResponse;
  };
} cmd_unpacked_response_t;
