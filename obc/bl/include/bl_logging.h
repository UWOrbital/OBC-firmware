#pragma once

#include <stdint.h>
#include <stdarg.h>

#define LOG_ERROR_CODE(errCode)  // DO nothing ATM

#define RETURN_IF_ERROR_CODE(_ret)         \
  do {                                     \
    errCode = _ret;                        \
    if (errCode != OBC_ERR_CODE_SUCCESS) { \
      LOG_ERROR_CODE(errCode);             \
      return errCode;                      \
    }                                      \
  } while (0)

#define LOG_IF_ERROR_CODE(_ret)            \
  do {                                     \
    errCode = _ret;                        \
    if (errCode != OBC_ERR_CODE_SUCCESS) { \
      LOG_ERROR_CODE(errCode);             \
    }                                      \
  } while (0)
