#include "obc_reset.h"
#include "reg_system.h"
#include "obc_privilege.h"
#include "obc_persistent.h"
#include "obc_logging.h"

#define RESET_SYSTEM_MASK (1 << 15)

// TODO: Save the reason to persistent storage

void resetSystem(obc_reset_reason_t reason) {
  obc_reset_reason_persist_data_t resetReason;
  resetReason.reason = reason;

  setPersistentResetReason(&resetReason);

  BaseType_t xRunningPrivileged = prvRaisePrivilege();

  systemREG1->SYSECR |= RESET_SYSTEM_MASK;

  portRESET_PRIVILEGE(xRunningPrivileged);
}
