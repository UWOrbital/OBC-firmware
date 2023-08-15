#pragma once

#include "obc_errors.h"

#define TPL5010_WAKE_PIN 0
#define TPL5010_DONE_PIN 1

obc_error_code_t tpl5010Init(void);

obc_error_code_t tpl5010Reset(void);
