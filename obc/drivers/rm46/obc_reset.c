#include "obc_reset.h"
#include "reg_system.h"
#include "obc_privilege.h"
#include "obc_persistent.h"
#include "obc_logging.h"

#define RESET_SYSTEM_MASK (1 << 15)

// TODO: Save the reason to persistent storage

void resetSystem(obc_reset_reason_t reason) {
  BaseType_t xRunningPrivileged = prvRaisePrivilege();

  systemREG1->SYSECR |= RESET_SYSTEM_MASK;

  portRESET_PRIVILEGE(xRunningPrivileged);

  setPersistentResetReason(reason);
  // do I put this here?
  switch (reason) {
    case RESET_REASON_STACK_CHECK_FAIL:
      LOG_ERROR_CODE(OBC_ERR_CODE_STACK_CHECK_FAIL);
    case RESET_REASON_FS_FAILURE:
      LOG_ERROR_CODE(OBC_ERR_CODE_FS_FAILURE);
    case RESET_REASON_CMD_EXEC_OBC_RESET:
      LOG_ERROR_CODE(OBC_ERR_CODE_CMD_EXEC_OBC_RESET);
  }
}
