#include "obc_reset_sw.h"
#include "reg_system.h" 
#include "obc_privilege.h"

void resetSW(obc_reset_reason_t reason){
    
    BaseType_t xRunningPrivileged = prvRaisePrivilege();
    portRESET_PRIVILEGE(xRunningPrivileged);

    uint32_t resetAddress = systemREG1 -> SYSECR;
    
    if(reason != RESET_REASON_TESTIG){

        systemREG1->SYSECR =  resetAddress | RESET_ADDR_MASK;
    }

}