#pragma once

typedef enum {
  /* Common error codes 0-100 */
  OBC_GS_ERR_CODE_SUCCESS = 0,
  OBC_GS_ERR_CODE_INVALID_ARG = 1,
  OBC_GS_ERR_CODE_BUFF_TOO_SMALL = 3,

  /* Pack/Unpack error codes 100-200 */
  OBC_GS_ERR_CODE_UNSUPPORTED_CMD = 2,

  /* AES error codes 200-300 */

  /* FEC error codes 300-400 */
  OBC_GS_ERR_CODE_REED_SOL_ENC_ERR = 300,
  OBC_GS_ERR_CODE_REED_SOL_DEC_ERR = 301,

  /* AX25 error codes 400-500 */
  OBC_GS_ERR_CODE_INVALID_AX25_PACKET = 400,
  OBC_GS_ERR_CODE_CORRUPTED_AX25_MSG = 401,

} obc_gs_error_code_t;
