#pragma once

typedef enum {
  GS_ERR_CODE_SUCCESS,
  GS_ERR_CODE_CORRUPTED_MSG,
  GS_ERR_CODE_AX25_DECODE_FAILURE,
  GS_ERR_CODE_AX25_ENCODE_FAILURE,
  GS_ERR_CODE_UNRECOGNIZED_TRANSITION,
  GS_ERR_CODE_INVALID_STATE,
  GS_ERR_CODE_INVALID_ARG,
  GS_ERR_CODE_INVALID_BUFFER
} gs_error_code_t;

#define RETURN_IF_ERROR_CODE(_ret)        \
  do {                                    \
    errCode = _ret;                       \
    if (errCode != GS_ERR_CODE_SUCCESS) { \
      return errCode;                     \
    }                                     \
  } while (0)

#define AX25_RETURN_IF_ERROR_CODE(_ret)           \
  do {                                            \
    ax25errCode = _ret;                           \
    if (ax25errCode != OBC_GS_ERR_CODE_SUCCESS) { \
      return ax25errCode;                         \
    }                                             \
  } while (0)
