#ifndef COMMON_INCLUDE_OBC_PERSISTENT_STORE_H_
#define COMMON_INCLUDE_OBC_PERSISTENT_STORE_H_

#include "obc_persistent_data_config.h"
#include "obc_errors.h"

obc_error_code_t getPersistentObcTime(obc_time_persist_data_t *buffer);
obc_error_code_t setPersistentObcTime(obc_time_persist_data_t *data);

obc_error_code_t getPersistentObcState(obc_state_persist_data_t *buffer);
obc_error_code_t setPersistentObcState(obc_state_persist_data_t *data);

obc_error_code_t getPersistentObcConfig(obc_config_persist_data_t *buffer);
obc_error_code_t setPersistentObcConfig(obc_config_persist_data_t *data);

#endif //COMMON_INCLUDE_OBC_PERSISTENT_STORE_H_
