#pragma once

typedef enum {
  BL_ERR_CODE_SUCCESS = 0,

  // General errors
  BL_ERR_CODE_INVALID_ARG = 1,
  BL_ERR_CODE_UNKNOWN = 2,

  // F021 Flash API errors
  BL_ERR_CODE_FAPI_INIT = 100,
  BL_ERR_CODE_FAPI_ERASE = 101,
  BL_ERR_CODE_FAPI_PROGRAM = 102,

} bl_error_code_t;
