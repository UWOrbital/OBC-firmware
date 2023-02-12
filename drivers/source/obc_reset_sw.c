#include "errata_SSWF021_45_defs.h"
#include "obc_reset_sw.h"
#include "reg_system.h"    

void resetSW(reset_t reason){

    uint32_t* resetAddress = systemREG1 -> SYSECR;
    resetAddress += 0x000F;
    if(reason != NULLREASON){

        *resetAddress = (uint32_t)0x0001;
    }

}