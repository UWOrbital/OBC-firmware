#include "obc_reset.h"
#include "reg_system.h"
#include "obc_privilege.h"
#include "obc_persistent.h"

#define RESET_SYSTEM_MASK (1 << 15)

void resetSystem(obc_reset_reason_t reason) {
  setPersistentData(OBC_PERSIST_SECTION_ID_RESET_REASON, &reason, sizeof(obc_reset_reason_t));

  BaseType_t xRunningPrivileged = prvRaisePrivilege();

  systemREG1->SYSECR |= RESET_SYSTEM_MASK;

  portRESET_PRIVILEGE(xRunningPrivileged);
}
