#ifndef _OBC_PRIVILEGE_H_
#define _OBC_PRIVILEGE_H_

#include "os_portmacro.h"

extern BaseType_t prvRaisePrivilege(void);

#define portRESET_PRIVILEGE(xRunningPrivileged) if( xRunningPrivileged == 0 ) portSWITCH_TO_USER_MODE()

#endif //_OBC_PRIVILEGE_H_