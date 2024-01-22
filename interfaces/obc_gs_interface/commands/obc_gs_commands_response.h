#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define CMD_RESPONSE_SUCCESS_MASK 0x01
#define CMD_RESPONSE_MAX_PACKED_SIZE sizeof(cmd_unpacked_response_t)

typedef uint8_t cmd_callback_encoded_t;

typedef enum {
  execObCResetCmd = 0x0,
  rtlSyncCmd,
  downlinkLogsCmd,
  microSDCmd,
  pingCmd,
  donwlinkTelemCmd,
  NUM_CMD_CALLBACKS
} cmd_callback_id_t;

typedef struct {
  float data1;
  uint32_t data2;
} ObcCmdResetCmdResponse;

typedef struct {
  bool success;
  cmd_callback_id_t cmdId;
  union {
    ObcCmdResetCmdResponse obcResetResponse;
  };
} cmd_unpacked_response_t;
