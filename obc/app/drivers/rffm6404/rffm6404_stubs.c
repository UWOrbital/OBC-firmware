#ifndef CONFIG_RFFM6404
#include "obc_errors.h"
#include "obc_general_util.h"
#include "rffm6404.h"

obc_error_code_t rffm6404ActivateRx(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t rffm6404ActivateTx(float voltagePowerControl) {
  UNUSED(voltagePowerControl);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t rffm6404ActivateRecvByp(void) { return OBC_ERR_CODE_SUCCESS; }

obc_error_code_t rffm6404PowerOff(void) { return OBC_ERR_CODE_SUCCESS; }

#endif  // CONFIG_RFFM6404
