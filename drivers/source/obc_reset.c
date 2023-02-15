#include "obc_reset.h"
#include "reg_system.h" 
#include "obc_privilege.h"

uint32_t ResetAddrMask = (1 << 15);

// TODO: Save the reason to persistent storage
void resetSystem(obc_reset_reason_t reason){
    
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    
    
    systemREG1->SYSECR |= ResetAddrMask;

    portRESET_PRIVILEGE(xRunningPrivileged);

    

}
