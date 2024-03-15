#include "logger.h"
#include "obc_gs_errors.h"
#include <stdio.h>

int main() {
  printf("Interfaces\n");
  obc_gs_error_code_t errCode;
  LOG_IF_ERROR_CODE(OBC_GS_ERR_CODE_UNKNOWN);
  return 0;
}
