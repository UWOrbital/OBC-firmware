#pragma once

#include <FreeRTOS.h>
#include <os_portmacro.h>

/**
 * @brief Set processor to privileged mode
 */
extern BaseType_t prvRaisePrivilege(void);

/**
 * @brief Set processor to user mode
 */
#define portRESET_PRIVILEGE(xRunningPrivileged) \
  if (xRunningPrivileged == 0) portSWITCH_TO_USER_MODE()
