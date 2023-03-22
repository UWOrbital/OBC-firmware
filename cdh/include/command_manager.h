#ifndef CDH_INCLUDE_COMMAND_MANAGER_H_
#define CDH_INCLUDE_COMMAND_MANAGER_H_

#include "obc_errors.h"
#include "command_data.h"

void initCommandManager(void);

obc_error_code_t sendToCommandQueue(cmd_msg_t *cmd);

#endif /* CDH_INCLUDE_COMMAND_MANAGER_H_ */