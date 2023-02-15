#ifndef COMMON_INCLUDE_OBC_PRIVILEGE_H_
#define COMMON_INCLUDE_OBC_PRIVILEGE_H_

#include "os_portmacro.h"

/**
 * @brief Set processor to privileged mode
 */
extern BaseType_t prvRaisePrivilege(void);

/**
 * @brief Set processor to user mode
 */
#define portRESET_PRIVILEGE(xRunningPrivileged) if( xRunningPrivileged == 0 ) portSWITCH_TO_USER_MODE()

#endif //COMMON_INCLUDE_OBC_PRIVILEGE_H_
