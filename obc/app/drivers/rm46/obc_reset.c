#include "obc_reset.h"
#include "reg_system.h"
#include "obc_privilege.h"
#include "obc_persistent.h"
#include "obc_logging.h"

#define RESET_SYSTEM_MASK (1 << 15)

void resetSystem(obc_reset_reason_t reason) {
  obc_error_code_t errCode;
  errCode = setPersistentData(OBC_PERSIST_SECTION_ID_RESET_REASON, &reason, sizeof(obc_reset_reason_t));
  if (errCode != OBC_ERR_CODE_SUCCESS) {
    LOG_ERROR_CODE(errCode);
    vTaskDelay(pdMS_TO_TICKS(25));  // delay to allow for logger to run
  }

  BaseType_t xRunningPrivileged = prvRaisePrivilege();

  systemREG1->SYSECR |= RESET_SYSTEM_MASK;

  portRESET_PRIVILEGE(xRunningPrivileged);
}
