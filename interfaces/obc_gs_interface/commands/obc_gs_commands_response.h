#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "obc_errors.h"

#define CMD_ARGUMENT_SIZE 1U

#define CMD_ID_SHIFT 4
#define CMD_ID_MASK 0xF0

#define CMD_RESPONSE_SUCCESS_MASK 0x01

typedef uint8_t cmd_callback_encoded_t;

typedef enum {
  execObCResetCmd = 0x0,
  rtlSyncCmd,
  downlinkLogsCmd,
  microSDCmd,
  pingCmd,
  donwlinkTelemCmd
} cmd_callback_id_t;

typedef struct {
  bool success;
  cmd_callback_id_t cmdId;
  union {
    struct execObCResetCmd {};
  };
} cmd_unpacked_response_t;
