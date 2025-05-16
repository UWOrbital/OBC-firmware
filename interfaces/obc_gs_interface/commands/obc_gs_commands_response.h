#pragma once

#include "obc_gs_command_id.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define CMD_RESPONSE_SUCCESS_MASK 0x01
#define CMD_RESPONSE_MAX_PACKED_SIZE sizeof(cmd_unpacked_response_t)

/*---------------------------------------------------------------------------*/
/* GUIDE FOR ADDING A NEW COMMAND RESPONSE
 * 1. Create new response struct
 *     - Create a new struct such as obc_command_response_t below to define.
 *        what the command response must look like. The command_response_error_code_t enum case
 *        must be already defind for the error you are trying to implement.
 *     - Add the command response struct to the union type of cmd_unpacked_response_t.
 * 2. In both obc_gs_commands_response_unpack.c and obc_gs_commands_response_pack.c define the callback function
 *      - For each command callback you must define a callback function that should be executed to pack the
 *        struct that you declared in the union type of cmd_unpacked_response_t struct below.
 *      - You must use the data_pack_unpack interface to avoid issues with endianess. The callback is executed after
 *        the cmdId is passed from the cmd_unpacked_response_t struct.
 *---------------------------------------------------------------------------*/

typedef struct {
  float data1;
  uint32_t data2;
} obc_cmd_reset_response_t;  // This is only a sample response. Implement an actual one

// NOTE: Update python error codes as well when these are updated
typedef enum { CMD_RESPONSE_SUCCESS = 0x0, CMD_RESPONSE_ERROR } cmd_response_error_code_t;

typedef struct {
  cmd_response_error_code_t errCode;
  cmd_callback_id_t cmdId;
  union {
    obc_cmd_reset_response_t obcResetResponse;
  };
} cmd_unpacked_response_t;
