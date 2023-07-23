#pragma once

#include "obc_persistent_data_config.h"
#include "obc_errors.h"

obc_error_code_t getPersistentObcTime(obc_time_persist_data_t *buffer);
obc_error_code_t setPersistentObcTime(obc_time_persist_data_t *data);
