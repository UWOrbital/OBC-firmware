#pragma once

#include <FreeRTOS.h>
#include <os_queue.h>
#include "obc_errors.h"

extern QueueHandle_t rtcTempQueueHandle;

obc_error_code_t postRtcTempQueue();
