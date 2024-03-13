#pragma once

typedef enum {
  GS_ERR_CODE_SUCCESS,
  GS_ERR_CODE_CORRUPTED_MSG,
  GS_ERR_CODE_AX25_DECODE_FAILURE,
  GS_ERR_CODE_INVALID_ARG,
  GS_ERR_CODE_LOG_MSG_SILENCED,
  GS_ERR_CODE_BUFF_TOO_SMALL,

  /* File System errors */
  GS_ERR_CODE_FAILED_FILE_OPEN,
  GS_ERR_CODE_FAILED_FILE_CLOSE,
  GS_ERR_CODE_FAILED_FILE_WRITE,

} gs_error_code_t;
