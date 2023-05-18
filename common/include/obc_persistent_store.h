#ifndef COMMON_INCLUDE_OBC_PERSISTENT_STORE_H_
#define COMMON_INCLUDE_OBC_PERSISTENT_STORE_H_

#include "obc_persistent_data_config.h"
#include "obc_errors.h"

obc_error_code_t getPersistentSysData(fram_sys_data_t *buffer);
obc_error_code_t setPersistentSysData(fram_sys_data_t data);

obc_error_code_t getPersistentADCSData(fram_adcs_data_t *buffer);
obc_error_code_t setPersistentADCSData(fram_adcs_data_t data);

obc_error_code_t getPersistentCOMMSData(fram_comms_data_t *buffer);
obc_error_code_t setPersistentCOMMSData(fram_comms_data_t data);

obc_error_code_t getPersistentCDHData(fram_cdh_data_t *buffer);
obc_error_code_t setPersistentCDHData(fram_cdh_data_t data);

obc_error_code_t getPersistentEPSData(fram_eps_data_t *buffer);
obc_error_code_t setPersistentEPSData(fram_eps_data_t data);

#endif //COMMON_INCLUDE_OBC_PERSISTENT_STORE_H_