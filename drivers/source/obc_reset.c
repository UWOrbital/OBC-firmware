#include "obc_reset.h"
#include "reg_system.h" 
#include "obc_privilege.h"

// TODO: Save the reason to persistent storage
void resetSystem(obc_reset_reason_t reason){
    
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    
    
    systemREG1->SYSECR |= RESET_ADDR_MASK;

    portRESET_PRIVILEGE(xRunningPrivileged);

    

}
